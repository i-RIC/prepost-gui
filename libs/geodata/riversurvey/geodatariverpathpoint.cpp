#include "geodatarivercrosssection.h"
#include "geodatariverpathpoint.h"
#include "geodatarivershapeinterpolator.h"

#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainert.h>
#include <hydraulicdata/riversurveywaterelevation/hydraulicdatariversurveywaterelevation.h>
#include <hydraulicdata/riversurveywaterelevation/hydraulicdatariversurveywaterelevationitem.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>

#include <QList>
#include <QVector>

#include <algorithm>
#include <cmath>
#include <iriclib_riversurvey.h>

const QString GeoDataRiverPathPoint::NAME_REGEXP = "[A-Za-z0-9_\\-\\.]+";

GeoDataRiverPathPoint::GeoDataRiverPathPoint()
{
	initializeInnerValues();
	initializeInterpolators();
}

GeoDataRiverPathPoint::GeoDataRiverPathPoint(double x, double y)
{
	initializeInnerValues();
	initializeInterpolators();
	m_position = QVector2D(x, y);
}

GeoDataRiverPathPoint::GeoDataRiverPathPoint(const QString& name, double x, double y)
{
	initializeInnerValues();
	initializeInterpolators();
	m_name = name;
	m_position = QVector2D(x, y);
}

GeoDataRiverPathPoint::~GeoDataRiverPathPoint()
{}

const QVector2D& GeoDataRiverPathPoint::position() const
{
	return m_position;
}

void GeoDataRiverPathPoint::initializeInnerValues()
{
	m_crosssection.setParent(this);
	m_nextPoint = nullptr;
	m_previousPoint = nullptr;
	m_firstPoint = true;
	m_gridSkip = false;
	m_CtrlSections.reserve(1);
	m_CtrlSections.push_back(1);
	IsSelected = false;
	InhibitInterpolatorUpdate = false;
	m_crosssectionDirectionL = QVector2D(1, 0);
	m_crosssectionDirectionR = QVector2D(1, 0);
	m_areaGrid = vtkSmartPointer<vtkStructuredGrid>::New();
}

void GeoDataRiverPathPoint::initializeInterpolators()
{
	// linear interpolation is applied now
	m_riverCenter = nullptr;
	m_leftBank = nullptr;
	m_rightBank = nullptr;
	m_lXSec = new LinearLXSecInterpolator(this);
	m_rXSec = new LinearRXSecInterpolator(this);
}

void GeoDataRiverPathPoint::addPathPoint(GeoDataRiverPathPoint* p)
{
	// Inform to the next point that the previous point is changed
	if (m_nextPoint != nullptr) {
		m_nextPoint->m_previousPoint = p;
	}
	// Set the pointers of the added point.
	p->m_previousPoint = this;
	p->m_nextPoint = m_nextPoint;
	p->m_firstPoint = false;
	// Put the new point as the next point of me.
	m_nextPoint = p;
}

void GeoDataRiverPathPoint::insertPathPoint(GeoDataRiverPathPoint* p)
{
	// Inform to the previous point that the next point is changed
	if (m_previousPoint != nullptr) {
		m_previousPoint->m_nextPoint = p;
	}
	// Set the pointers of the added point.
	p->m_previousPoint = m_previousPoint;
	p->m_nextPoint = this;
	p->m_firstPoint = m_firstPoint;
	// Put the new point as the previous point of me.
	m_previousPoint = p;
	m_firstPoint = false;
}

void GeoDataRiverPathPoint::remove()
{
	m_previousPoint->m_nextPoint = m_nextPoint;
	if (m_nextPoint == nullptr) {m_previousPoint->setGridSkip(false);}
	if (m_nextPoint != nullptr) {
		m_nextPoint->m_previousPoint = m_previousPoint;
		if (m_previousPoint->firstPoint()) {m_nextPoint->setGridSkip(false);}
	}
}

void GeoDataRiverPathPoint::movePosition(double offset)
{
	// Calculate the new position.
	QVector2D newPosition = crosssectionPosition(offset);
	m_crosssection.moveCenter(offset);
	m_position = newPosition;
}

bool GeoDataRiverPathPoint::firstPoint() const
{
	return m_firstPoint;
}

void GeoDataRiverPathPoint::updateAllXSecInterpolators()
{
	updateXSecInterpolators();
	if (m_nextPoint != nullptr) {
		m_nextPoint->updateAllXSecInterpolators();
	}
}

void GeoDataRiverPathPoint::updateXSecInterpolators()
{
	if (! InhibitInterpolatorUpdate) {
		m_lXSec->updateParameters();
		m_rXSec->updateParameters();
	}
}

void GeoDataRiverPathPoint::updateRiverShapeInterpolators()
{
	if (! InhibitInterpolatorUpdate) {
		m_riverCenter->updateParameters();
		m_leftBank->updateParameters();
		m_rightBank->updateParameters();
	}
}

void GeoDataRiverPathPoint::UpdateGridInterpolators()
{
	if (! InhibitInterpolatorUpdate) {
		// All interpolators are updated.
		for (Interpolator2D1* interpolator : m_LGridLines) {
			if (interpolator != nullptr) {
				interpolator->updateParameters();
			}
		}
		for (Interpolator2D1* interpolator : m_RGridLines) {
			if (interpolator != nullptr) {
				interpolator->updateParameters();
			}
		}
	}
}

unsigned int GeoDataRiverPathPoint::gridCounts(GeoDataRiverPathPoint* end) const
{
	if (this == end) {
		return 1;
	} else {
		if (! m_gridSkip) {
			return 1 + static_cast<unsigned int>(CenterLineCtrlPoints.size()) + m_nextPoint->gridCounts(end);
		} else {
			return m_nextPoint->gridCounts(end);
		}
	}
}

QVector2D GeoDataRiverPathPoint::CtrlPointPosition2D(CtrlPointPosition pos, unsigned int index) /* throw (ErrorCodes)*/
{
	int idx = static_cast<int>(index);
	switch (pos) {
	case pposCenterToLeft:
		if (idx >= CenterToLeftCtrlPoints.size()) {throw ec_OutOfCtrlPointRange;}
		return CtrlPointPosition2D(pos, CenterToLeftCtrlPoints[index]);
		break;
	case pposCenterToRight:
		if (idx >= CenterToRightCtrlPoints.size()) {throw ec_OutOfCtrlPointRange;}
		return CtrlPointPosition2D(pos, CenterToRightCtrlPoints[index]);
		break;
	case pposCenterLine:
		if (idx >= CenterLineCtrlPoints.size()) {throw ec_OutOfCtrlPointRange;}
		return CtrlPointPosition2D(pos, CenterLineCtrlPoints[index]);
		break;
	case pposLeftBank:
		if (idx >= LeftBankCtrlPoints.size()) {throw ec_OutOfCtrlPointRange;}
		return CtrlPointPosition2D(pos, LeftBankCtrlPoints[index]);
		break;
	case pposRightBank:
		if (idx >= RightBankCtrlPoints.size()) {throw ec_OutOfCtrlPointRange;}
		return CtrlPointPosition2D(pos, RightBankCtrlPoints[index]);
		break;
	default:
		// Never used. Just to avoid compiler warning.
		return QVector2D(0, 0);
	}
}

QVector2D GeoDataRiverPathPoint::CtrlPointPosition2D(CtrlPointPosition pos, double d)
{
	if (d < 0 || d > 1) {throw ec_OutOfCtrlPointRange;}
	GeoDataRiverCrosssection::Altitude altitude;
	switch (pos) {
	case pposCenterToLeft:
		altitude = m_lXSec->interpolate(d);
		return crosssectionPosition(altitude.position());
		break;
	case pposCenterToRight:
		altitude = m_rXSec->interpolate(d);
		return crosssectionPosition(altitude.position());
		break;
	case pposCenterLine:
		return myCtrlPointPosition2D(&GeoDataRiverPathPoint::riverCenter, d);
		break;
	case pposLeftBank:
		return myCtrlPointPosition2D(&GeoDataRiverPathPoint::leftBank, d);
		break;
	case pposRightBank:
		return myCtrlPointPosition2D(&GeoDataRiverPathPoint::rightBank, d);
		break;
	default:
		return QVector2D(0, 0);
	}
}

