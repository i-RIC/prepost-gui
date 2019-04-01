#include "geodatariverpathpoint.h"
#include "geodatarivershapeinterpolator.h"

#include <misc/mathsupport.h>

bool RiverSplineSolver::m_linearMode = false;

AltitudeInterpolator::AltitudeInterpolator()
{}

AltitudeInterpolator::~AltitudeInterpolator()
{}

void AltitudeInterpolator::updateParameters()
{}

LinearAltitudeInterpolator::LinearAltitudeInterpolator(GeoDataRiverCrosssection::Altitude& v0, GeoDataRiverCrosssection::Altitude& v1) :
	m_value0 {v0},
	m_value1 {v1}
{}

LinearAltitudeInterpolator::LinearAltitudeInterpolator(double t0, GeoDataRiverCrosssection::Altitude& v0, double t1, GeoDataRiverCrosssection::Altitude& v1)
{
	m_value0.setPosition((t1 * v0.position() - t0 * v1.position()) / (t1 - t0));
	m_value0.setHeight((t1 * v0.height() - t0 * v1.height()) / (t1 - t0));
	m_value1.setPosition(((1 - t0) * v1.position() + (t1 - 1) * v0.position()) / (t1 - t0));
	m_value1.setHeight(((1 - t0) * v1.height() + (t1 - 1) * v0.height()) / (t1 - t0));
}

LinearAltitudeInterpolator::~LinearAltitudeInterpolator()
{}

void LinearAltitudeInterpolator::setValues(GeoDataRiverCrosssection::Altitude& v0, GeoDataRiverCrosssection::Altitude& v1)
{
	m_value0 = v0;
	m_value1 = v1;
}

void LinearAltitudeInterpolator::setValues(double t0, GeoDataRiverCrosssection::Altitude& v0, double t1, GeoDataRiverCrosssection::Altitude& v1)
{
	m_value0.setPosition((t1 * v0.position() - t0 * v1.position()) / (t1 - t0));
	m_value0.setHeight((t1 * v0.height() - t0 * v1.height()) / (t1 - t0));
	m_value1.setPosition(((1 - t0) * v1.position() + (t1 - 1) * v0.position()) / (t1 - t0));
	m_value1.setHeight(((1 - t0) * v1.height() + (t1 - 1) * v0.height()) / (t1 - t0));
}

GeoDataRiverCrosssection::Altitude LinearAltitudeInterpolator::interpolate(double t) const
{
	GeoDataRiverCrosssection::Altitude alt;
	alt.setPosition(m_value1.position() * t + m_value0.position() * (1. - t));
	alt.setHeight(m_value1.height() * t + m_value0.height() * (1. - t));
	return alt;
}

LinearLXSecInterpolator::LinearLXSecInterpolator(GeoDataRiverPathPoint* parent) : AltitudeInterpolator()
{
	m_parent = parent;
	updateParameters();
}

LinearLXSecInterpolator::~LinearLXSecInterpolator()
{}

void LinearLXSecInterpolator::updateParameters()
{
	// Get the left bank position
	GeoDataRiverCrosssection::Altitude leftbank(0, 0);
	try {
		leftbank = m_parent->crosssection().leftBank(true);
	} catch (GeoDataRiverCrosssection::ErrorCodes /* codes */) {}
	// Remove old objects
	for (auto l_it = m_interpolators.begin(); l_it != m_interpolators.end(); ++l_it) {
		delete l_it->second;
	}
	m_interpolators.clear();
	GeoDataRiverCrosssection::AltitudeList& AList = m_parent->crosssection().AltitudeInfo();
	auto it = AList.begin();
	if (it == AList.end()) {return;}
	while (it != AList.end() - 1 && it->position() <= 0) {
		while (it != AList.end() - 1 && ! it->active()) {
			++it;
		}
		auto it2 = it;
		if (it2 != AList.end() - 1) {++it2;}
		while (it2 != AList.end() - 1 && ! it2->active()) {
			++it2;
		}
		if (it != it2 && it->position() != it2->position()) {
			LinearAltitudeInterpolator* interpolator = new LinearAltitudeInterpolator(
				it2->position() / leftbank.position(), *it2,
				it->position() / leftbank.position(), *it
			);
			std::pair<double, LinearAltitudeInterpolator*> dl_pair =
				std::pair<double, LinearAltitudeInterpolator*>(it->position() / leftbank.position(), interpolator);
			m_interpolators.insert(dl_pair);
		}
		if (it != AList.end() - 1) {++it;}
	}
}

