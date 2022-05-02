#include "ui_continuoussnapshotgoogleearthsettingpage.h"

#include "../main/iricmainwindow.h"
#include "continuoussnapshotgoogleearthsettingpage.h"
#include "continuoussnapshotwizard.h"

#define _USE_MATH_DEFINES
#include <cmath>

#include <cs/coordinatesystem.h>
#include <guicore/post/postprocessorwindow.h>
#include <guicore/project/backgroundimageinfo.h>
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
	if (m_wizard->coordinateSystem() == nullptr) {
		ui->googleEarthCheckBox->setChecked(false);
		this->setEnabled(false);
	} else {
		ui->googleEarthCheckBox->setChecked(m_wizard->googleEarth());
		ui->kmlEdit->setText(m_wizard->kmlFilename());
		this->setEnabled(true);
	}
}

bool ContinuousSnapshotGoogleEarthSettingPage::validatePage()
{
	m_wizard->setGoogleEarth(ui->googleEarthCheckBox->isChecked());
	m_wizard->setKMLFilename(ui->kmlEdit->text());

	if (! m_wizard->googleEarth()) {return true;}

	// calculate angle/north/south/east/west
	calculateKMLInformation();

	return true;
}

QPointF ContinuousSnapshotGoogleEarthSettingPage::snapshotToWorld(const QPointF& p)
{
	QList<QMdiSubWindow*>::const_iterator it;
	QWidget* center;
	int target = m_wizard->targetWindow();
	int idx;
	int tmpX = 0;
	int tmpY = 0;
	QPoint offset(0, 0);
	switch (m_wizard->output()) {
	case ContinuousSnapshotWizard::Onefile:
		// calculate the offset
		switch (m_wizard->layout()) {
		case ContinuousSnapshotWizard::Asis:
			it = m_wizard->windowList().begin() + target;
			center = dynamic_cast<QMainWindow*>((*it)->widget())->centralWidget();
			offset = (*it)->pos() + (*it)->widget()->pos() + center->pos();
			offset -= m_wizard->beginPosition();
			break;
		case ContinuousSnapshotWizard::Horizontally:
			for (it = m_wizard->windowList().begin(), idx = 0;
			     it != m_wizard->windowList().end(), idx < target;
			     ++it, ++idx) {
				center = dynamic_cast<QMainWindow*>((*it)->widget())->centralWidget();
				tmpX += center->width();
			}
			offset.setX(tmpX);
			break;
		case ContinuousSnapshotWizard::Vertically:
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
	case ContinuousSnapshotWizard::Respectively:
		it = m_wizard->windowList().begin() + target;
		break;
	}

	// the const_iterator "it" should point the target window
	QWidget* w = dynamic_cast<QMainWindow*>((*it)->widget())->centralWidget();
	Post2dWindowGraphicsView* view = dynamic_cast<Post2dWindowGraphicsView*>(w);
	double worldX = p.x() - offset.x();
	double worldY = p.y() - offset.y();

	view->viewportToWorld(worldX, worldY);

	return QPointF(worldX, worldY);
}

QPointF ContinuousSnapshotGoogleEarthSettingPage::worldToLatLong(const QPointF& p)
{
	auto cs = m_wizard->coordinateSystem();
	double x, y, lon, lat;
	x = p.x();
	y = p.y();

	cs->mapGridToGeo(x, y, &lon, &lat);
	return QPointF(lon, lat);
}

QPointF ContinuousSnapshotGoogleEarthSettingPage::snapshotToLatLong(const QPointF& p)
{
	return worldToLatLong(snapshotToWorld(p));
}

void ContinuousSnapshotGoogleEarthSettingPage::calculateKMLInformation()
{
	QPointF center, centerRight;
	auto snapshotSize = targetSnapshotSize();

	center = QPointF(snapshotSize.width() / 2., snapshotSize.height() / 2.);
	centerRight = QPointF(snapshotSize.width(), snapshotSize.height() / 2.);

	auto centerXY = snapshotToWorld(center);
	auto centerRightXY = snapshotToWorld(centerRight);

	// calculate angle
	QPointF dv = centerRightXY - centerXY;
	double angle = std::atan(dv.y() / dv.x());
	m_wizard->setAngle(angle * 180 / M_PI);

	double halfWidth = iRIC::length(snapshotToWorld(QPointF(snapshotSize.width(), snapshotSize.height() / 2.)) - centerXY);
	double halfHeight = iRIC::length(snapshotToWorld(QPointF(snapshotSize.width() / 2., snapshotSize.height())) - centerXY);

	auto leftBottomXY = centerXY - QPointF(halfWidth, halfHeight);
	auto rightTopXY = centerXY + QPointF(halfWidth, halfHeight);

	auto leftBottomLatLon = worldToLatLong(leftBottomXY);
	auto rightTopLatLon = worldToLatLong(rightTopXY);

	m_wizard->setNorth(rightTopLatLon.y());
	m_wizard->setSouth(leftBottomLatLon.y());
	m_wizard->setEast(rightTopLatLon.x());
	m_wizard->setWest(leftBottomLatLon.x());
}

QSize ContinuousSnapshotGoogleEarthSettingPage::targetSnapshotSize()
{
	QSize ret;
	switch (m_wizard->output()) {
	case ContinuousSnapshotWizard::Onefile:
		ret.setWidth(m_wizard->snapshotSize().width());
		ret.setHeight(m_wizard->snapshotSize().height());
		break;
	case ContinuousSnapshotWizard::Respectively:
		auto it = m_wizard->windowList().begin() + m_wizard->targetWindow();
		QWidget* w = dynamic_cast<QMainWindow*>((*it)->widget())->centralWidget();
		ret.setWidth(w->width());
		ret.setHeight(w->height());
		break;
	}
	return ret;
}