int GeoDataRiverPathPoint::selectedPoints() const
{
	int selectedpoints = 0;
	if (IsSelected) {
		++ selectedpoints;
	}
	if (m_nextPoint != 0) {
		return selectedpoints + m_nextPoint->selectedPoints();
	} else {
		return selectedpoints;
	}
}

void GeoDataRiverPathPoint::clearSelection()
{
	IsSelected = false;
	if (m_nextPoint == nullptr) {return;}

	m_nextPoint->clearSelection();
}

void GeoDataRiverPathPoint::selectRegion(const QVector2D& point0, const QVector2D& v0, const QVector2D& v1)
{
	QVector2D dv = m_position - point0;
	QVector2D tmpv0 = v0;
	tmpv0.normalize();
	QVector2D tmpv1 = v1;
	tmpv1.normalize();
	double d0 = QVector2D::dotProduct(dv, tmpv0);
	double d1 = QVector2D::dotProduct(dv, tmpv1);
	if ((! firstPoint()) &&
			0 < d0 && d0 < v0.length() &&
			0 < d1 && d1 < v1.length()) {
		IsSelected = true;
	} else {
		IsSelected = false;
	}
	if (m_nextPoint != nullptr) {
		m_nextPoint->selectRegion(point0, v0, v1);
	}
}

void GeoDataRiverPathPoint::XORSelectRegion(const QVector2D& point0, const QVector2D& v0, const QVector2D& v1)
{
	QVector2D dv = m_position - point0;
	QVector2D tmpv0 = v0;
	tmpv0.normalize();
	QVector2D tmpv1 = v1;
	tmpv1.normalize();
	double d0 = QVector2D::dotProduct(dv, tmpv0);
	double d1 = QVector2D::dotProduct(dv, tmpv1);
	if (0 < d0 && d0 < v0.length() &&
			0 < d1 && d1 < v1.length()) {
		IsSelected = ! IsSelected;
	}
	if (m_nextPoint != nullptr) {
		m_nextPoint->XORSelectRegion(point0, v0, v1);
	}
}

void GeoDataRiverPathPoint::SelectCtrlPointsRegion(
	const QVector2D& point0, const QVector2D& v0, const QVector2D& v1,
	std::list<CtrlPointSelectionInfo>& info)
{
	if (! m_gridSkip) {
		unsigned int i;
		QVector2D tmpv0 = v0;
		QVector2D tmpv1 = v1;
		tmpv0.normalize();
		tmpv1.normalize();
		double l0 = v0.length();
		double l1 = v1.length();
		// Rough checking first

		// Investigate left bank
		for (i = 0; i < CenterToLeftCtrlPoints.size(); ++i) {
			QVector2D dv = CtrlPointPosition2D(pposCenterToLeft, i) - point0;
			double ip0 = QVector2D::dotProduct(dv, tmpv0);
			double ip1 = QVector2D::dotProduct(dv, tmpv1);
			if (0 < ip0 && ip0 < l0 &&
					0 < ip1 && ip1 < l1) {
				CtrlPointSelectionInfo sinfo;
				sinfo.Point = this;
				sinfo.Position = pposCenterToLeft;
				sinfo.Index = i;
				info.push_back(sinfo);
			}
		}
		// Investigate right bank
		for (i = 0; i < CenterToRightCtrlPoints.size(); ++i) {
			QVector2D dv = CtrlPointPosition2D(pposCenterToRight, i) - point0;
			double ip0 = QVector2D::dotProduct(dv, tmpv0);
			double ip1 = QVector2D::dotProduct(dv, tmpv1);
			if (0 < ip0 && ip0 < l0 &&
					0 < ip1 && ip1 < l1) {
				CtrlPointSelectionInfo sinfo;
				sinfo.Point = this;
				sinfo.Position = pposCenterToRight;
				sinfo.Index = i;
				info.push_back(sinfo);
			}
		}

		if (m_nextPoint != 0) {

			// Check river center line
			for (i = 0; i < CenterLineCtrlPoints.size(); ++i) {
				QVector2D dv = CtrlPointPosition2D(pposCenterLine, i) - point0;
				double ip0 = QVector2D::dotProduct(dv, tmpv0);
				double ip1 = QVector2D::dotProduct(dv, tmpv1);
				if (0 < ip0 && ip0 < l0 &&
						0 < ip1 && ip1 < l1) {
					CtrlPointSelectionInfo sinfo;
					sinfo.Point = this;
					sinfo.Position = pposCenterLine;
					sinfo.Index = i;
					info.push_back(sinfo);
				}
			}
			// Check left bank line
			for (i = 0; i < LeftBankCtrlPoints.size(); ++i) {
				QVector2D dv = CtrlPointPosition2D(pposLeftBank, i) - point0;
				double ip0 = QVector2D::dotProduct(dv, tmpv0);
				double ip1 = QVector2D::dotProduct(dv, tmpv1);
				if (0 < ip0 && ip0 < l0 &&
						0 < ip1 && ip1 < l1) {
					CtrlPointSelectionInfo sinfo;
					sinfo.Point = this;
					sinfo.Position = pposLeftBank;
					sinfo.Index = i;
					info.push_back(sinfo);
				}
			}
			// Check right bank line
			for (i = 0; i < RightBankCtrlPoints.size(); ++i) {
				QVector2D dv = CtrlPointPosition2D(pposRightBank, i) - point0;
				double ip0 = QVector2D::dotProduct(dv, tmpv0);
				double ip1 = QVector2D::dotProduct(dv, tmpv1);
				if (0 < ip0 && ip0 < l0 &&
						0 < ip1 && ip1 < l1) {
					CtrlPointSelectionInfo sinfo;
					sinfo.Point = this;
					sinfo.Position = pposRightBank;
					sinfo.Index = i;
					info.push_back(sinfo);
				}
			}
		}
	}
	if (m_nextPoint != nullptr) {
		m_nextPoint->SelectCtrlPointsRegion(point0, v0, v1, info);
	}
}


QVector2D GeoDataRiverPathPoint::GridCtrlPosition(Bank bank, unsigned int index1, double param)
{
	if (bank == bk_LeftBank) {
		return GeoDataRiverPathPoint::myCtrlPointPosition2D(&GeoDataRiverPathPoint::LGridLine, index1, param);
	} else {
		return GeoDataRiverPathPoint::myCtrlPointPosition2D(&GeoDataRiverPathPoint::RGridLine, index1, param);
	}
}

QVector2D GeoDataRiverPathPoint::backgroundGridCtrlPosition(Bank bank, unsigned int index1, double param)
{
	if (bank == bk_LeftBank) {
		return GeoDataRiverPathPoint::myBgCtrlPointPosition2D(&GeoDataRiverPathPoint::backgroundLGridLine, index1, param);
	} else {
		return GeoDataRiverPathPoint::myBgCtrlPointPosition2D(&GeoDataRiverPathPoint::backgroundRGridLine, index1, param);
	}
}

double GeoDataRiverPathPoint::GridCtrlParameter(Bank bank, int index1, int index2)
{
	double t0, t1, s0, s1;
	t0 = CenterLineCtrlPoints[index2];
	if (bank == bk_LeftBank) {
		s0 = CenterToLeftCtrlPoints[index1];
		s1 = m_nextPoint->CenterToLeftCtrlPoints[index1];
		t1 = LeftBankCtrlPoints[index2];
	} else {
		s0 = CenterToRightCtrlPoints[index1];
		s1 = m_nextPoint->CenterToRightCtrlPoints[index1];
		t1 = RightBankCtrlPoints[index2];
	}
	return ((t1 - t0) * s0 + t0) / (1 - (t1 - t0) * (s1 - s0));
}