GeoDataRiverCrosssection::Altitude LinearLXSecInterpolator::interpolate(double t) const /*throw (ErrorCodes)*/
{
	if (t < 0 || t > 1) {
		throw ec_OutOfInterpolationRange;
	}
	auto it = m_interpolators.lower_bound(t);
	return (it->second)->interpolate(t);
}

LinearRXSecInterpolator::LinearRXSecInterpolator(GeoDataRiverPathPoint* parent) : AltitudeInterpolator()
{
	m_parent = parent;
	updateParameters();
}

LinearRXSecInterpolator::~LinearRXSecInterpolator()
{}

void LinearRXSecInterpolator::updateParameters()
{
	// Get the right bank position
	GeoDataRiverCrosssection::Altitude rightbank(0, 0);
	try {
		rightbank = m_parent->crosssection().rightBank(true);
	} catch (GeoDataRiverCrosssection::ErrorCodes /* codes */) {}
	// Delete old objects
	for (auto l_it = m_interpolators.begin(); l_it != m_interpolators.end(); ++l_it) {
		delete l_it->second;
	}
	m_interpolators.clear();
	GeoDataRiverCrosssection::AltitudeList& AList = m_parent->crosssection().AltitudeInfo();
	auto it = AList.end();
	if (it == AList.begin()) { return; }
	--it;
	while (it != AList.begin() && it->position() >= 0) {
		while (it != AList.begin() && ! it->active()) {
			--it;
		}
		auto it2 = it;
		if (it2 != AList.begin()) {--it2;}
		while (it2 != AList.begin() && ! it2->active()) {
			--it2;
		}
		if (it != it2 && it->position() != it2->position()) {
			LinearAltitudeInterpolator* interpolator = new LinearAltitudeInterpolator(
				it2->position() / rightbank.position(), *it2,
				it->position() / rightbank.position(), *it
			);
			std::pair<double, LinearAltitudeInterpolator*> dl_pair =
				std::pair<double, LinearAltitudeInterpolator*>(it->position() / rightbank.position(), interpolator);
			m_interpolators.insert(dl_pair);
		}
		if (it != AList.begin()) {--it;}
	}
}

GeoDataRiverCrosssection::Altitude LinearRXSecInterpolator::interpolate(double t) const /*throw (ErrorCodes)*/
{
	if (t < 0 || t > 1) {
		throw ec_OutOfInterpolationRange;
	}
	auto it = m_interpolators.lower_bound(t);
	return (it->second)->interpolate(t);
}

RiverInterpolator2D1::RiverInterpolator2D1(GeoDataRiverPathPoint* parent) :
	m_parent {parent}
{}

RiverInterpolator2D1::~RiverInterpolator2D1()
{}

RiverSplineSolver::RiverSplineSolver() :
	m_headPoint {nullptr}
{}

RiverSplineSolver::~RiverSplineSolver()
{}

void RiverSplineSolver::setHeadPoint(GeoDataRiverPathPoint* head)
{
	m_headPoint = head;
}

