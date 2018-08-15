#include "geodatarivercrosssection.h"
#include "geodatariverpathpoint.h"

#include <misc/interpolator.h>

GeoDataRiverCrosssection::GeoDataRiverCrosssection() :
	m_fixedPointLSet {false},
	m_fixedPointRSet {false},
	m_leftShift {0}
{}

GeoDataRiverCrosssection::~GeoDataRiverCrosssection()
{}

void GeoDataRiverCrosssection::setParent(GeoDataRiverPathPoint* point)
{
	m_parent = point;
}

double GeoDataRiverCrosssection::leftShift() const
{
	return m_leftShift;
}

void GeoDataRiverCrosssection::setLeftShift(double shift)
{
	m_leftShift = shift;
}

void GeoDataRiverCrosssection::expand(double ratio)
{
	if (ratio == 0) {
		return;
	}
	for (Altitude& alt : m_altitudeInfo) {
		alt.setPosition(alt.position() * ratio);
	}
	m_leftShift *= ratio;
}

void GeoDataRiverCrosssection::moveCenter(double offset)
{
	for (Altitude& alt : m_altitudeInfo) {
		alt.setPosition(alt.position() - offset);
	}
	m_leftShift += offset;
}

int GeoDataRiverCrosssection::addPoint(double position, double height)
{
	int i = 0;
	for (
		auto it = m_altitudeInfo.begin();
		it != m_altitudeInfo.end() && it->position() < position;
		++it, ++i
	) {
		// Do nothing.
	}
	addPoint(i, position, height);
	return i;
}

void GeoDataRiverCrosssection::addPoint(int index, double position, double height)
{
	Altitude alt(position, height);
	m_altitudeInfo.insert(index, alt);
//	m_Parent->UpdateRiverShapeInterpolators();
//	m_Parent->UpdateXSecInterpolators();
	updateFixedPointDivs();
}

void GeoDataRiverCrosssection::removePoint(int index) /*throw (ErrorCodes)*/
{
	if (index < 0 || index >= m_altitudeInfo.count()) {
		throw ec_OutOfIndex;
	}
	m_altitudeInfo.removeAt(index);

	if (! enoughActivePoints()) {
		throw ec_AltitudesMustExistTwo;
	}

//	m_Parent->UpdateRiverShapeInterpolators();
//	m_Parent->UpdateXSecInterpolators();
	updateFixedPointDivs();
}

void GeoDataRiverCrosssection::removePoint(const QList<int>& indices) /*throw (ErrorCodes)*/
{
	for (auto it = indices.begin(); it != indices.end(); ++it) {
		int index = *it;
		if (m_fixedPointLSet && (index == 0 || index == m_fixedPointL)) {
			throw ec_FixDelete;
		}
		if (m_fixedPointRSet && (index == m_altitudeInfo.count() - 1 || index == m_fixedPointR)) {
			throw ec_FixDelete;
		}
	}

	int offset = 0;
	for (auto it = indices.begin(); it != indices.end(); ++it) {
		int index = *it - offset;
		m_altitudeInfo.removeAt(index);
		++offset;
	}

	if (! enoughActivePoints()) {
		throw ec_AltitudesMustExistTwo;
	}

	int l = leftBankIndex(true);
	int r = rightBankIndex(true);
	if (m_altitudeInfo[l].position() > 0 || m_altitudeInfo[r].position() < 0) {
		throw ec_AltitudesBiased;
	}
//	m_Parent->UpdateRiverShapeInterpolators();
//	m_Parent->UpdateXSecInterpolators();
	updateFixedPointDivs();
}

int GeoDataRiverCrosssection::leftBankIndex(bool OnlyActive) /*throw (ErrorCodes)*/
{
	if (m_altitudeInfo.count() == 0) {
		throw ec_NoAltitudeExists;
	}
	if (! OnlyActive) {
		return 0;
	}
	int i;
	for (i = 0; i < m_altitudeInfo.count() && (! m_altitudeInfo.at(i).active()); ++i) {
		// try next
	}
	if (i == m_altitudeInfo.count()) {
		throw ec_NoAltitudeExists;
	}
	return i;
}

