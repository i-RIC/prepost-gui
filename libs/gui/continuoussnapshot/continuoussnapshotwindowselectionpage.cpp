#include "ui_continuoussnapshotwindowselectionpage.h"

#include "../main/iricmainwindow.h"
#include "continuoussnapshotwindowselectionpage.h"
#include "continuoussnapshotwizard.h"

#include <guicore/post/postprocessorwindow.h>

#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMessageBox>

ContinuousSnapshotWindowSelectionPage::ContinuousSnapshotWindowSelectionPage(QWidget* parent) :
	QWizardPage(parent),
	ui(new Ui::ContinuousSnapshotWindowSelectionPage)
{
	ui->setupUi(this);

	m_wizard = dynamic_cast<ContinuousSnapshotWizard*>(parent);
	m_mainWindow = dynamic_cast<iRICMainWindow*>(m_wizard->parent());
	setupWindowList();
	m_wizard->setBeginPosition(QPoint(0, 0));
	m_wizard->setSnapshotSize(QSize(0, 0));
}

ContinuousSnapshotWindowSelectionPage::~ContinuousSnapshotWindowSelectionPage()
{
	delete ui;
}

void ContinuousSnapshotWindowSelectionPage::setupWindowList()
{
	QMdiArea* center = dynamic_cast<QMdiArea*>(m_mainWindow->centralWidget());
for (QMdiSubWindow* sub : center->subWindowList(QMdiArea::ActivationHistoryOrder)) {
		PostProcessorWindow* post = dynamic_cast<PostProcessorWindow*>(sub->widget());
		if (post == nullptr) { continue; }
		QListWidgetItem* item = new QListWidgetItem(sub->windowTitle(), ui->targetListWidget);
		item->setCheckState(Qt::Checked);
	}
}

void ContinuousSnapshotWindowSelectionPage::initializePage()
{
	const auto s = m_wizard->setting();
	switch (s.fileOutputSetting) {
	case ContinuousSnapshotSetting::FileOutputSetting::Onefile:
		ui->onefileRadioButton->setChecked(true);
		break;
	case ContinuousSnapshotSetting::FileOutputSetting::Respectively:
		ui->respectivelyRadioButton->setChecked(true);
		break;
	}

	switch (s.outputLayout) {
	case ContinuousSnapshotSetting::OutputLayout::AsIs:
		ui->asisRadioButton->setChecked(true);
		break;
	case ContinuousSnapshotSetting::OutputLayout::Horizontally:
		ui->horizontallyRadioButton->setChecked(true);
		break;
	case ContinuousSnapshotSetting::OutputLayout::Vertically:
		ui->verticallyRadioButton->setChecked(true);
		break;
	}

	if (s.imageIsTransparent) {
		ui->transparentRadioButton->setChecked(true);
	} else {
		ui->whiteRadioButton->setChecked(true);
	}
}

bool ContinuousSnapshotWindowSelectionPage::validatePage()
{
	auto s = m_wizard->setting();

	QMdiArea* center = dynamic_cast<QMdiArea*>(m_mainWindow->centralWidget());
	// Windows
	m_wizard->clearWindowList();
	bool hasTransparent = false;
	for (QMdiSubWindow* sub : center->subWindowList(QMdiArea::ActivationHistoryOrder)) {
		PostProcessorWindow* post = dynamic_cast<PostProcessorWindow*>(sub->widget());
		if (post == nullptr) { continue; }
		QListWidgetItem* item = ui->targetListWidget->findItems(sub->windowTitle(), Qt::MatchExactly).at(0);
		if (item->checkState() == Qt::Checked) {
			m_wizard->addWindowList(sub);
			hasTransparent = hasTransparent || post->hasTransparentPart();
		}
	}
	// Layout
	if (ui->horizontallyRadioButton->isChecked()) {
		s.outputLayout = ContinuousSnapshotSetting::OutputLayout::Horizontally;
	} else if (ui->verticallyRadioButton->isChecked()) {
		s.outputLayout = ContinuousSnapshotSetting::OutputLayout::Vertically;
	} else {
		s.outputLayout = ContinuousSnapshotSetting::OutputLayout::AsIs;
	}
	// How to output
	if (ui->respectivelyRadioButton->isChecked()) {
		s.fileOutputSetting = ContinuousSnapshotSetting::FileOutputSetting::Respectively;
	} else {
		s.fileOutputSetting = ContinuousSnapshotSetting::FileOutputSetting::Onefile;
		QPoint position;
		QSize size;
		measurePixmapSize(position, size);
		m_wizard->setSnapshotSize(size);
		m_wizard->setBeginPosition(position);
	}
	// White or transparent
	if (ui->transparentRadioButton->isChecked()) {
		s.imageIsTransparent = true;
	} else {
		s.imageIsTransparent = false;
	}
	if (ui->transparentRadioButton->isChecked() && hasTransparent) {
		QMessageBox::warning(this, tr("Warning"), tr("More than one visualization window has transparent region. They can not saved into transparent background images. Please select white background, or cancel and disable transparency for contours etc."));
		return false;
	}

	if (m_wizard->windowList().size() == 0) {
		QMessageBox::warning(this, tr("Warning"), tr("No window is currently selected."));
		return false;
	}
	m_wizard->setSetting(s);

	return true;
}

void ContinuousSnapshotWindowSelectionPage::measurePixmapSize(QPoint& p, QSize& s)
{
	auto setting = m_wizard->setting();

	// coordinates in the MdiArea of the main window
	QPoint min;
	QPoint max;

	bool first = true;
	for (QMdiSubWindow* sub : m_wizard->windowList()) {
		// post window graphics view
		QWidget* post = dynamic_cast<QMainWindow*>(sub->widget())->centralWidget();
		QPoint beginPos = sub->pos() + sub->widget()->pos() + post->pos();
		QPoint endPos = beginPos + QPoint(post->width(), post->height());

		int tmpX, tmpY;
		switch (setting.outputLayout) {
		case ContinuousSnapshotSetting::OutputLayout::AsIs:
			if (beginPos.x() < min.x() || first) {
				min.setX(beginPos.x());
			}
			if (beginPos.y() < min.y() || first) {
				min.setY(beginPos.y());
			}
			if (endPos.x() > max.x() || first) {
				max.setX(endPos.x());
			}
			if (endPos.y() > max.y() || first) {
				max.setY(endPos.y());
			}
			break;

		case ContinuousSnapshotSetting::OutputLayout::Horizontally:
			if (first) {
				min = beginPos;
				max = endPos;
				break;
			}
			tmpX = max.x() + post->width();
			tmpY = min.y() + post->height();
			max.setX(tmpX);
			if (tmpY > max.y()) { max.setY(tmpY); }
			break;

		case ContinuousSnapshotSetting::OutputLayout::Vertically:
			if (first) {
				min = beginPos;
				max = endPos;
				break;
			}
			tmpX = min.x() + post->width();
			tmpY = max.y() + post->height();
			max.setY(tmpY);
			if (tmpX > max.x()) { max.setX(tmpX); }
			break;
		}
		first = false;
	}
	p.setX(min.x());
	p.setY(min.y());
	s.setWidth(max.x() - min.x());
	s.setHeight(max.y() - min.y());
}