void RiverSplineSolver::update()
{
	if (m_linearMode) {
		// Create Linear interpolator
		GeoDataRiverPathPoint* tmpp = m_headPoint;
		// The first one is never used.
		RiverLinearInterpolator* interpolator =
			new RiverLinearInterpolator(this, getVector(tmpp->nextPoint()), getVector(tmpp->nextPoint()));
		setInterpolator(interpolator, tmpp);
		tmpp = tmpp->nextPoint();
		while (tmpp != nullptr) {
			GeoDataRiverPathPoint* nextp = tmpp->nextPoint();
			if (nextp == nullptr) {nextp = tmpp;}
			RiverLinearInterpolator* interpolator =
				new RiverLinearInterpolator(this, getVector(tmpp), getVector(nextp));
			setInterpolator(interpolator, tmpp);
			tmpp = tmpp->nextPoint();
		}
	} else {
		// Investigate the number of sections, and lengths of each sections.
		int num = 0;
		GeoDataRiverPathPoint* tmpp = m_headPoint;
		if (tmpp != nullptr) {tmpp = tmpp->nextPoint();}
		while (tmpp != nullptr && tmpp->nextPoint() != nullptr) {
			++num;
			tmpp = tmpp->nextPoint();
		}
		// The number of sections is now stored in num.
		m_XA.assign(num + 1, 0);
		m_XB.assign(num, 0);
		m_XC.assign(num + 1, 0);
		m_XD.assign(num, 0);
		m_YA.assign(num + 1, 0);
		m_YB.assign(num, 0);
		m_YC.assign(num + 1, 0);
		m_YD.assign(num, 0);
		m_Dist.assign(num, 0);
		tmpp = m_headPoint;
		if (tmpp != nullptr) {tmpp = tmpp->nextPoint();}
		int i = 0;
		while (tmpp != nullptr && tmpp->nextPoint() != nullptr) {
			// Put section length to Distvector
			m_Dist[i] = iRIC::length(getVector(tmpp->nextPoint()) - getVector(tmpp));
			// Copy x coordinate to m_XA
			m_XA[i] = getVector(tmpp).x();
			// Copy y foordinate to m_YA
			m_YA[i] = getVector(tmpp).y();
			++i;
			tmpp = tmpp->nextPoint();
		}
		if (tmpp != nullptr) {
			// Copy x coordinate to m_XA
			m_XA[i] = getVector(tmpp).x();
			// Copy y foordinate to m_YA
			m_YA[i] = getVector(tmpp).y();
		}
		std::vector<double> alfax, alfay, lx, ly, mx, my, zx, zy;
		alfax.assign(num, 0);
		alfay.assign(num, 0);
		lx.assign(num + 1, 0);
		ly.assign(num + 1, 0);
		mx.assign(num, 0);
		my.assign(num, 0);
		zx.assign(num + 1, 0);
		zy.assign(num + 1, 0);
		for (i = 1; i < num; ++i) {
			alfax[i] =
				3. / m_Dist[i]   * (m_XA[i + 1] - m_XA[i]) -
				3. / m_Dist[i - 1] * (m_XA[i] - m_XA[i - 1]);
			alfay[i] =
				3. / m_Dist[i]   * (m_YA[i + 1] - m_YA[i]) -
				3. / m_Dist[i - 1] * (m_YA[i] - m_YA[i - 1]);
		}
		if (num > 0) {
			lx[0] = 1;
			ly[0] = 1;
			mx[0] = 0;
			my[0] = 0;
			zx[0] = 0;
			zy[0] = 0;
		}
		for (i = 1; i < num; ++i) {
			lx[i] = 2 * (m_Dist[i - 1] + m_Dist[i]) - m_Dist[i - 1] * mx[i - 1];
			ly[i] = 2 * (m_Dist[i - 1] + m_Dist[i]) - m_Dist[i - 1] * my[i - 1];
			mx[i] = m_Dist[i] / lx[i];
			my[i] = m_Dist[i] / ly[i];
			zx[i] = (alfax[i] - m_Dist[i - 1] * zx[i - 1]) / lx[i];
			zy[i] = (alfay[i] - m_Dist[i - 1] * zy[i - 1]) / ly[i];
		}
		if (num >= 0) {
			lx[num] = 1;
			ly[num] = 1;
			zx[num] = 0;
			zy[num] = 0;
			m_XC[num] = 0;
			m_YC[num] = 0;
		}
		for (i = num - 1; i >= 0; --i) {
			m_XC[i] = zx[i] - mx[i] * m_XC[i + 1];
			m_YC[i] = zy[i] - my[i] * m_YC[i + 1];
			m_XB[i] =
				(m_XA[i + 1] - m_XA[i]) / m_Dist[i] -
				m_Dist[i] * (m_XC[i + 1] + 2 * m_XC[i]) / 3.;
			m_YB[i] =
				(m_YA[i + 1] - m_YA[i]) / m_Dist[i] -
				m_Dist[i] * (m_YC[i + 1] + 2 * m_YC[i]) / 3.;
			m_XD[i] = (m_XC[i + 1] - m_XC[i]) / 3. / m_Dist[i];
			m_YD[i] = (m_YC[i + 1] - m_YC[i]) / 3. / m_Dist[i];
		}
		// Set the interpolators created.
		tmpp = m_headPoint;
		i = - 1;
		while (tmpp != 0) {
			RiverSplineInterpolator* interpolator = new RiverSplineInterpolator(this, i);
			setInterpolator(interpolator, tmpp);
			++i;
			tmpp = tmpp->nextPoint();
		}

	}
}

QPointF RiverSplineSolver::interpolate(int index, double d) const
{
	d *= m_Dist[index];
	double x;
	double y;
	x = m_XA[index] +
			m_XB[index] * d +
			m_XC[index] * d * d +
			m_XD[index] * d * d * d;
	y = m_YA[index] +
			m_YB[index] * d +
			m_YC[index] * d * d +
			m_YD[index] * d * d * d;
	return QPointF(x, y);
}

