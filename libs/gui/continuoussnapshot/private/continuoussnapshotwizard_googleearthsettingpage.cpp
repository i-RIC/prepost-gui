#include "../../main/iricmainwindow.h"
#include "continuoussnapshotwizard_googleearthsettingpage.h"
#include "ui_continuoussnapshotwizard_googleearthsettingpage.h"

#include <cs/coordinatesystem.h>
#include <guicore/post/postprocessorwindow.h>
#include <guicore/project/backgroundimageinfo.h>
#include <guicore/project/projectmainfile.h>
#include <misc/mathsupport.h>
#include <post/post2d/post2dwindow.h>
#include <post/post2d/post2dwindowgraphicsview.h>

#define _USE_MATH_DEFINES
#include <math.h>

ContinuousSnapshotWizard::GoogleEarthSettingPage::GoogleEarthSettingPage(ContinuousSnapshotWizard *wizard) :
	QWizardPage(wizard),
	m_mainWindow {dynamic_cast<iRICMainWindow*> (wizard->parent())},
	m_wizard {wizard},
	ui(new Ui::ContinuousSnapshotWizard_GoogleEarthSettingPage)
{
	ui->setupUi(this);
}

ContinuousSnapshotWizard::GoogleEarthSettingPage::~GoogleEarthSettingPage()
{
	delete ui;
}

void ContinuousSnapshotWizard::GoogleEarthSettingPage::calculateKMLInformation()
{
	auto setting = m_wizard->setting();

	QPointF center, centerRight;
	auto snapshotSize = targetSnapshotSize();

	center = QPointF(snapshotSize.width() / 2., snapshotSize.height() / 2.);
	centerRight = QPointF(snapshotSize.width(), snapshotSize.height() / 2.);

	auto centerXY = snapshotToWorld(center);
	auto centerRightXY = snapshotToWorld(centerRight);

	// calculate angle
	QPointF dv = centerRightXY - centerXY;
	double angle = std::atan(dv.y() / dv.x());
	setting.angle = (angle * 180 / M_PI);

	double halfWidth = iRIC::length(snapshotToWorld(QPointF(snapshotSize.width(), snapshotSize.height() / 2.)) - centerXY);
	double halfHeight = iRIC::length(snapshotToWorld(QPointF(snapshotSize.width() / 2., snapshotSize.height())) - centerXY);

	auto leftBottomXY = centerXY - QPointF(halfWidth, halfHeight);
	auto rightTopXY = centerXY + QPointF(halfWidth, halfHeight);

	auto leftBottomLatLon = worldToLatLong(leftBottomXY);
	auto rightTopLatLon = worldToLatLong(rightTopXY);

	setting.north = rightTopLatLon.y();
	setting.south = leftBottomLatLon.y();
	setting.east = rightTopLatLon.x();
	setting.west = leftBottomLatLon.x();

	m_wizard->setSetting(setting);
}

QSize ContinuousSnapshotWizard::GoogleEarthSettingPage::targetSnapshotSize()
{
	const auto& s = m_wizard->setting();

	QSize ret;
	switch (s.fileOutputSetting) {
	case ContinuousSnapshotSetting::FileOutputSetting::Onefile:
		ret.setWidth(m_wizard->snapshotSize().width());
		ret.setHeight(m_wizard->snapshotSize().height());
		break;

	case ContinuousSnapshotSetting::FileOutputSetting::Respectively:
		auto it = m_wizard->windowList().begin() + m_wizard->targetWindow();
		QWidget* w = dynamic_cast<QMainWindow*>((*it)->widget())->centralWidget();
		ret.setWidth(w->width());
		ret.setHeight(w->height());
		break;
	}
	return ret;
}