QList<QVector2D> GeoDataRiverPathPoint::CtrlZonePoints(CtrlZonePosition position, unsigned int index, int num)
{
	QList<QVector2D> result;
	QVector<double> tmpdbls;
	double t0;
	double t1;
	double t;
	double dt;
	switch (position) {
	case zposCenterToLeft:
		tmpdbls.reserve(3);
		if (index == 0) {
			tmpdbls.push_back(0);
			if (CenterToLeftCtrlPoints.size() == 0) {
				if (m_crosssection.fixedPointLSet()) {
					tmpdbls.push_back(m_crosssection.fixedPointLDiv());
				}
				tmpdbls.push_back(1);
			} else {
				if (m_crosssection.fixedPointLSet() && m_crosssection.fixedPointLDiv() < CenterToLeftCtrlPoints[0]) {
					tmpdbls.push_back(m_crosssection.fixedPointLDiv());
				}
				tmpdbls.push_back(CenterToLeftCtrlPoints[0]);
			}
		} else if (index < CenterToLeftCtrlPoints.size()) {
			tmpdbls.push_back(CenterToLeftCtrlPoints[index - 1]);
			if (m_crosssection.fixedPointLSet() &&
					(
						m_crosssection.fixedPointLDiv() > CenterToLeftCtrlPoints[index - 1] &&
						m_crosssection.fixedPointLDiv() < CenterToLeftCtrlPoints[index]
					)
				 ) {
				tmpdbls.push_back(m_crosssection.fixedPointLDiv());
			}
			tmpdbls.push_back(CenterToLeftCtrlPoints[index]);
		} else if (index == CenterToLeftCtrlPoints.size()) {
			tmpdbls.push_back(CenterToLeftCtrlPoints[index - 1]);
			if (m_crosssection.fixedPointLSet() && m_crosssection.fixedPointLDiv() > CenterToLeftCtrlPoints[index - 1]) {
				tmpdbls.push_back(m_crosssection.fixedPointLDiv());
			}
			tmpdbls.push_back(1);
		} else {
			throw(ec_OutOfCtrlZoneRange);
		}
		for (auto dblit = tmpdbls.begin(); dblit != tmpdbls.end(); ++dblit) {
			result.push_back(CtrlPointPosition2D(pposCenterToLeft, *dblit));
		}
		break;
	case zposCenterToRight:
		// It is always a straight line. Returns only start point and end point
		tmpdbls.reserve(3);
		if (index == 0) {
			tmpdbls.push_back(0);
			if (CenterToRightCtrlPoints.size() == 0) {
				if (m_crosssection.fixedPointRSet()) {
					tmpdbls.push_back(m_crosssection.fixedPointRDiv());
				}
				tmpdbls.push_back(1);
			} else {
				if (m_crosssection.fixedPointRSet() && m_crosssection.fixedPointRDiv() < CenterToRightCtrlPoints[0]) {
					tmpdbls.push_back(m_crosssection.fixedPointRDiv());
				}
				tmpdbls.push_back(CenterToRightCtrlPoints[0]);
			}
		} else if (index < CenterToRightCtrlPoints.size()) {
			tmpdbls.push_back(CenterToRightCtrlPoints[index - 1]);
			if (m_crosssection.fixedPointRSet() &&
					(
						m_crosssection.fixedPointRDiv() > CenterToRightCtrlPoints[index - 1] &&
						m_crosssection.fixedPointRDiv() < CenterToRightCtrlPoints[index]
					)
				 ) {
				tmpdbls.push_back(m_crosssection.fixedPointRDiv());
			}
			tmpdbls.push_back(CenterToRightCtrlPoints[index]);
		} else if (index == CenterToRightCtrlPoints.size()) {
			tmpdbls.push_back(CenterToRightCtrlPoints[index - 1]);
			if (m_crosssection.fixedPointRSet() && m_crosssection.fixedPointRDiv() > CenterToRightCtrlPoints[index - 1]) {
				tmpdbls.push_back(m_crosssection.fixedPointRDiv());
			}
			tmpdbls.push_back(1);
		} else {
			throw(ec_OutOfCtrlZoneRange);
		}
		for (auto dblit = tmpdbls.begin(); dblit != tmpdbls.end(); ++dblit) {
			result.push_back(CtrlPointPosition2D(pposCenterToRight, *dblit));
		}
		break;
	case zposCenterLine:
		if (index == 0) {
			t0 = 0;
			if (CenterLineCtrlPoints.size() == 0) {
				t1 = 1;
			} else {
				t1 = CenterLineCtrlPoints[0];
			}
		} else if (index < CenterLineCtrlPoints.size()) {
			t0 = CenterLineCtrlPoints[index - 1];
			t1 = CenterLineCtrlPoints[index];
		} else if (index == CenterLineCtrlPoints.size()) {
			t0 = CenterLineCtrlPoints[index - 1];
			t1 = 1;
		} else {
			throw(ec_OutOfCtrlZoneRange);
		}
		t = t0;
		dt = (t1 - t0) / static_cast<double>(num);
		if (dt < 0.0001) {dt = 0.0001;}
		while (t < t1 - 0.0001) {
			result.push_back(CtrlPointPosition2D(pposCenterLine, t));
			t += dt;
		}
		result.push_back(CtrlPointPosition2D(pposCenterLine, t1));
		break;
	case zposLeftBank:
		if (index == 0) {
			t0 = 0;
			if (LeftBankCtrlPoints.size() > 0) {
				t1 = LeftBankCtrlPoints[0];
			} else {
				t1 = 1;
			}
		} else if (index < LeftBankCtrlPoints.size()) {
			t0 = LeftBankCtrlPoints[index - 1];
			t1 = LeftBankCtrlPoints[index];
		} else if (index == LeftBankCtrlPoints.size()) {
			t0 = LeftBankCtrlPoints[index - 1];
			t1 = 1;
		} else {
			throw(ec_OutOfCtrlZoneRange);
		}
		t = t0;
		dt = (t1 - t0) / static_cast<double>(num);
		while (t < t1 - 0.0001) {
			result.push_back(CtrlPointPosition2D(pposLeftBank, t));
			t += dt;
		}
		result.push_back(CtrlPointPosition2D(pposLeftBank, t1));
		break;
	case zposRightBank:
		if (index == 0) {
			t0 = 0;
			if (RightBankCtrlPoints.size() > 0) {
				t1 = RightBankCtrlPoints[0];
			} else {
				t1 = 1;
			}
		} else if (index < RightBankCtrlPoints.size()) {
			t0 = RightBankCtrlPoints[index - 1];
			t1 = RightBankCtrlPoints[index];
		} else if (index == RightBankCtrlPoints.size()) {
			t0 = RightBankCtrlPoints[index - 1];
			t1 = 1;
		} else {
			throw(ec_OutOfCtrlZoneRange);
		}
		t = t0;
		dt = (t1 - t0) / static_cast<double>(num);
		while (t < t1 - 0.0001) {
			result.push_back(CtrlPointPosition2D(pposRightBank, t));
			t += dt;
		}
		result.push_back(CtrlPointPosition2D(pposRightBank, t1));
		break;
	}
	return result;
}


bool GeoDataRiverPathPoint::gridSkip() const
{
	return m_gridSkip;
}

void GeoDataRiverPathPoint::setGridSkip(bool skip)
{
	if (m_nextPoint == 0) {return;}
	if (firstPoint()) {return;}
	if (m_previousPoint->firstPoint()) {return;}

	m_gridSkip = skip;
}



Interpolator2D1* GeoDataRiverPathPoint::riverCenter() const
{
	return m_riverCenter;
}

void GeoDataRiverPathPoint::setRiverCenter(Interpolator2D1* interpolator)
{
	m_riverCenter = interpolator;
}

Interpolator2D1* GeoDataRiverPathPoint::leftBank() const
{
	return m_leftBank;
}

void GeoDataRiverPathPoint::setLeftBank(Interpolator2D1* interpolator)
{
	m_leftBank = interpolator;
}

Interpolator2D1* GeoDataRiverPathPoint::rightBank() const
{
	return m_rightBank;
}

void GeoDataRiverPathPoint::setRightBank(Interpolator2D1* interpolator)
{
	m_rightBank = interpolator;
}