GeoDataRiverCrosssection::Altitude& GeoDataRiverCrosssection::leftBank(bool OnlyActive) /*throw (ErrorCodes)*/
{
	return m_altitudeInfo[leftBankIndex(OnlyActive)];
}

int GeoDataRiverCrosssection::rightBankIndex(bool OnlyActive) /*throw (ErrorCodes)*/
{
	if (m_altitudeInfo.size() == 0) {
		throw ec_NoAltitudeExists;
	}
	if (! OnlyActive) {
		return m_altitudeInfo.count() - 1;
	}
	int i;
	for (i = m_altitudeInfo.count() - 1; i >= 0 && (! m_altitudeInfo.at(i).active()); --i) {
		// try next
	}
	if (i < 0) {
		throw ec_NoAltitudeExists;
	}
	return i;
}

GeoDataRiverCrosssection::Altitude& GeoDataRiverCrosssection::rightBank(bool OnlyActive) /*throw (ErrorCodes)*/
{
	return m_altitudeInfo[rightBankIndex(OnlyActive)];
}

unsigned int GeoDataRiverCrosssection::numOfAltitudes(bool OnlyActive)
{
	if (! OnlyActive) {
		return static_cast<unsigned int>(m_altitudeInfo.count());
	}
	unsigned int actives = 0;
	for (auto it = m_altitudeInfo.begin(); it != m_altitudeInfo.end(); ++it) {
		if (it->active()) {
			++actives;
		}
	}
	return actives;
}

GeoDataRiverCrosssection::AltitudeList& GeoDataRiverCrosssection::AltitudeInfo()
{
	return m_altitudeInfo;
}

const GeoDataRiverCrosssection::AltitudeList& GeoDataRiverCrosssection::AltitudeInfo() const
{
	return m_altitudeInfo;
}

GeoDataRiverPathPoint* GeoDataRiverCrosssection::parent() const
{
	return m_parent;
}

bool GeoDataRiverCrosssection::fixedPointLSet() const
{
	return m_fixedPointLSet;
}

const GeoDataRiverCrosssection::Altitude& GeoDataRiverCrosssection::fixedPointL() const
{
	return m_altitudeInfo.at(m_fixedPointL);
}

int GeoDataRiverCrosssection::fixedPointLIndex() const
{
	return m_fixedPointL;
}

bool GeoDataRiverCrosssection::fixedPointRSet() const
{
	return m_fixedPointRSet;
}

const GeoDataRiverCrosssection::Altitude& GeoDataRiverCrosssection::fixedPointR() const
{
	return m_altitudeInfo.at(m_fixedPointR);
}

int GeoDataRiverCrosssection::fixedPointRIndex() const
{
	return m_fixedPointR;
}

QList<int> GeoDataRiverCrosssection::selectRegion(double position1, double position2, double height1, double height2)
{
	double posmin, posmax;
	double heightmin, heightmax;
	if (position1 < position2) {
		posmin = position1;
		posmax = position2;
	} else {
		posmin = position2;
		posmax = position1;
	}
	if (height1 < height2) {
		heightmin = height1;
		heightmax = height2;
	} else {
		heightmin = height2;
		heightmax = height1;
	}
	QList<int> result;
	int i = 0;
	for (auto it = m_altitudeInfo.begin(); it != m_altitudeInfo.end(); ++i, ++it) {
		if (
			it->position() >= posmin &&
			it->position() <= posmax &&
			it->height()   >= heightmin &&
			it->height()   <= heightmax
		) {
			result.push_back(i);
		}
	}
	return result;
}

void GeoDataRiverCrosssection::movePoint(double Hoffset, double Voffset, const QList<int>& Indices)
{
	for (int i = 0; i < Indices.count(); ++i) {
		if (m_fixedPointLSet && Indices.at(i) == m_fixedPointL) {
			Hoffset = 0;
			break;
		}
		if (m_fixedPointRSet && Indices.at(i) == m_fixedPointR) {
			Hoffset = 0;
			break;
		}
	}
	for (int i = 0; i < Indices.count(); ++i) {
		Altitude& a = m_altitudeInfo[Indices.at(i)];
		a.setPosition(a.position() + Hoffset);
		a.setHeight(a.height() + Voffset);
	}
//	m_Parent->UpdateRiverShapeInterpolators();
//	m_Parent->UpdateXSecInterpolators();
}

