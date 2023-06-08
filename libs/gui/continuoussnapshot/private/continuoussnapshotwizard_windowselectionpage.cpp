#include "../../main/iricmainwindow.h"
#include "continuoussnapshotwizard_windowselectionpage.h"
#include "ui_continuoussnapshotwizard_windowselectionpage.h"

#include <guicore/post/postprocessorwindow.h>

ContinuousSnapshotWizard::WindowSelectionPage::WindowSelectionPage(ContinuousSnapshotWizard* wizard) :
	QWizardPage(wizard),
	m_mainWindow {dynamic_cast<iRICMainWindow*> (wizard->parent())},
	m_wizard {wizard},
	ui(new Ui::ContinuousSnapshotWizard_WindowSelectionPage)
{
	ui->setupUi(this);

	setupWindowList();
	m_wizard->setBeginPosition(QPoint(0, 0));
	m_wizard->setSnapshotSize(QSize(0, 0));
}

ContinuousSnapshotWizard::WindowSelectionPage::~WindowSelectionPage()
{
	delete ui;
}

void ContinuousSnapshotWizard::WindowSelectionPage::setupWindowList()
{
	auto setting = m_wizard->setting();
	auto center = dynamic_cast<QMdiArea*>(m_mainWindow->centralWidget());

	for (auto sub : center->subWindowList(QMdiArea::ActivationHistoryOrder)) {
		auto post = dynamic_cast<PostProcessorWindow*>(sub->widget());
		if (post == nullptr) {continue;}

		new QListWidgetItem(sub->windowTitle(), ui->targetListWidget);
	}

	m_wizard->setSetting(setting);
}

void ContinuousSnapshotWizard::WindowSelectionPage::measurePixmapSize(QPoint* p, QSize* s)
{
	auto setting = m_wizard->setting();

	// coordinates in the MdiArea of the main window
	QPoint min;
	QPoint max;

	bool first = true;
	for (auto sub : m_wizard->windowList()) {
		// post window graphics view
		auto post = dynamic_cast<QMainWindow*>(sub->widget())->centralWidget();
		auto beginPos = sub->pos() + sub->widget()->pos() + post->pos();
		auto endPos = beginPos + QPoint(post->width(), post->height());

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
	p->setX(min.x());
	p->setY(min.y());

	s->setWidth(max.x() - min.x());
	s->setHeight(max.y() - min.y());
}

void ContinuousSnapshotWizard::WindowSelectionPage::initializePage()
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

	auto& windowSettings = s.windowSettings;
	for (int i = 0; i < ui->targetListWidget->count(); ++i) {
		auto item = ui->targetListWidget->item(i);
		auto windowTitle = item->text();
		auto it = windowSettings.find(windowTitle);

		Qt::CheckState cs = Qt::CheckState::Checked;
		if (it != windowSettings.end()) {
			if (! it->second.output) {
				cs = Qt::CheckState::Unchecked;
			}
		}
		item->setCheckState(cs);
	}
}
bool ContinuousSnapshotWizard::WindowSelectionPage::validatePage()
{
	auto s = m_wizard->setting();
	auto& oldWindowSettings = s.windowSettings;
	std::map<QString, ContinuousSnapshotSetting::WindowSetting> newWindowSettings;

	auto center = dynamic_cast<QMdiArea*>(m_mainWindow->centralWidget());

	// Windows
	m_wizard->clearWindowList();
	bool hasTransparent = false;
	for (auto sub : center->subWindowList(QMdiArea::ActivationHistoryOrder)) {
		auto post = dynamic_cast<PostProcessorWindow*>(sub->widget());
		if (post == nullptr) {continue;}

		auto item = ui->targetListWidget->findItems(sub->windowTitle(), Qt::MatchExactly).at(0);
		auto it = oldWindowSettings.find(sub->windowTitle());
		std::map<QString, ContinuousSnapshotSetting::WindowSetting>::iterator new_it;

		if (it != oldWindowSettings.end()) {
			auto pair = newWindowSettings.insert({it->first, it->second});
			new_it = pair.first;
		} else {
			ContinuousSnapshotSetting::WindowSetting ws;
			ws.output = true;
			ws.prefix = "";
			auto pair = newWindowSettings.insert({post->windowTitle(), ws});
			new_it = pair.first;
		}

		new_it->second.output = false;

		if (item->checkState() == Qt::Checked) {
			m_wizard->addWindowList(sub);
			hasTransparent = hasTransparent || post->hasTransparentPart();
			new_it->second.output = true;
		}
	}
	s.windowSettings = newWindowSettings;

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
		measurePixmapSize(&position, &size);
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