LinearLXSecInterpolator* GeoDataRiverPathPoint::lXSec() const
{
	return m_lXSec;
}

LinearRXSecInterpolator* GeoDataRiverPathPoint::rXSec() const
{
	return m_rXSec;
}

std::vector<Interpolator2D1*>& GeoDataRiverPathPoint::LGridLines()
{
	return m_LGridLines;
}

const std::vector<Interpolator2D1 *> &GeoDataRiverPathPoint::LGridLines() const
{
	return m_LGridLines;
}

std::vector<Interpolator2D1*>& GeoDataRiverPathPoint::RGridLines()
{
	return m_RGridLines;
}

const std::vector<Interpolator2D1*>& GeoDataRiverPathPoint::RGridLines() const
{
	return m_RGridLines;
}

std::vector<Interpolator2D1*>& GeoDataRiverPathPoint::backgroundLGridLines()
{
	return m_backgroundLGridLines;
}

const std::vector<Interpolator2D1*>& GeoDataRiverPathPoint::backgroundLGridLines() const
{
	return m_backgroundLGridLines;
}

std::vector<Interpolator2D1*>& GeoDataRiverPathPoint::backgroundRGridLines()
{
	return m_backgroundRGridLines;
}

const std::vector<Interpolator2D1*>& GeoDataRiverPathPoint::backgroundRGridLines() const
{
	return m_backgroundRGridLines;
}

Interpolator2D1* GeoDataRiverPathPoint::LGridLine(unsigned int index) const
{
	return m_LGridLines[index];
}

Interpolator2D1* GeoDataRiverPathPoint::RGridLine(unsigned int index) const
{
	return m_RGridLines[index];
}

Interpolator2D1* GeoDataRiverPathPoint::backgroundLGridLine(unsigned int index) const
{
	return m_backgroundLGridLines[index];
}

Interpolator2D1* GeoDataRiverPathPoint::backgroundRGridLine(unsigned int index) const
{
	return m_backgroundRGridLines[index];
}

void GeoDataRiverPathPoint::setCrosssectionAngle(double angle) /* throw (ErrorCodes)*/
{
	if (m_previousPoint == nullptr) {
		throw ec_PreviousPointDontExist;
	}
	QVector2D dv = m_previousPoint->position() - m_position;
	dv.normalize();

	iRIC::rotateVector(dv, angle);
	setCrosssectionDirection(dv);
}

void GeoDataRiverPathPoint::setCrosssectionDirection(const QVector2D& v)
{
	double rotAngle = iRIC::angleRadian(m_crosssectionDirection, v);
	m_crosssectionDirection = v;
	iRIC::rotateVectorRadian(m_crosssectionDirectionL, rotAngle);
	iRIC::rotateVectorRadian(m_crosssectionDirectionR, rotAngle);
	updateRiverShapeInterpolators();
}

GeoDataRiverPathPoint* GeoDataRiverPathPoint::previousPoint() const
{
	return m_previousPoint;
}

GeoDataRiverPathPoint* GeoDataRiverPathPoint::nextPoint() const
{
	return m_nextPoint;
}

const QString& GeoDataRiverPathPoint::name() const
{
	return m_name;
}

void GeoDataRiverPathPoint::setCrosssectionDirectionL(const QVector2D& direction)
{
	m_crosssectionDirectionL = direction;
}

void GeoDataRiverPathPoint::setCrosssectionDirectionR(const QVector2D& direction)
{
	m_crosssectionDirectionR = direction;
}

void GeoDataRiverPathPoint::addExtentionPointLeft(const QVector2D& pos)
{
	GeoDataRiverCrosssection::Altitude& alt = m_crosssection.leftBank(true);
	QVector2D fixPoint = crosssectionPosition(alt.position());
	QVector2D newdirection = pos - fixPoint;
	double distance = newdirection.length();
	newdirection.normalize();
	setCrosssectionDirectionL(- newdirection);
	if (m_crosssection.fixedPointRSet()) {
		m_crosssection.setFixedPointR(m_crosssection.fixedPointRIndex() + 1);
	}
	m_crosssection.addPoint(alt.position() - distance, alt.height());
	m_crosssection.setFixedPointL(1);
}

void GeoDataRiverPathPoint::addExtentionPointRight(const QVector2D& pos)
{
	GeoDataRiverCrosssection::Altitude& alt = m_crosssection.rightBank(true);
	QVector2D fixPoint = crosssectionPosition(alt.position());
	QVector2D newdirection = pos - fixPoint;
	double distance = newdirection.length();
	newdirection.normalize();
	setCrosssectionDirectionR(newdirection);
	int rightBankIndex = m_crosssection.rightBankIndex(true);
	m_crosssection.addPoint(alt.position() + distance, alt.height());
	m_crosssection.setFixedPointR(rightBankIndex);
}

void GeoDataRiverPathPoint::moveExtentionPointLeft(const QVector2D& pos)
{
	crosssection().unsetFixedPointL();
	addExtentionPointLeft(pos);
}

void GeoDataRiverPathPoint::moveExtentionPointRight(const QVector2D& pos)
{
	crosssection().unsetFixedPointR();
	addExtentionPointRight(pos);
}

void GeoDataRiverPathPoint::removeExtentionPointLeft()
{
	crosssection().unsetFixedPointL();
}

void GeoDataRiverPathPoint::removeExtentionPointRight()
{
	crosssection().unsetFixedPointR();
}

void GeoDataRiverPathPoint::shiftCenter(double shiftWidth)
{
	double width = shiftWidth;
	double right = crosssection().rightBank().position();
	double left = crosssection().leftBank().position();
	if (left > shiftWidth) {
		width = left + 1.0e-6;
	}
	if (right < shiftWidth) {
		width = right - 1.0e-6;
	}
	setPosition(m_position + m_crosssectionDirection * width);
	m_crosssection.moveCenter(width);
}

void GeoDataRiverPathPoint::setPosition(const QVector2D& v)
{
	m_position = v;
}

void GeoDataRiverPathPoint::UpdateCtrlSections()
{
	if (m_gridSkip) {
		m_CtrlSections.clear();
		m_CtrlSections.insert(0, 1, 1);
	} else {
		std::vector<double> lengths;
		if (m_nextPoint != nullptr) {
			lengths.push_back((m_nextPoint->position() - position()).length());
		}
		GeoDataRiverPathPoint* tmpp = this;
		while (tmpp->nextPoint() && tmpp->nextPoint()->gridSkip()) {
			tmpp = tmpp->nextPoint();
			lengths.push_back((tmpp->nextPoint()->position() - tmpp->position()).length());
		}
		double lengthsum = 0;
		for (auto it = lengths.begin(); it != lengths.end(); ++it) {
			lengthsum += *it;
		}
		m_CtrlSections.clear();
		m_CtrlSections.reserve(lengths.size());
		double subsum = 0;
		for (auto it = lengths.begin(); it != lengths.end(); ++it) {
			subsum += *it;
			m_CtrlSections.push_back(subsum / lengthsum);
		}
	}
	if (m_nextPoint != nullptr) {
		m_nextPoint->UpdateCtrlSections();
	}
}

vtkStructuredGrid* GeoDataRiverPathPoint::areaGrid() const
{
	return m_areaGrid;
}

QVector2D GeoDataRiverPathPoint::myCtrlPointPosition2D(Interpolator2D1* (GeoDataRiverPathPoint::* f )() const, double d)
{
	int i = 0;
	double d0 = 0;
	double d1 = m_CtrlSections[i];
	GeoDataRiverPathPoint* tmpp = this;
	while (d > d1) {
		++i;
		d0 = d1;
		d1 = m_CtrlSections[i];
		tmpp = tmpp->nextPoint();
	}
	double subd = (d - d0) / (d1 - d0);
	Interpolator2D1* interpolator = (tmpp->*f)();
	return interpolator->interpolate(subd);
}