void GeoDataRiverCrosssection::movePoint(int index, double newposition, double newheight)
{
	movePosition(index, newposition);
	moveHeight(index, newheight);
}

void GeoDataRiverCrosssection::movePosition(int index, double newposition)
{
	Altitude& a = m_altitudeInfo[index];
	// fixed points can not moved in horizontal direction.
	if (m_fixedPointLSet && index == m_fixedPointL) {
		return;
	}
	if (m_fixedPointRSet && index == m_fixedPointR) {
		return;
	}
	a.setPosition(newposition);
}

void GeoDataRiverCrosssection::moveHeight(int index, double newheight)
{
	Altitude& a = m_altitudeInfo[index];
	a.setHeight(newheight);
}
void GeoDataRiverCrosssection::setFixedPointL(int index)
{
	m_fixedPointLSet = true;
	m_fixedPointL = index;
	updateFixedPointDivs();
}

void GeoDataRiverCrosssection::setFixedPointR(int index)
{
	m_fixedPointRSet = true;
	m_fixedPointR = index;
	updateFixedPointDivs();
}
void GeoDataRiverCrosssection::unsetFixedPointL()
{
	if (! m_fixedPointLSet) {return;}
	m_fixedPointLSet = false;
	removePoint(0);
	if (m_fixedPointRSet) {
		--m_fixedPointR;
	}
//	Parent()->UpdateXSecInterpolators();
//	Parent()->UpdateRiverShapeInterpolators();
}

void GeoDataRiverCrosssection::unsetFixedPointR()
{
	if (! m_fixedPointRSet) {return;}
	m_fixedPointRSet = false;
	removePoint(m_altitudeInfo.count() - 1);
//	Parent()->UpdateXSecInterpolators();
//	Parent()->UpdateRiverShapeInterpolators();
}

double GeoDataRiverCrosssection::fixedPointLDiv() const
{
	return m_fixedPointLDiv;
}

double GeoDataRiverCrosssection::fixedPointRDiv() const
{
	return m_fixedPointRDiv;
}

void GeoDataRiverCrosssection::activate(int index, bool a) /*throw (ErrorCodes)*/
{
	QList<int> indices;
	indices.append(index);
	activate(indices, a);
}

void GeoDataRiverCrosssection::activate(const QList<int>& indices, bool a) /*throw (ErrorCodes)*/
{

	// When fixed points or the end points are included, they can not be inactivated.
	if (! a) {
		for (auto ii = indices.begin(); ii != indices.end(); ++ii) {
			int index = *ii;
			// Left fixed point exists
			if (m_fixedPointLSet && (index == 0 || index == m_fixedPointL)) {
				throw ec_FixInactivate;
			}
			// Right fixed point exists
			if (m_fixedPointRSet && (index == m_altitudeInfo.count() - 1 || index == m_fixedPointR)) {
				throw ec_FixInactivate;
			}
		}
	}
	for (auto ii = indices.begin(); ii != indices.end(); ++ii) {
		(*(m_altitudeInfo.begin() + (*ii))).setActive(a);
//		m_altitudeInfo[*ii].setActive(a);
	}
	if (! enoughActivePoints()) {
		throw ec_AltitudesMustExistTwo;
	}
	int l = leftBankIndex(true);
	int r = rightBankIndex(true);
	if (m_altitudeInfo[l].position() > 0 || m_altitudeInfo[r].position() < 0) {
		throw ec_AltitudesBiased;
	}
}

bool GeoDataRiverCrosssection::enoughActivePoints()
{
	int count = 0;
	for (auto it = m_altitudeInfo.begin(); it != m_altitudeInfo.end(); ++it) {
		if (it->active()) {
			++count;
			if (count == 2) {return true;}
		}
	}
	return false;
}
void GeoDataRiverCrosssection::updateFixedPointDivs()
{
	if (m_fixedPointLSet) {
		m_fixedPointLDiv = m_altitudeInfo.at(m_fixedPointL).position() / leftBank(true).position();
	}
	if (m_fixedPointRSet) {
		m_fixedPointRDiv = m_altitudeInfo.at(m_fixedPointR).position() / rightBank(true).position();
	}
}

