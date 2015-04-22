#include "rawdatariversurveyctrlpointbackup.h"

void RawDataRiverSurveyCtrlPointBackup::backup(RawDataRiverPathPoint* point, RawDataRiverPathPoint::CtrlZonePosition position){
	RawDataRiverPathPoint* tmpp;
	CtrlPoints points;
	m_ctrlPoints.clear();
	switch (position){
	case RawDataRiverPathPoint::zposCenterToLeft:
		// point には、 HeadPoint() が入っているはず。
		tmpp = point;
		if (tmpp != 0){tmpp = tmpp->nextPoint();}
		while (tmpp != 0){
			points.point = tmpp;
			points.position = position;
			points.ctrlPointVector = tmpp->CenterToLeftCtrlPoints;
			m_ctrlPoints.push_back(points);
			tmpp = tmpp->nextPoint();
		}
		break;
	case RawDataRiverPathPoint::zposCenterToRight:
		// point には、 HeadPoint() が入っているはず。
		tmpp = point;
		if (tmpp != 0){tmpp = tmpp->nextPoint();}
		while (tmpp != 0){
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
		// point の指す左岸、右岸、河川中心を退避すればよい。
		// 左岸
		points.point = point;
		points.position = RawDataRiverPathPoint::zposLeftBank;
		points.ctrlPointVector = point->LeftBankCtrlPoints;
		m_ctrlPoints.push_back(points);
		// 右岸
		points.point = point;
		points.position = RawDataRiverPathPoint::zposRightBank;
		points.ctrlPointVector = point->RightBankCtrlPoints;
		m_ctrlPoints.push_back(points);
		// 河川中心線
		points.point = point;
		points.position = RawDataRiverPathPoint::zposCenterLine;
		points.ctrlPointVector = point->CenterLineCtrlPoints;
		m_ctrlPoints.push_back(points);
		break;
	}
}

void RawDataRiverSurveyCtrlPointBackup::restore(){
	for (auto it = m_ctrlPoints.begin(); it != m_ctrlPoints.end(); ++it){
		myRestore(*it);
	}
}

void RawDataRiverSurveyCtrlPointBackup::myRestore(CtrlPoints& points){
	switch (points.position){
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