void ContinuousSnapshotWizard::GoogleEarthSettingPage::initializePage()
{
	auto setting = m_wizard->setting();

	auto wl = m_wizard->windowList();
	int windowIndex = 0;
	for (int i = 0; i < wl.size(); ++i) {
		auto w = wl.at(i);
		auto postW = dynamic_cast<Post2dWindow*> (w->widget());
		if (postW != nullptr) {
			ui->post2dComboBox->addItem(postW->windowTitle());
			m_post2dWindowIndices.push_back(i);
			if (postW->windowTitle() == setting.post2dWindowTitle) {
				windowIndex = m_post2dWindowIndices.size() - 1;
			}
		}
	}

	if (m_wizard->projectMainFile()->coordinateSystem() == nullptr || m_post2dWindowIndices.size() == 0) {
		ui->googleEarthCheckBox->setChecked(false);
		this->setEnabled(false);
	} else {
		ui->googleEarthCheckBox->setChecked(setting.outputKml);
		ui->kmlEdit->setText(setting.kmlFilename);
		ui->post2dComboBox->setCurrentIndex(windowIndex);

		this->setEnabled(true);
	}
}

bool ContinuousSnapshotWizard::GoogleEarthSettingPage::validatePage()
{
	auto setting = m_wizard->setting();
	setting.outputKml = ui->googleEarthCheckBox->isChecked();
	setting.kmlFilename = ui->kmlEdit->text();

	auto windowIndex = m_post2dWindowIndices.at(ui->post2dComboBox->currentIndex());
	auto windowTitle = m_wizard->windowList().at(windowIndex)->widget()->windowTitle();
	setting.post2dWindowTitle = windowTitle;

	m_wizard->setSetting(setting);
	m_wizard->setTargetWindow(windowIndex);

	if (! setting.outputKml) {return true;}

	// calculate angle/north/south/east/west
	calculateKMLInformation();

	return true;
}

QPointF ContinuousSnapshotWizard::GoogleEarthSettingPage::snapshotToWorld(const QPointF& p)
{
	QList<QMdiSubWindow*>::const_iterator it;
	QWidget* center;
	int target = m_wizard->targetWindow();
	int idx;
	int tmpX = 0;
	int tmpY = 0;
	QPoint offset(0, 0);

	const auto s = m_wizard->setting();

	switch (s.fileOutputSetting) {
	case ContinuousSnapshotSetting::FileOutputSetting::Onefile:
		// calculate the offset
		switch (s.outputLayout) {
		case ContinuousSnapshotSetting::OutputLayout::AsIs:
			it = m_wizard->windowList().begin() + target;
			center = dynamic_cast<QMainWindow*>((*it)->widget())->centralWidget();
			offset = (*it)->pos() + (*it)->widget()->pos() + center->pos();
			offset -= m_wizard->beginPosition();
			break;
		case ContinuousSnapshotSetting::OutputLayout::Horizontally:
			for (it = m_wizard->windowList().begin(), idx = 0;
					 it != m_wizard->windowList().end(), idx < target;
					 ++it, ++idx) {
				center = dynamic_cast<QMainWindow*>((*it)->widget())->centralWidget();
				tmpX += center->width();
			}
			offset.setX(tmpX);
			break;
		case ContinuousSnapshotSetting::OutputLayout::Vertically:
			for (it = m_wizard->windowList().begin(), idx = 0;
					 it != m_wizard->windowList().end(), idx < target;
					 ++it, ++idx) {
				center = dynamic_cast<QMainWindow*>((*it)->widget())->centralWidget();
				tmpY += center->height();
			}
			offset.setY(tmpY);
			break;
		}
		break;
	case ContinuousSnapshotSetting::FileOutputSetting::Respectively:
		it = m_wizard->windowList().begin() + target;
		break;
	}

	// the const_iterator "it" should point the target window
	auto w = dynamic_cast<QMainWindow*>((*it)->widget())->centralWidget();
	auto view = dynamic_cast<Post2dWindowGraphicsView*>(w);
	double worldX = p.x() - offset.x();
	double worldY = p.y() - offset.y();

	view->viewportToWorld(worldX, worldY);

	return QPointF(worldX, worldY);
}

QPointF ContinuousSnapshotWizard::GoogleEarthSettingPage::worldToLatLong(const QPointF& p)
{
	auto cs = m_wizard->projectMainFile()->coordinateSystem();
	double x, y, lon, lat;
	x = p.x();
	y = p.y();

	cs->mapGridToGeo(x, y, &lon, &lat);
	return QPointF(lon, lat);
}

QPointF ContinuousSnapshotWizard::GoogleEarthSettingPage::snapshotToLatLong(const QPointF& p)
{
	return worldToLatLong(snapshotToWorld(p));
}