QVector2D GeoDataRiverPathPoint::myCtrlPointPosition2D(Interpolator2D1* (GeoDataRiverPathPoint::* f)(unsigned int) const, unsigned int index, double d)
{
	int i = 0;
	double d0 = 0;
	double d1 = m_CtrlSections[i];
	GeoDataRiverPathPoint* tmpp = this;
	while (d > d1) {
		++i;
		d0 = d1;
		d1 = m_CtrlSections[i];
		tmpp = tmpp->nextPoint();
	}
	double subd = (d - d0) / (d1 - d0);
	Interpolator2D1* interpolator = (tmpp->*f)(index);
	return interpolator->interpolate(subd);
}

QVector2D GeoDataRiverPathPoint::myBgCtrlPointPosition2D(Interpolator2D1* (GeoDataRiverPathPoint::* f)(unsigned int) const, unsigned int index, double d)
{
	Interpolator2D1* interpolator = (this->*f)(index);
	return interpolator->interpolate(d);
}

QVector2D GeoDataRiverPathPoint::crosssectionPosition(double x)
{
	QVector2D dir = m_crosssectionDirection;
	QVector2D dirl;
	dirl = m_crosssectionDirectionL;
	QVector2D dirr;
	dirr = m_crosssectionDirectionR;
	QVector2D tmpv;
	if (
		m_crosssection.fixedPointLSet() &&
		x < m_crosssection.fixedPointL().position()
	) {
		double leftfix = m_crosssection.fixedPointL().position();
		tmpv = m_position + dir * leftfix + dirl * (x - leftfix);
	} else if (
		m_crosssection.fixedPointRSet() &&
		x > m_crosssection.fixedPointR().position()
	) {
		double rightfix = m_crosssection.fixedPointR().position();
		tmpv = m_position + dir * rightfix + dirr * (x - rightfix);
	} else {
		tmpv = m_position + m_crosssectionDirection * x;
	}
	return tmpv;
}

void GeoDataRiverPathPoint::setName(const QString& newname)
{
	m_name = newname;
}

GeoDataRiverCrosssection& GeoDataRiverPathPoint::crosssection()
{
	return m_crosssection;
}

const GeoDataRiverCrosssection& GeoDataRiverPathPoint::crosssection() const
{
	return m_crosssection;
}

const QVector2D& GeoDataRiverPathPoint::crosssectionDirection() const
{
	return m_crosssectionDirection;
}

const QVector2D& GeoDataRiverPathPoint::crosssectionDirectionL() const
{
	return m_crosssectionDirectionL;
}

const QVector2D& GeoDataRiverPathPoint::crosssectionDirectionR() const
{
	return m_crosssectionDirectionR;
}

void GeoDataRiverPathPoint::createGrid(Structured2DGrid* grid, unsigned int initcount, bool elevmapping, bool last)
{
	GridAttributeContainerT<double>* elev = dynamic_cast<GridAttributeContainerT<double>*>(grid->gridAttribute("Elevation"));
	QVector2D vec2d;
	int index;
	if (last) {
		// export grid points only crosssections.
		// check grid size.
		bool error = false;
		if ((1 + initcount) > grid->dimensionI()) {
			error = true;
		}
		if (error) {throw ec_InvalidGridSize;}

		unsigned int leftctrlpoints = static_cast<unsigned int>(CenterToLeftCtrlPoints.size());
		unsigned int rightctrlpoints = static_cast<unsigned int>(CenterToRightCtrlPoints.size());
		// river center
		vec2d = m_position;
		index = grid->vertexIndex(initcount, rightctrlpoints + 1);
		grid->setVertex(index, vec2d);
		if (elev != nullptr && elevmapping) {
			elev->setValue(index, m_lXSec->interpolate(0).height());
		}

		// left bank
		vec2d = crosssectionPosition(m_lXSec->interpolate(1).position());
		index = grid->vertexIndex(initcount, leftctrlpoints + rightctrlpoints + 2);
		grid->setVertex(index, vec2d);
		if (elev != nullptr && elevmapping) {
			elev->setValue(index, m_lXSec->interpolate(1).height());
		}
		// right bank.
		vec2d = crosssectionPosition(m_rXSec->interpolate(1).position());
		index = grid->vertexIndex(initcount, 0);
		grid->setVertex(index, vec2d);
		if (elev != nullptr && elevmapping) {
			elev->setValue(index, m_rXSec->interpolate(1).height());
		}
		// points between river center and left bank
		for (unsigned int i = 0; i < leftctrlpoints; ++i) {
			vec2d = crosssectionPosition(m_lXSec->interpolate(CenterToLeftCtrlPoints[i]).position());
			index = grid->vertexIndex(initcount, rightctrlpoints + 2 + i);
			grid->setVertex(index, vec2d);
			if (elev != nullptr && elevmapping) {
				elev->setValue(index, m_lXSec->interpolate(CenterToLeftCtrlPoints[i]).height());
			}
		}
		// Points between river center and right bank
		for (unsigned int i = 0; i < rightctrlpoints; ++i) {
			// Check the points on the crosssection
			vec2d = crosssectionPosition(m_rXSec->interpolate(CenterToRightCtrlPoints[i]).position());
			index = grid->vertexIndex(initcount, rightctrlpoints - i);
			grid->setVertex(index, vec2d);
			if (elev != nullptr && elevmapping) {
				elev->setValue(index, m_rXSec->interpolate(CenterToRightCtrlPoints[i]).height());
			}
		}
		return;
	}
	// check grid size.
	bool error = false;
	if ((CenterLineCtrlPoints.size() + 1 + initcount) > grid->dimensionI()) {
		error = true;
	}
	if (error) {throw ec_InvalidGridSize;}
	LinearInterpolator1D1 inter1;
	double valuethis, valuenext;
	unsigned int leftctrlpoints = static_cast<unsigned int>(CenterToLeftCtrlPoints.size());
	unsigned int rightctrlpoints = static_cast<unsigned int>(CenterToRightCtrlPoints.size());
	// inter1 is used for height interpolation
	// river center
	valuethis = m_lXSec->interpolate(0).height();
	valuenext = m_nextPoint->m_lXSec->interpolate(0).height();
	inter1.setValues(valuethis, valuenext);

	// on cross section
	vec2d = m_position;
	index = grid->vertexIndex(initcount, rightctrlpoints + 1);
	grid->setVertex(index, vec2d);
	if (elev != nullptr && elevmapping) {
		elev->setValue(index, myHeight(bk_LeftBank, 0, 0, 0));
	}

	// points between river center and the river center of the next point.
	for (unsigned int i = 0; i < CenterLineCtrlPoints.size(); ++i) {
		vec2d = myCtrlPointPosition2D(&GeoDataRiverPathPoint::riverCenter, CenterLineCtrlPoints[i]);
		index = grid->vertexIndex(initcount + i + 1, rightctrlpoints + 1);
		grid->setVertex(index, vec2d);
		if (elev != nullptr && elevmapping) {
			elev->setValue(index, myHeight(bk_LeftBank, 0, 0, CenterLineCtrlPoints[i]));
		}
	}

	// left bank

	// on cross section
	vec2d = m_leftBank->interpolate(0);
	index = grid->vertexIndex(initcount, rightctrlpoints + leftctrlpoints + 2);
	grid->setVertex(index, vec2d);
	if (elev != nullptr && elevmapping) {
		elev->setValue(index, myHeight(bk_LeftBank, 1, 1, 0));
	}
	// points between left bank and the left bank of the next point.
	for (unsigned int i = 0; i < LeftBankCtrlPoints.size(); ++i) {
		vec2d = myCtrlPointPosition2D(&GeoDataRiverPathPoint::leftBank, LeftBankCtrlPoints[i]);
		index = grid->vertexIndex(initcount + i + 1, rightctrlpoints + leftctrlpoints + 2);
		grid->setVertex(index, vec2d);
		if (elev != nullptr && elevmapping) {
			elev->setValue(index, myHeight(bk_LeftBank, 1, 1, LeftBankCtrlPoints[i]));
		}
	}

	// right bank

	// on cross section
	vec2d = m_rightBank->interpolate(0);
	index = grid->vertexIndex(initcount, 0);
	grid->setVertex(index, vec2d);
	if (elev != nullptr && elevmapping) {
		elev->setValue(index, myHeight(bk_RightBank, 1, 1, 0));
	}
	// points between right bank and the right bank of the next point.
	for (unsigned int i = 0; i < RightBankCtrlPoints.size(); ++i) {
		vec2d = myCtrlPointPosition2D(&GeoDataRiverPathPoint::rightBank, RightBankCtrlPoints[i]);
		index = grid->vertexIndex(initcount + i + 1, 0);
		grid->setVertex(index, vec2d);
		if (elev != nullptr && elevmapping) {
			elev->setValue(index, myHeight(bk_RightBank, 1, 1, RightBankCtrlPoints[i]));
		}
	}

	// points between rever center and left bank
	for (unsigned int i = 0; i < leftctrlpoints; ++i) {
		// on cross section
		vec2d = crosssectionPosition(m_lXSec->interpolate(CenterToLeftCtrlPoints[i]).position());
		index = grid->vertexIndex(initcount, rightctrlpoints + 2 + i);
		grid->setVertex(index, vec2d);
		if (elev != nullptr && elevmapping) {
			elev->setValue(index, myHeight(GeoDataRiverPathPoint::bk_LeftBank, CenterToLeftCtrlPoints[i], m_nextPoint->CenterToLeftCtrlPoints[i], 0));
		}
		// grid points
		for (unsigned int j = 0; j < CenterLineCtrlPoints.size(); ++j) {
			double param = GridCtrlParameter(bk_LeftBank, i, j);
			vec2d = GridCtrlPosition(bk_LeftBank, i, param);
			index = grid->vertexIndex(initcount + 1 + j, rightctrlpoints + 2 + i);
			grid->setVertex(index, vec2d);
			if (elev != nullptr && elevmapping) {
				elev->setValue(index, myHeight(GeoDataRiverPathPoint::bk_LeftBank, CenterToLeftCtrlPoints[i], m_nextPoint->CenterToLeftCtrlPoints[i], param));
			}
		}
	}
	// points between rever center and right bank
	for (unsigned int i = 0; i < rightctrlpoints; ++i) {
		// on cross section
		vec2d = crosssectionPosition(m_rXSec->interpolate(CenterToRightCtrlPoints[i]).position());
		index = grid->vertexIndex(initcount, rightctrlpoints - i);
		grid->setVertex(index, vec2d);
		if (elev != nullptr && elevmapping) {
			elev->setValue(index, myHeight(GeoDataRiverPathPoint::bk_RightBank, CenterToRightCtrlPoints[i], m_nextPoint->CenterToRightCtrlPoints[i], 0));
		}
		// grid points
		for (unsigned int j = 0; j < CenterLineCtrlPoints.size(); ++j) {
			double param = GridCtrlParameter(bk_RightBank, i, j);
			vec2d = GridCtrlPosition(bk_RightBank, i, param);
			index = grid->vertexIndex(initcount + 1 + j, rightctrlpoints - i);
			grid->setVertex(index, vec2d);
			if (elev != nullptr && elevmapping) {
				elev->setValue(index, myHeight(GeoDataRiverPathPoint::bk_RightBank, CenterToRightCtrlPoints[i], m_nextPoint->CenterToRightCtrlPoints[i], param));
			}
		}
	}
}

