#include "ui_continuoussnapshotgoogleearthsettingpage.h"

#include "../main/iricmainwindow.h"
#include "continuoussnapshotgoogleearthsettingpage.h"
#include "continuoussnapshotwizard.h"

#define _USE_MATH_DEFINES
#include <cmath>

#include <guicore/post/postprocessorwindow.h>
#include <guicore/project/backgroundimageinfo.h>
#include <misc/mathsupport.h>
#include <post/post2d/post2dwindow.h>
#include <post/post2d/post2dwindowgraphicsview.h>

#include <QMdiSubWindow>
#include <QVector2D>

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
	ui->googleEarthCheckBox->setChecked(m_wizard->googleEarth());
	setupWindowComboBox();
	setupBackgroundComboBox();

	if (m_wizard->backgroundList().size() == 0) {
		ui->googleEarthCheckBox->setChecked(false);
		this->setEnabled(false);
		return;
	} else {
		this->setEnabled(true);
	}

	int d, m;
	double tmp;
	// bottom left latitude
	tmp = m_wizard->leftLatitude();
	if (tmp < 0) {
		// North (+) / South (-)
		ui->leftLatitudeComboBox->setCurrentIndex(1);
		tmp = - tmp;
	} else {
		ui->leftLatitudeComboBox->setCurrentIndex(0);
	}
	d = static_cast<int>(tmp);
	tmp = (tmp - d) * 60.;
	m = static_cast<int>(tmp);
	tmp = (tmp - m) * 60.;
	ui->leftLatitudeDegreeSpinBox->setValue(d);
	ui->leftLatitudeMinuteSpinBox->setValue(m);
	ui->leftLatitudeSecondSpinBox->setValue(tmp);
	// bottom left longitude
	tmp = m_wizard->leftLongitude();
	if (tmp < 0) {
		// East (+) / West (-)
		ui->leftLongitudeComboBox->setCurrentIndex(1);
		tmp = - tmp;
	} else {
		ui->leftLongitudeComboBox->setCurrentIndex(0);
	}
	d = static_cast<int>(tmp);
	tmp = (tmp - d) * 60.;
	m = static_cast<int>(tmp);
	tmp = (tmp - m) * 60.;
	ui->leftLongitudeDegreeSpinBox->setValue(d);
	ui->leftLongitudeMinuteSpinBox->setValue(m);
	ui->leftLongitudeSecondSpinBox->setValue(tmp);

	// bottom right latitude
	tmp = m_wizard->rightLatitude();
	if (tmp < 0) {
		ui->rightLatitudeComboBox->setCurrentIndex(1);
		tmp = - tmp;
	} else {
		ui->rightLatitudeComboBox->setCurrentIndex(0);
	}
	d = static_cast<int>(tmp);
	tmp = (tmp - d) * 60.;
	m = static_cast<int>(tmp);
	tmp = (tmp - m) * 60.;
	ui->rightLatitudeDegreeSpinBox->setValue(d);
	ui->rightLatitudeMinuteSpinBox->setValue(m);
	ui->rightLatitudeSecondSpinBox->setValue(tmp);
	// bottom right longitude
	tmp = m_wizard->rightLongitude();
	if (tmp < 0) {
		ui->rightLongitudeComboBox->setCurrentIndex(1);
		tmp = - tmp;
	} else {
		ui->rightLongitudeComboBox->setCurrentIndex(0);
	}
	d = static_cast<int>(tmp);
	tmp = (tmp - d) * 60.;
	m = static_cast<int>(tmp);
	tmp = (tmp - m) * 60.;
	ui->rightLongitudeDegreeSpinBox->setValue(d);
	ui->rightLongitudeMinuteSpinBox->setValue(m);
	ui->rightLongitudeSecondSpinBox->setValue(tmp);

	// kml file name
	ui->kmlEdit->setText(m_wizard->kmlFilename());
}

