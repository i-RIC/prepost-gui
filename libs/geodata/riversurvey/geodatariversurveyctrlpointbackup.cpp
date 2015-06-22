#include "geodatariversurveyctrlpointbackup.h"

void GeoDataRiverSurveyCtrlPointBackup::backup(GeoDataRiverPathPoint* point, GeoDataRiverPathPoint::CtrlZonePosition position)
{
	GeoDataRiverPathPoint* tmpp;
	CtrlPoints points;
	m_ctrlPoints.clear();
	switch (position) {
	case GeoDataRiverPathPoint::zposCenterToLeft:
		tmpp = point;
		if (tmpp != nullptr) {tmpp = tmpp->nextPoint();}
		while (tmpp != nullptr) {
			points.point = tmpp;
			points.position = position;
			points.ctrlPointVector = tmpp->CenterToLeftCtrlPoints;
			m_ctrlPoints.push_back(points);
			tmpp = tmpp->nextPoint();
		}
		break;
	case GeoDataRiverPathPoint::zposCenterToRight:
		tmpp = point;
		if (tmpp != nullptr) {tmpp = tmpp->nextPoint();}
		while (tmpp != nullptr) {
			points.point = tmpp;
			points.position = position;
			points.ctrlPointVector = tmpp->CenterToRightCtrlPoints;
			m_ctrlPoints.push_back(points);
			tmpp = tmpp->nextPoint();
		}
		break;
	case GeoDataRiverPathPoint::zposCenterLine:
	case GeoDataRiverPathPoint::zposLeftBank:
	case GeoDataRiverPathPoint::zposRightBank:
		// Left bank
		points.point = point;
		points.position = GeoDataRiverPathPoint::zposLeftBank;
		points.ctrlPointVector = point->LeftBankCtrlPoints;
		m_ctrlPoints.push_back(points);
		// Right bank
		points.point = point;
		points.position = GeoDataRiverPathPoint::zposRightBank;
		points.ctrlPointVector = point->RightBankCtrlPoints;
		m_ctrlPoints.push_back(points);
		// River center line
		points.point = point;
		points.position = GeoDataRiverPathPoint::zposCenterLine;
		points.ctrlPointVector = point->CenterLineCtrlPoints;
		m_ctrlPoints.push_back(points);
		break;
	}
}

void GeoDataRiverSurveyCtrlPointBackup::restore()
{
	for (const CtrlPoints& p : m_ctrlPoints) {
		myRestore(p);
	}
}

void GeoDataRiverSurveyCtrlPointBackup::myRestore(const CtrlPoints& points)
{
	switch (points.position) {
	case GeoDataRiverPathPoint::pposCenterToLeft:
		points.point->CenterToLeftCtrlPoints = points.ctrlPointVector;
		break;
	case GeoDataRiverPathPoint::pposCenterToRight:
		points.point->CenterToRightCtrlPoints = points.ctrlPointVector;
		break;
	case GeoDataRiverPathPoint::pposCenterLine:
		points.point->CenterLineCtrlPoints = points.ctrlPointVector;
		break;
	case GeoDataRiverPathPoint::pposLeftBank:
		points.point->LeftBankCtrlPoints = points.ctrlPointVector;
		break;
	case GeoDataRiverPathPoint::pposRightBank:
		points.point->RightBankCtrlPoints = points.ctrlPointVector;
		break;
	}
}