double GeoDataRiverPathPoint::myHeight(GeoDataRiverPathPoint::Bank bank, double t0, double t1, double d)
{
	int i = 0;
	double d0 = 0;
	double d1 = m_CtrlSections[i];
	GeoDataRiverPathPoint* tmpp = this;
	LinearInterpolator1D1 inter;
	while (d > d1) {
		++i;
		d0 = d1;
		d1 = m_CtrlSections[i];
		tmpp = tmpp->nextPoint();
	}
	double subd = (d - d0) / (d1 - d0);
	double subt0 = t0 + (t1 - t0) * d0;
	double subt1 = t0 + (t1 - t0) * d1;

	if (bank == GeoDataRiverPathPoint::bk_LeftBank) {
		// Left bank
		double valuethis = tmpp->lXSec()->interpolate(subt0).height();
		double valuenext = tmpp->nextPoint()->lXSec()->interpolate(subt1).height();
		inter.setValues(valuethis, valuenext);
		return inter.interpolate(subd);
	} else {
		// Right bank
		double valuethis = tmpp->rXSec()->interpolate(subt0).height();
		double valuenext = tmpp->nextPoint()->rXSec()->interpolate(subt1).height();
		inter.setValues(valuethis, valuenext);
		return inter.interpolate(subd);
	}
}

void GeoDataRiverPathPoint::load(QDataStream& s, const VersionNumber& number)
{
	// name
	s >> m_name;
	// position()
	s >> m_position;
	// cross section direction
	s >> m_crosssectionDirection;
	// cross section info
	int count;
	s >> count;
	m_crosssection.AltitudeInfo().clear();
	for (int i = 0; i < count; ++i) {
		GeoDataRiverCrosssection::Altitude alt;
		double pos, height;
		bool active;
		s >> pos >> height >> active;
		alt.setPosition(pos);
		alt.setHeight(height);
		alt.setActive(active);
		m_crosssection.AltitudeInfo().push_back(alt);
	}
	bool tmpbool;
	QVector2D dir;
	int index;
	// left fixed point
	s >> tmpbool;
	if (tmpbool) {
		s >> dir >> index;
		setCrosssectionDirectionL(dir);
		m_crosssection.setFixedPointL(index);
	} else {
		m_crosssection.unsetFixedPointL();
	}
	// right fixed point
	s >> tmpbool;
	if (tmpbool) {
		s >> dir >> index;
		setCrosssectionDirectionR(dir);
		m_crosssection.setFixedPointR(index);
	} else {
		m_crosssection.unsetFixedPointR();
	}
	s >> m_gridSkip;
	// division points
	s >> CenterToLeftCtrlPoints;
	s >> CenterToRightCtrlPoints;
	s >> CenterLineCtrlPoints;
	s >> LeftBankCtrlPoints;
	s >> RightBankCtrlPoints;
	if (number.major() >= 2 && number.build() > 1920) {
		// left shift
		double shift;
		s >> shift;
		m_crosssection.setLeftShift(shift);
	}
	if (number.build() >= 2810) {
		// water surface elevation
		int wseSpecified;
		double wseValue;
		s >> wseSpecified;
		s >> wseValue;
	}
}

void GeoDataRiverPathPoint::save(QDataStream& s) const
{
	// name
	s << m_name;
	// position
	s << m_position;
	// cross section direction
	s << m_crosssectionDirection;
	// cross section info
	// count
	s << m_crosssection.AltitudeInfo().size();
	for (auto it = m_crosssection.AltitudeInfo().begin(); it != m_crosssection.AltitudeInfo().end(); ++it) {
		GeoDataRiverCrosssection::Altitude alt = (*it);
		s << alt.position() << alt.height() << alt.active();
	}
	// fixed points
	s << m_crosssection.fixedPointLSet();
	if (m_crosssection.fixedPointLSet()) {
		s << m_crosssectionDirectionL;
		s << m_crosssection.fixedPointLIndex();
	}
	s << m_crosssection.fixedPointRSet();
	if (m_crosssection.fixedPointRSet()) {
		s << m_crosssectionDirectionR;
		s << m_crosssection.fixedPointRIndex();
	}

	s << m_gridSkip;

	// division points
	s << CenterToLeftCtrlPoints;
	s << CenterToRightCtrlPoints;
	s << CenterLineCtrlPoints;
	s << LeftBankCtrlPoints;
	s << RightBankCtrlPoints;
	// left shift
	s << m_crosssection.leftShift();
	int wseSpecified = 0;
	double wseValue = 0;
	// water surface elevation
	s << wseSpecified;
	s << wseValue;
}

