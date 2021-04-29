#include "ui_continuoussnapshotgoogleearthsettingpage.h"

#include "../main/iricmainwindow.h"
#include "continuoussnapshotgoogleearthsettingpage.h"
#include "continuoussnapshotwizard.h"

#define _USE_MATH_DEFINES
#include <cmath>

#include <cs/coordinatesystem.h>
#include <guicore/post/postprocessorwindow.h>
#include <guicore/project/backgroundimageinfo.h>
#include <guicore/project/projectmainfile.h>
#include <misc/mathsupport.h>
#include <post/post2d/post2dwindow.h>
#include <post/post2d/post2dwindowgraphicsview.h>

#include <QMdiSubWindow>

ContinuousSnapshotGoogleEarthSettingPage::ContinuousSnapshotGoogleEarthSettingPage(QWidget* parent) :
	QWizardPage(parent),
	ui(new Ui::ContinuousSnapshotGoogleEarthSettingPage)
{
	ui->setupUi(this);

	m_wizard = dynamic_cast<ContinuousSnapshotWizard*>(parent);
	m_mainWindow = dynamic_cast<iRICMainWindow*>(m_wizard->parent());
}

ContinuousSnapshotGoogleEarthSettingPage::~ContinuousSnapshotGoogleEarthSettingPage()
{
	delete ui;
}

void ContinuousSnapshotGoogleEarthSettingPage::initializePage()
{
	auto setting = m_wizard->setting();

	if (m_wizard->projectMainFile()->coordinateSystem() == nullptr) {
		ui->googleEarthCheckBox->setChecked(false);
		this->setEnabled(false);
	} else {
		ui->googleEarthCheckBox->setChecked(setting.outputKml);
		ui->kmlEdit->setText(setting.kmlFilename);
		this->setEnabled(true);
	}
}

bool ContinuousSnapshotGoogleEarthSettingPage::validatePage()
{
	auto setting = m_wizard->setting();
	setting.outputKml = ui->googleEarthCheckBox->isChecked();
	setting.kmlFilename = ui->kmlEdit->text();
	m_wizard->setSetting(setting);

	if (! setting.outputKml) {return true;}

	// calculate angle/north/south/east/west
	calculateKMLInformation();

	return true;
}

void ContinuousSnapshotGoogleEarthSettingPage::snapshotToWorld(QPointF& p)
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
	QWidget* w = dynamic_cast<QMainWindow*>((*it)->widget())->centralWidget();
	Post2dWindowGraphicsView* view = dynamic_cast<Post2dWindowGraphicsView*>(w);
	double worldX = p.x() - offset.x();
	double worldY = p.y() - offset.y();

	view->viewportToWorld(worldX, worldY);
	p.setX(worldX);
	p.setY(worldY);
}

void ContinuousSnapshotGoogleEarthSettingPage::worldToLatLong(QPointF& p)
{
	auto cs = m_wizard->projectMainFile()->coordinateSystem();
	double x, y, lon, lat;
	x = p.x();
	y = p.y();

	cs->mapGridToGeo(x, y, &lon, &lat);
	p.setX(lon);
	p.setY(lat);
}

void ContinuousSnapshotGoogleEarthSettingPage::snapshotToLatLong(QPointF& p)
{
	snapshotToWorld(p);
	worldToLatLong(p);
}

void ContinuousSnapshotGoogleEarthSettingPage::calculateKMLInformation()
{
	auto setting = m_wizard->setting();

	QPointF center, centerRight;
	auto snapshotSize = targetSnapshotSize();

	center = QPointF(snapshotSize.width() / 2., snapshotSize.height() / 2.);
	centerRight = QPointF(snapshotSize.width(), snapshotSize.height() / 2.);
	snapshotToLatLong(center);
	snapshotToLatLong(centerRight);

	// calculate angle (radian)
	QPointF dv = centerRight - center;
	double angle = std::atan(dv.y() / dv.x());
	setting.angle = (angle * 180 / M_PI);

	// calculate north/south/east/west
	QPointF centerV(snapshotSize.width() / 2., snapshotSize.height() / 2.);
	QPointF centerRightV(snapshotSize.width(), snapshotSize.height() / 2.);
	QPointF centerTopV(snapshotSize.width() / 2., 0);

	QPointF dx = centerRightV - centerV;
	QPointF dy = centerTopV - centerV;
	// reflection
	dx.setY(- dx.y());
	dy.setY(- dy.y());

	// rotation
	iRIC::rotateVector(dx, - setting.angle);
	iRIC::rotateVector(dy, - setting.angle);

	// reflection
	dx.setY(- dx.y());
	dy.setY(- dy.y());

	QPointF rightTopV = centerV + dx + dy;
	QPointF leftBottomV = centerV - dx - dy;

	QPointF rightTop(rightTopV.x(), rightTopV.y());
	QPointF leftBottom(leftBottomV.x(), leftBottomV.y());

	snapshotToLatLong(rightTop);
	snapshotToLatLong(leftBottom);

	setting.north = rightTop.y();
	setting.south = leftBottom.y();
	setting.east = rightTop.x();
	setting.west = leftBottom.x();

	m_wizard->setSetting(setting);
}

QSize ContinuousSnapshotGoogleEarthSettingPage::targetSnapshotSize()
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
