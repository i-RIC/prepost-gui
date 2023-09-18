#include "gridcreatingconditionriversurvey15d_deletectrlpointcommand.h"

#include <geodata/riversurvey/geodatariversurvey.h>
#include <geodata/riversurvey/geodatariversurveyctrlpointbackup.h>

GridCreatingConditionRiverSurvey15D::DeleteCtrlPointCommand::DeleteCtrlPointCommand(GridCreatingConditionRiverSurvey15D* cond) :
	QUndoCommand(GridCreatingConditionRiverSurvey15D::tr("Delete Selected Control Points")),
	m_condition {cond}
{
	executeDeleteCtrlPoints();
}

void GridCreatingConditionRiverSurvey15D::DeleteCtrlPointCommand::redo()
{
	apply(m_after);
}

void GridCreatingConditionRiverSurvey15D::DeleteCtrlPointCommand::undo()
{
	apply(m_before);
}

void GridCreatingConditionRiverSurvey15D::DeleteCtrlPointCommand::executeDeleteCtrlPoints()
{
	GeoDataRiverSurveyCtrlPointBackup* backup;

	typedef std::set<int> iset;
	iset EmptyIndices;
	iset LeftRemoveIndices;
	iset RightRemoveIndices;

	std::set<GeoDataRiverPathPoint*> points;
	std::map<GeoDataRiverPathPoint*, iset> PointIndices;

	bool RemoveCenterToLeft = false;
	bool RemoveCenterToRight = false;

	for (auto it = m_condition->m_selectedCtrlPointInfoList.begin(); it != m_condition->m_selectedCtrlPointInfoList.end(); ++it) {
		if (it->Position == GeoDataRiverPathPoint::pposCenterToLeft) {
			RemoveCenterToLeft = true;
			LeftRemoveIndices.insert(it->Index);
		} else if (it->Position == GeoDataRiverPathPoint::pposCenterToRight) {
			RemoveCenterToRight = true;
			RightRemoveIndices.insert(it->Index);
		} else {
			points.insert(it->Point);
			auto psetit = PointIndices.find(it->Point);
			if (psetit == PointIndices.end()) {
				std::pair<GeoDataRiverPathPoint*, iset> pi_pair =
					std::pair<GeoDataRiverPathPoint*, iset>(it->Point, EmptyIndices);
				auto insertresult =
					PointIndices.insert(pi_pair);
				psetit = insertresult.first;
			}
			psetit->second.insert(it->Index);
		}
	}

	GeoDataRiverPathPoint* headPoint = m_condition->m_riverSurvey->headPoint();

	// Save backup
	if (RemoveCenterToLeft) {
		backup = new GeoDataRiverSurveyCtrlPointBackup();
		backup->backup(headPoint, GeoDataRiverPathPoint::zposCenterToLeft);
		m_before.push_back(backup);
	}
	if (RemoveCenterToRight) {
		backup = new GeoDataRiverSurveyCtrlPointBackup();
		backup->backup(headPoint, GeoDataRiverPathPoint::zposCenterToRight);
		m_before.push_back(backup);
	}
	for (GeoDataRiverPathPoint* point : points) {
		backup = new GeoDataRiverSurveyCtrlPointBackup();
		backup->backup(point, GeoDataRiverPathPoint::zposCenterLine);
		m_before.push_back(backup);
	}

	// Delete the points
	if (RemoveCenterToLeft) {
		GeoDataRiverPathPoint* tmpp = m_condition->m_riverSurvey->headPoint();
		if (tmpp != nullptr) {tmpp = tmpp->nextPoint();}
		while (tmpp != nullptr) {
			tmpp->removeCtrlPoints(GeoDataRiverPathPoint::zposCenterToLeft, LeftRemoveIndices);
			tmpp = tmpp->nextPoint();
		}
	}
	if (RemoveCenterToRight) {
		GeoDataRiverPathPoint* tmpp = m_condition->m_riverSurvey->headPoint();
		if (tmpp != nullptr) {tmpp = tmpp->nextPoint();}
		while (tmpp != nullptr) {
			tmpp->removeCtrlPoints(GeoDataRiverPathPoint::zposCenterToRight, RightRemoveIndices);
			tmpp = tmpp->nextPoint();
		}
	}
	for (auto psetit = PointIndices.begin(); psetit != PointIndices.end(); ++psetit) {
		(psetit->first)->removeCtrlPoints(GeoDataRiverPathPoint::zposCenterLine, psetit->second);
	}

	// Save backup.
	if (RemoveCenterToLeft) {
		backup = new GeoDataRiverSurveyCtrlPointBackup();
		backup->backup(headPoint, GeoDataRiverPathPoint::zposCenterToLeft);
		m_after.push_back(backup);
	}
	if (RemoveCenterToRight) {
		backup = new GeoDataRiverSurveyCtrlPointBackup();
		backup->backup(headPoint, GeoDataRiverPathPoint::zposCenterToRight);
		m_after.push_back(backup);
	}
	for (auto pit = points.begin(); pit != points.end(); ++pit) {
		backup = new GeoDataRiverSurveyCtrlPointBackup();
		backup->backup(*pit, GeoDataRiverPathPoint::zposCenterLine);
		m_after.push_back(backup);
	}

	m_condition->m_selectedCtrlPointInfoList.clear();
}

void GridCreatingConditionRiverSurvey15D::DeleteCtrlPointCommand::apply(const std::vector<GeoDataRiverSurveyCtrlPointBackup*>& data)
{
	for (auto backup : data) {
		backup->restore();
	}
	m_condition->updateShapeData();
}