QVector<double>& GeoDataRiverPathPoint::CtrlPoints(CtrlZonePosition position)
{
	switch (position) {
	case zposCenterToLeft:
		return CenterToLeftCtrlPoints;
	case zposCenterToRight:
		return CenterToRightCtrlPoints;
	case zposCenterLine:
		return CenterLineCtrlPoints;
	case zposLeftBank:
		return LeftBankCtrlPoints;
	case zposRightBank:
		return RightBankCtrlPoints;
	default:
		return RightBankCtrlPoints;
	}
}

QVector<double>& GeoDataRiverPathPoint::CtrlPoints(CtrlPointPosition position)
{
	switch (position) {
	case pposCenterToLeft:
		return CenterToLeftCtrlPoints;
	case pposCenterToRight:
		return CenterToRightCtrlPoints;
	case pposCenterLine:
		return CenterLineCtrlPoints;
	case pposLeftBank:
		return LeftBankCtrlPoints;
	case pposRightBank:
		return RightBankCtrlPoints;
		// Never used. this is written to avoid warning in compiling.
	default:
		return RightBankCtrlPoints;
	}
}

void GeoDataRiverPathPoint::centerToBanksRegion(QVector2D& mins, QVector2D& maxs)
{
	QVector2D leftbank = crosssectionPosition(m_crosssection.leftBank(true).position());
	QVector2D rightbank = crosssectionPosition(m_crosssection.rightBank(true).position());
	double xmin, xmax, ymin, ymax;
	// from left bank, right bank.
	if (leftbank.x() < rightbank.x()) {
		xmin = leftbank.x();
		xmax = rightbank.x();
	} else {
		xmin = rightbank.x();
		xmax = leftbank.x();
	}
	if (leftbank.y() < rightbank.y()) {
		ymin = leftbank.y();
		ymax = rightbank.y();
	} else {
		ymin = rightbank.y();
		ymax = leftbank.y();
	}
	// if fixed points set, take them into account.
	if (m_crosssection.fixedPointLSet()) {
		QVector2D tmpv = crosssectionPosition(m_crosssection.fixedPointL().position());
		if (tmpv.x() < xmin) {xmin = tmpv.x();}
		if (tmpv.x() > xmax) {xmax = tmpv.x();}
		if (tmpv.y() < ymin) {ymin = tmpv.y();}
		if (tmpv.y() > ymax) {ymax = tmpv.y();}
	}
	if (m_crosssection.fixedPointRSet()) {
		QVector2D tmpv = crosssectionPosition(m_crosssection.fixedPointR().position());
		if (tmpv.x() < xmin) {xmin = tmpv.x();}
		if (tmpv.x() > xmax) {xmax = tmpv.x();}
		if (tmpv.y() < ymin) {ymin = tmpv.y();}
		if (tmpv.y() > ymax) {ymax = tmpv.y();}
	}
	mins = QVector2D(xmin, ymin);
	maxs = QVector2D(xmax, ymax);
}

void GeoDataRiverPathPoint::thisToNextRegion(QVector2D& mins, QVector2D& maxs)
{
	double xmin, xmax, ymin, ymax;
	QVector2D leftbank  = crosssectionPosition(m_crosssection.leftBank(true).position());
	QVector2D rightbank = crosssectionPosition(m_crosssection.rightBank(true).position());
	QVector2D nextleftbank  = m_nextPoint->crosssectionPosition(m_nextPoint->m_crosssection.leftBank(true).position());
	QVector2D nextrightbank = m_nextPoint->crosssectionPosition(m_nextPoint->m_crosssection.rightBank(true).position());
	xmin = leftbank.x();
	if (rightbank.x() < xmin) {xmin = rightbank.x();}
	if (nextleftbank.x() < xmin) {xmin = nextleftbank.x();}
	if (nextrightbank.x() < xmin) {xmin = nextrightbank.x();}
	xmax = leftbank.x();
	if (rightbank.x() > xmax) {xmax = rightbank.x();}
	if (nextleftbank.x() > xmax) {xmax = nextleftbank.x();}
	if (nextrightbank.x() > xmax) {xmax = nextrightbank.x();}
	ymin = leftbank.y();
	if (rightbank.y() < ymin) {ymin = rightbank.y();}
	if (nextleftbank.y() < ymin) {ymin = nextleftbank.y();}
	if (nextrightbank.y() < ymin) {ymin = nextrightbank.y();}
	ymax = leftbank.y();
	if (rightbank.y() > ymax) {ymax = rightbank.y();}
	if (nextleftbank.y() > ymax) {ymax = nextleftbank.y();}
	if (nextrightbank.y() > ymax) {ymax = nextrightbank.y();}
	mins = QVector2D(xmin, ymin);
	maxs = QVector2D(xmax, ymax);
}

void addCtrlPointsToVector(QVector<double>& values, unsigned int index, GeoDataRiverPathPoint::CtrlPointsAddMethod method)
{
	double t0, t1;
	QVector<double> tmpvalues;
	if (index == 0) {
		t0 = 0;
		if (values.size() == 0) {
			t1 = 1;
		} else {
			t1 = values[0];
		}
	} else if (index < values.size()) {
		t0 = values[index - 1];
		t1 = values[index];
	} else if (index == values.size()) {
		t0 = values[index - 1];
		t1 = 1;
	} else {
		throw GeoDataRiverPathPoint::ec_OutOfCtrlZoneRange;
	}
	tmpvalues.reserve(values.size() + method.number);
	unsigned int i;
	for (i = 0; i < index; ++i) {
		tmpvalues.push_back(values[i]);
	}
	if (method.method == GeoDataRiverPathPoint::CtrlPointsAddMethod::am_Uniform || method.param == 1) {
		double dt = (t1 - t0) / static_cast<double>(method.number + 1);
		for (i = 0; i < method.number; ++i) {
			tmpvalues.push_back(t0 + dt * (i + 1));
		}
	} else if (method.method == GeoDataRiverPathPoint::CtrlPointsAddMethod::am_EqRatio_Ratio) {
		double first = (t1 - t0) / ((1 - std::pow(method.param, static_cast<int>(method.number + 1))) / (1 - method.param));
		for (i = 0; i < method.number; ++i) {
			double t = t0 + first * ((1 - std::pow(method.param, static_cast<int>(i + 1))) / (1 - method.param));
			tmpvalues.push_back(t);
		}
	}
	for (i = index; i < values.size(); ++i) {
		tmpvalues.push_back(values[i]);
	}
	// Replace with the old one
	values = tmpvalues;
}

void GeoDataRiverPathPoint::addCtrlPoints(CtrlZonePosition position, unsigned int index, CtrlPointsAddMethod method)
{
	switch (position) {
	case zposCenterLine:
	case zposLeftBank:
	case zposRightBank:
		// CenterLine;
		addCtrlPointsToVector(CenterLineCtrlPoints, index, method);
		addCtrlPointsToVector(LeftBankCtrlPoints, index, method);
		addCtrlPointsToVector(RightBankCtrlPoints, index, method);
		break;
	case zposCenterToLeft:
		addCtrlPointsToVector(CenterToLeftCtrlPoints, index, method);
		break;
	case zposCenterToRight:
		addCtrlPointsToVector(CenterToRightCtrlPoints, index, method);
		break;
	}
}

void GeoDataRiverPathPoint::reposCtrlPoints(CtrlPointPosition position, int minindex, int maxindex, CtrlPointsAddMethod method)
{
	int numbers = maxindex - minindex + 1;
	QVector<double>& vec = CtrlPoints(position);
	double t0, t1;
	if (minindex == 0) {
		t0 = 0;
	} else {
		t0 = vec[minindex - 1];
	}
	if (maxindex == vec.size() - 1) {
		t1 = 1;
	} else {
		t1 = vec[maxindex + 1];
	}
	if (method.method == CtrlPointsAddMethod::am_Uniform) {
		double dt = (t1 - t0) / static_cast<double>(numbers + 1);
		for (int i = 0; i < numbers; ++i) {
			vec[minindex + i] = t0 + dt * (i + 1);
//			tmpvalues.push_back(t0 + dt * (i+1));
		}
	} else if (method.method == CtrlPointsAddMethod::am_EqRatio_Ratio) {
		double first;
		if (method.param == 1) {
			first = (t1 - t0) / static_cast<double>(numbers + 1);
		} else {
			first = (t1 - t0) / ((1 - std::pow(method.param, numbers + 1)) / (1 - method.param));
		}
		double t = t0;
		for (int i = 0; i < numbers; ++i) {
			t += first;
			vec[minindex + i] = t;
			first *= method.param;
		}
	}
}

