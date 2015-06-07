#include "rawdatariversurveyctrlpointbackup.h"

void RawDataRiverSurveyCtrlPointBackup::backup(RawDataRiverPathPoint* point, RawDataRiverPathPoint::CtrlZonePosition position)
{
	RawDataRiverPathPoint* tmpp;
	CtrlPoints points;
	m_ctrlPoints.clear();
	switch (position) {
	case RawDataRiverPathPoint::zposCenterToLeft:
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
	case RawDataRiverPathPoint::zposCenterToRight:
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
	case RawDataRiverPathPoint::zposCenterLine:
	case RawDataRiverPathPoint::zposLeftBank:
	case RawDataRiverPathPoint::zposRightBank:
		// Left bank
		points.point = point;
		points.position = RawDataRiverPathPoint::zposLeftBank;
		points.ctrlPointVector = point->LeftBankCtrlPoints;
		m_ctrlPoints.push_back(points);
		// Right bank
		points.point = point;
		points.position = RawDataRiverPathPoint::zposRightBank;
		points.ctrlPointVector = point->RightBankCtrlPoints;
		m_ctrlPoints.push_back(points);
		// River center line
		points.point = point;
		points.position = RawDataRiverPathPoint::zposCenterLine;
		points.ctrlPointVector = point->CenterLineCtrlPoints;
		m_ctrlPoints.push_back(points);
		break;
	}
}

void RawDataRiverSurveyCtrlPointBackup::restore()
{
	for (const CtrlPoints& p : m_ctrlPoints) {
		myRestore(p);
	}
}

void RawDataRiverSurveyCtrlPointBackup::myRestore(const CtrlPoints& points)
{
	switch (points.position) {
	case RawDataRiverPathPoint::pposCenterToLeft:
		points.point->CenterToLeftCtrlPoints = points.ctrlPointVector;
		break;
	case RawDataRiverPathPoint::pposCenterToRight:
		points.point->CenterToRightCtrlPoints = points.ctrlPointVector;
		break;
	case RawDataRiverPathPoint::pposCenterLine:
		points.point->CenterLineCtrlPoints = points.ctrlPointVector;
		break;
	case RawDataRiverPathPoint::pposLeftBank:
		points.point->LeftBankCtrlPoints = points.ctrlPointVector;
		break;
	case RawDataRiverPathPoint::pposRightBank:
		points.point->RightBankCtrlPoints = points.ctrlPointVector;
		break;
	}
}