RiverCenterLineSolver::RiverCenterLineSolver() :
	RiverSplineSolver()
{}

RiverCenterLineSolver::~RiverCenterLineSolver()
{}

QPointF RiverCenterLineSolver::getVector(GeoDataRiverPathPoint* p)
{
	return p->position();
}

void RiverCenterLineSolver::setInterpolator(Interpolator2D1* interpolator, GeoDataRiverPathPoint* p)
{
	delete p->riverCenter();
	p->setRiverCenter(interpolator);
}

RiverLeftBankSolver::RiverLeftBankSolver() :
	RiverSplineSolver()
{}

RiverLeftBankSolver::~RiverLeftBankSolver()
{}

QPointF RiverLeftBankSolver::getVector(GeoDataRiverPathPoint* p)
{
	try {
		return p->crosssectionPosition(p->crosssection().leftBank(true).position());
	} catch (GeoDataRiverCrosssection::ErrorCodes /* code */) {
		return p->position();
	}
}

void RiverLeftBankSolver::setInterpolator(Interpolator2D1* interpolator, GeoDataRiverPathPoint* p)
{
	delete p->leftBank();
	p->setLeftBank(interpolator);
}

RiverRightBankSolver::RiverRightBankSolver() :
	RiverSplineSolver()
{}

RiverRightBankSolver::~RiverRightBankSolver()
{}

QPointF RiverRightBankSolver::getVector(GeoDataRiverPathPoint* p)
{
	try {
		return p->crosssectionPosition(p->crosssection().rightBank(true).position());
	} catch (GeoDataRiverCrosssection::ErrorCodes /* code */) {
		return p->position();
	}
}

void RiverRightBankSolver::setInterpolator(Interpolator2D1* interpolator, GeoDataRiverPathPoint* p)
{
	delete p->rightBank();
	p->setRightBank(interpolator);
}

RiverGridCtrlSolver::RiverGridCtrlSolver() :
	RiverSplineSolver(),
	m_bankSide {bs_LeftBank},
	m_index {0}
{}

RiverGridCtrlSolver::~RiverGridCtrlSolver()
{}

void RiverGridCtrlSolver::setIndex(int index)
{
	m_index = index;
}

int RiverGridCtrlSolver::index() const
{
	return m_index;
}

void RiverGridCtrlSolver::setBankSide(Bank side)
{
	m_bankSide = side;
}

RiverGridCtrlSolver::Bank RiverGridCtrlSolver::bankSide() const
{
	return m_bankSide;
}

QPointF RiverGridCtrlSolver::getVector(GeoDataRiverPathPoint* p)
{
	if (m_bankSide == bs_LeftBank) {
		if (p->CenterToLeftCtrlPoints.size() == 0) {return p->position();}
		double tmpdbl = p->lXSec()->interpolate(p->CenterToLeftCtrlPoints[m_index]).position();
		return p->crosssectionPosition(tmpdbl);
	} else {
		if (p->CenterToRightCtrlPoints.size() == 0) {return p->position();}
		double tmpdbl = p->rXSec()->interpolate(p->CenterToRightCtrlPoints[m_index]).position();
		return p->crosssectionPosition(tmpdbl);
	}
}

void RiverGridCtrlSolver::setInterpolator(Interpolator2D1* interpolator, GeoDataRiverPathPoint* p)
{
	if (m_bankSide == bs_LeftBank) {
		if (p->LGridLines()[m_index] != 0) {
			delete p->LGridLines()[m_index];
		}
		p->LGridLines()[m_index] = interpolator;
	} else {
		if (p->RGridLines()[m_index] != 0) {
			delete p->RGridLines()[m_index];
		}
		p->RGridLines()[m_index] = interpolator;
	}
}

RiverBackgroundGridCtrlSolver::RiverBackgroundGridCtrlSolver() :
	RiverSplineSolver(),
	m_bankSide {bs_LeftBank},
	m_parameter {0},
	m_index {0}
{}

RiverBackgroundGridCtrlSolver::~RiverBackgroundGridCtrlSolver()
{}

int RiverBackgroundGridCtrlSolver::index() const
{
	return m_index;
}

void RiverBackgroundGridCtrlSolver::setIndex(int index)
{
	m_index = index;
}

double RiverBackgroundGridCtrlSolver::parameter() const
{
	return m_parameter;
}

void RiverBackgroundGridCtrlSolver::setParameter(double param)
{
	m_parameter = param;
}