void GeoDataRiverPathPoint::removeCtrlPoints(CtrlZonePosition position, std::set<int> indices)
{
	QVector<double> tmpvector, tmpvector2, tmpvector3;
	unsigned int i;
	switch (position) {
	case zposCenterToLeft:
		tmpvector.reserve(CenterToLeftCtrlPoints.size() - indices.size());
		for (i = 0; i < CenterToLeftCtrlPoints.size(); ++i) {
			//			if (i == *(indices.begin())){
			if (indices.begin() != indices.end() && i == *(indices.begin())) {
				// Skipped
				indices.erase(i);
			} else {
				tmpvector.push_back(CenterToLeftCtrlPoints[i]);
			}
		}
		CenterToLeftCtrlPoints = tmpvector;
		break;

	case zposCenterToRight:
		tmpvector.reserve(CenterToRightCtrlPoints.size() - indices.size());
		for (i = 0; i < CenterToRightCtrlPoints.size(); ++i) {
			if (indices.begin() != indices.end() && i == *(indices.begin())) {
				// Skipped
				indices.erase(i);
			} else {
				tmpvector.push_back(CenterToRightCtrlPoints[i]);
			}
		}
		CenterToRightCtrlPoints = tmpvector;
		break;

	case zposCenterLine:
	case zposLeftBank:
	case zposRightBank:
		tmpvector.reserve(CenterLineCtrlPoints.size() - indices.size());
		for (i = 0; i < CenterLineCtrlPoints.size(); ++i) {
			if (indices.begin() != indices.end() && i == *(indices.begin())) {
				indices.erase(i);
			} else {
				tmpvector.push_back(LeftBankCtrlPoints[i]);
				tmpvector2.push_back(RightBankCtrlPoints[i]);
				tmpvector3.push_back(CenterLineCtrlPoints[i]);
			}
		}
		LeftBankCtrlPoints = tmpvector;
		RightBankCtrlPoints = tmpvector2;
		CenterLineCtrlPoints = tmpvector3;
		break;

	}
}

std::vector<int> GeoDataRiverPathPoint::getPointsToInactivateUsingWaterElevation(HydraulicDataRiverSurveyWaterElevation *we)
{
	std::vector<int> ret;
	auto item = we->getItem(m_name);
	if (item == nullptr) {
		return ret;
	}
	if (! item->isSpecified()) {
		return ret;
	}

	GeoDataRiverCrosssection& cross = m_crosssection;
	GeoDataRiverCrosssection::AltitudeList& alist = cross.AltitudeInfo();

	int leftlimit = -1;
	int rightlimit = -1;
	int idx = 0;
	for (auto it = alist.begin(); it != alist.end(); ++it) {
		GeoDataRiverCrosssection::Altitude alt = *it;
		if (alt.position() > 0) {break;}
		if (alt.height() > item->value()) {
			leftlimit = idx;
		}
		++ idx;
	}
	for (auto it = alist.begin(); it != alist.end(); ++it) {
		GeoDataRiverCrosssection::Altitude alt = *it;
		if (alt.position() < 0) {continue;}
		if (alt.height() > item->value()) {
			rightlimit = idx;
			break;
		}
		++ idx;
	}

	for (int i = 0; i < leftlimit; ++i) {
		ret.push_back(i);
	}
	if (rightlimit != -1) {
		for (int i = rightlimit + 1; i < alist.size(); ++i) {
			ret.push_back(i);
		}
	}
	return ret;
}

void moveData(const QVector<double>& from, std::vector<double>& to)
{
	to.clear();
	for (int i = 0; i < from.size(); ++i) {
		to.push_back(from[i]);
	}
}

void moveData(const std::vector<double>& from, QVector<double>& to)
{
	to.clear();
	for (unsigned int i = 0; i < from.size(); ++i) {
		to.push_back(from[i]);
	}
}

void GeoDataRiverPathPoint::loadFromiRICLibObject(const iRICLib::RiverPathPoint* p)
{
	m_position.setX(p->positionX);
	m_position.setY(p->positionY);

	m_crosssectionDirection.setX(p->directionX);
	m_crosssectionDirection.setY(p->directionY);

	m_name = p->name.c_str();

	m_crosssection.setLeftShift(p->leftShift);
	m_crosssection.AltitudeInfo().clear();

	for (unsigned int i = 0; i < p->altitudes.size(); ++i) {
		iRICLib::Altitude libalt = p->altitudes.at(i);
		GeoDataRiverCrosssection::Altitude alt;
		alt.setPosition(libalt.position);
		alt.setHeight(libalt.height);
		alt.setActive(libalt.active == 1);
		m_crosssection.AltitudeInfo().push_back(alt);
	}
	m_crosssection.unsetFixedPointL();
	if (p->fixedPointLSet) {
		m_crosssection.setFixedPointL(p->fixedPointLIndex);
		m_crosssectionDirectionL.setX(p->directionLX);
		m_crosssectionDirectionL.setY(p->directionLY);
	}
	m_crosssection.unsetFixedPointR();
	if (p->fixedPointRSet) {
		m_crosssection.setFixedPointR(p->fixedPointRIndex);
		m_crosssectionDirectionR.setX(p->directionRX);
		m_crosssectionDirectionR.setY(p->directionRY);
	}

	m_gridSkip = (p->gridSkip == 1);

	moveData(p->centerToLeftCtrlPoints, CenterToLeftCtrlPoints);
	moveData(p->centerToRightCtrlPoints, CenterToRightCtrlPoints);
	moveData(p->centerLineCtrlPoints, CenterLineCtrlPoints);
	moveData(p->leftBankCtrlPoints, LeftBankCtrlPoints);
	moveData(p->rightBankCtrlPoints, RightBankCtrlPoints);
}

void GeoDataRiverPathPoint::saveToiRICLibObject(iRICLib::RiverPathPoint* p)
{
	p->positionX = m_position.x();
	p->positionY = m_position.y();

	p->directionX = m_crosssectionDirection.x();
	p->directionY = m_crosssectionDirection.y();

	p->name = iRIC::toStr(m_name);
	p->nameReal = m_name.toDouble();

	p->leftShift = m_crosssection.leftShift();

	p->altitudes.clear();
	for (auto it = m_crosssection.AltitudeInfo().begin(); it != m_crosssection.AltitudeInfo().end(); ++it) {
		GeoDataRiverCrosssection::Altitude alt = *it;
		iRICLib::Altitude libalt;
		libalt.position = alt.position();
		libalt.height = alt.height();
		if (alt.active()) {
			libalt.active = 1;
		} else {
			libalt.active = 0;
		}
		p->altitudes.push_back(libalt);
	}

	p->fixedPointLSet = m_crosssection.fixedPointLSet();
	p->directionLX = m_crosssectionDirectionL.x();
	p->directionLY = m_crosssectionDirectionL.y();
	p->fixedPointLIndex = m_crosssection.fixedPointLIndex();

	p->fixedPointRSet = m_crosssection.fixedPointRSet();
	p->directionRX = m_crosssectionDirectionR.x();
	p->directionRY = m_crosssectionDirectionR.y();
	p->fixedPointRIndex = m_crosssection.fixedPointRIndex();

	p->gridSkip = 0;
	if (m_gridSkip) {
		p->gridSkip = 1;
	}
	moveData(CenterToLeftCtrlPoints, p->centerToLeftCtrlPoints);
	moveData(CenterToRightCtrlPoints, p->centerToRightCtrlPoints);
	moveData(CenterLineCtrlPoints, p->centerLineCtrlPoints);
	moveData(LeftBankCtrlPoints, p->leftBankCtrlPoints);
	moveData(RightBankCtrlPoints, p->rightBankCtrlPoints);
}