bool ContinuousSnapshotGoogleEarthSettingPage::validatePage()
{
	if (! ui->googleEarthCheckBox->isChecked()) { return true; }

	// set the latitude and longitude
	int flag;
	m_wizard->setGoogleEarth(ui->googleEarthCheckBox->isChecked());
	flag = 1 - 2 * ui->leftLatitudeComboBox->currentIndex();
	m_wizard->setLeftLatitude((ui->leftLatitudeDegreeSpinBox->value()
		                       + ui->leftLatitudeMinuteSpinBox->value() / 60.
		                       + ui->leftLatitudeSecondSpinBox->value() / 3600.) * flag);
	flag = 1 - 2 * ui->leftLongitudeComboBox->currentIndex();
	m_wizard->setLeftLongitude((ui->leftLongitudeDegreeSpinBox->value()
		                        + ui->leftLongitudeMinuteSpinBox->value() / 60.
		                        + ui->leftLongitudeSecondSpinBox->value() / 3600.) * flag);
	flag = 1 - 2 * ui->rightLatitudeComboBox->currentIndex();
	m_wizard->setRightLatitude((ui->rightLatitudeDegreeSpinBox->value()
		                        + ui->rightLatitudeMinuteSpinBox->value() / 60.
		                        + ui->rightLatitudeSecondSpinBox->value() / 3600.) * flag);
	flag = 1 - 2 * ui->rightLongitudeComboBox->currentIndex();
	m_wizard->setRightLongitude((ui->rightLongitudeDegreeSpinBox->value()
		                         + ui->rightLongitudeMinuteSpinBox->value() / 60.
		                         + ui->rightLongitudeSecondSpinBox->value() / 3600.) * flag);

	int current;
	// set target window index of m_wizard->windowList()
	current = ui->windowComboBox->currentIndex();
	m_wizard->setTargetWindow(ui->windowComboBox->itemData(current).toInt());
	// set target background index of m_wizard->backgroundList()
	current = ui->backgroundComboBox->currentIndex();
	m_wizard->setTargetBackground(ui->backgroundComboBox->itemData(current).toInt());

	// set the kml file name
	m_wizard->setKMLFilename(ui->kmlEdit->text());

	// calculate angle/north/south/east/west
	calculateKMLInformation();

	return true;
}

void ContinuousSnapshotGoogleEarthSettingPage::setupWindowComboBox()
{
	int idx = 0;
	ui->windowComboBox->clear();
	for (auto it = m_wizard->windowList().begin(); it != m_wizard->windowList().end(); ++it, ++idx) {
		Post2dWindow* post = dynamic_cast<Post2dWindow*>((*it)->widget());
		if (post == nullptr) { continue; }
		// add only post2dwindows.
		ui->windowComboBox->addItem((*it)->windowTitle(), QVariant(idx));
		// idx is the index of (*it) in the list m_wizard->windowList().
	}
	int current = ui->windowComboBox->findData(QVariant(m_wizard->targetWindow()), Qt::UserRole);
	if (current < 0) { current = 0; }
	ui->windowComboBox->setCurrentIndex(current);
}

void ContinuousSnapshotGoogleEarthSettingPage::setupBackgroundComboBox()
{
	int idx = 0;
	ui->backgroundComboBox->clear();
	for (auto it = m_wizard->backgroundList().begin(); it != m_wizard->backgroundList().end(); ++it, ++idx) {
		BackgroundImageInfo* bg = *it;
		ui->backgroundComboBox->addItem(bg->caption(), QVariant(idx));
		// idx is the index of (*it) in the list of m_wizard->backgroundList().
	}
	int current = ui->backgroundComboBox->findData(QVariant(m_wizard->targetBackground()), Qt::UserRole);
	if (current < 0) { current = 0; }
	ui->backgroundComboBox->setCurrentIndex(current);
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
	p.setX(worldX);
	p.setY(worldY);
}