RiverBackgroundGridCtrlSolver::Bank RiverBackgroundGridCtrlSolver::bankSide() const
{
	return m_bankSide;
}

void RiverBackgroundGridCtrlSolver::setBankSide(Bank side)
{
	m_bankSide = side;
}

QPointF RiverBackgroundGridCtrlSolver::getVector(GeoDataRiverPathPoint* p)
{
	if (m_bankSide == bs_LeftBank) {
		double tmpdbl = p->lXSec()->interpolate(m_parameter).position();
		return p->crosssectionPosition(tmpdbl);
	} else {
		double tmpdbl = p->rXSec()->interpolate(m_parameter).position();
		return p->crosssectionPosition(tmpdbl);
	}
}

void RiverBackgroundGridCtrlSolver::setInterpolator(Interpolator2D1* interpolator, GeoDataRiverPathPoint* p)
{
	if (m_bankSide == bs_LeftBank) {
		if (p->backgroundLGridLines()[m_index] != 0) {
			delete p->backgroundLGridLines()[m_index];
		}
		p->backgroundLGridLines()[m_index] = interpolator;
	} else {
		if (p->backgroundRGridLines()[m_index] != 0) {
			delete p->backgroundRGridLines()[m_index];
		}
		p->backgroundRGridLines()[m_index] = interpolator;
	}
}

RiverSplineInterpolator::RiverSplineInterpolator(RiverSplineSolver* parent, int index) :
	m_parent {parent},
	m_Index {index}
{}

RiverSplineInterpolator::~RiverSplineInterpolator()
{}

void RiverSplineInterpolator::updateParameters()
{
	m_parent->update();
}

QPointF RiverSplineInterpolator::interpolate(double t) const
{
	return m_parent->interpolate(m_Index, t);
}

Interpolator2D1* RiverSplineInterpolator::copy() const
{
	Interpolator2D1* copy = new RiverSplineInterpolatorCopy(
		m_parent->m_Dist[m_Index],
		m_parent->m_XA[m_Index],
		m_parent->m_XB[m_Index],
		m_parent->m_XC[m_Index],
		m_parent->m_XD[m_Index],
		m_parent->m_YA[m_Index],
		m_parent->m_YB[m_Index],
		m_parent->m_YC[m_Index],
		m_parent->m_YD[m_Index]);
	return copy;
}

RiverSplineInterpolatorCopy::RiverSplineInterpolatorCopy(double d, double xa, double xb, double xc, double xd, double ya, double yb, double yc, double yd) :
	m_D {d},
	m_XA {xa},
	m_XB {xb},
	m_XC {xc},
	m_XD {xd},
	m_YA {ya},
	m_YB {yb},
	m_YC {yc},
	m_YD {yd}
{}

RiverSplineInterpolatorCopy::~RiverSplineInterpolatorCopy()
{}

void RiverSplineInterpolatorCopy::updateParameters()
{}

QPointF RiverSplineInterpolatorCopy::interpolate(double t) const
{
	t *= m_D;
	double x =
		m_XA +
		m_XB * t +
		m_XC * t * t +
		m_XD * t * t * t;
	double y =
		m_YA +
		m_YB * t +
		m_YC * t * t +
		m_YD * t * t * t;
	return QPointF(x, y);
}

Interpolator2D1* RiverSplineInterpolatorCopy::copy() const
{
	return new RiverSplineInterpolatorCopy(m_D, m_XA, m_XB, m_XC, m_XD, m_YA, m_YB, m_YC, m_YD);
}

RiverLinearInterpolator::RiverLinearInterpolator(RiverSplineSolver* parent, const QPointF& v0, const QPointF& v1) :
	Interpolator2D1(),
	m_Parent {parent}
{
	m_Interpolator.setValues(v0, v1);
}

RiverLinearInterpolator::~RiverLinearInterpolator()
{}

void RiverLinearInterpolator::updateParameters()
{
	m_Parent->update();
}

QPointF RiverLinearInterpolator::interpolate(double t) const
{
	return m_Interpolator.interpolate(t);
}

Interpolator2D1* RiverLinearInterpolator::copy() const
{
	return new RiverLinearInterpolator(m_Parent, m_Interpolator.interpolate(0), m_Interpolator.interpolate(1));
}

bool RiverSplineSolver::linearMode()
{
	return m_linearMode;
}

void RiverSplineSolver::setLinearMode(bool linearmode, GeoDataRiverPathPoint* /*head*/, bool /*noundo*/)
{
	m_linearMode = linearmode;
}