void ContinuousSnapshotGoogleEarthSettingPage::worldToImage(QPointF& p)
{
//	double newx, newy;

	// m_wizard->backgroundList().size() should be positive
	auto it = m_wizard->backgroundList().begin() + m_wizard->targetBackground();
	BackgroundImageInfo* bg = *it;

	// image width
	m_imageWidth = bg->imageWidth();

	QVector2D axisX = QVector2D(1, 0);
	iRIC::rotateVector(axisX, bg->angle());
	QVector2D axisY = axisX;
	iRIC::rotateVector90(axisY);

	QVector2D diff(p.x() - bg->translateX(), p.y() - bg->translateY());
	p.setX(QVector2D::dotProduct(diff, axisX) / bg->scale());
	p.setY(QVector2D::dotProduct(diff, axisY) / bg->scale());
}

void ContinuousSnapshotGoogleEarthSettingPage::imageToLatLong(QPointF& p)
{
	QVector2D point0(m_wizard->leftLongitude(), m_wizard->leftLatitude());
	QVector2D point1(m_wizard->rightLongitude(), m_wizard->rightLatitude());
	QVector2D dv = point1 - point0;

	// overlay on the boundary of east/west longitude
	if (point0.x() * point1.x() < 0) {
		if (point1.x() > 0) {
			point0.setX(point0.x() + 360);
		} else {
			point1.setX(point1.x() + 360);
		}
	}
	if (dv.lengthSquared() > (point1 - point0).lengthSquared()) {
		dv = point1 - point0;
	}

	QVector2D axisX = dv / m_imageWidth;
	double angle = point0.y() * M_PI / 180.;
	m_rate = std::cos(angle);
	QVector2D axisY(- axisX.y() / m_rate, axisX.x() * m_rate);

	QVector2D ret = point0 + axisX * p.x() + axisY * p.y();
	p.setX(ret.x());
	p.setY(ret.y());
}

void ContinuousSnapshotGoogleEarthSettingPage::snapshotToLatLong(QPointF& p)
{
	snapshotToWorld(p);
	worldToImage(p);
	imageToLatLong(p);
}

void ContinuousSnapshotGoogleEarthSettingPage::calculateKMLInformation()
{
	QPointF center, centerRight, centerTop;
	center = QPointF(targetSnapshotSize().width() / 2., targetSnapshotSize().height() / 2.);
	centerRight = QPointF(targetSnapshotSize().width(), targetSnapshotSize().height() / 2.);
	snapshotToLatLong(center);
	snapshotToLatLong(centerRight);

	// calculate angle (radian)
	QPointF dv = centerRight - center;
	dv.setX(dv.x() * m_rate);
	double angle = std::atan(dv.y() / dv.x());
	m_wizard->setAngle(angle * 180 / M_PI);

	// calculate north/south/east/west
	QVector2D centerV(targetSnapshotSize().width() / 2., targetSnapshotSize().height() / 2.);
	QVector2D centerRightV(targetSnapshotSize().width(), targetSnapshotSize().height() / 2.);
	QVector2D centerTopV(targetSnapshotSize().width() / 2., 0);

	QVector2D dx = centerRightV - centerV;
	QVector2D dy = centerTopV - centerV;
	// reflection
	dx.setY(- dx.y());
	dy.setY(- dy.y());

	// rotation
	iRIC::rotateVector(dx, - m_wizard->angle());
	iRIC::rotateVector(dy, - m_wizard->angle());

	// reflection
	dx.setY(- dx.y());
	dy.setY(- dy.y());

	QVector2D rightTopV = centerV + dx + dy;
	QVector2D leftBottomV = centerV - dx - dy;

	QPointF rightTop(rightTopV.x(), rightTopV.y());
	QPointF leftBottom(leftBottomV.x(), leftBottomV.y());

	snapshotToLatLong(rightTop);
	snapshotToLatLong(leftBottom);

	m_wizard->setNorth(rightTop.y());
	m_wizard->setSouth(leftBottom.y());
	m_wizard->setEast(rightTop.x());
	m_wizard->setWest(leftBottom.x());
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
