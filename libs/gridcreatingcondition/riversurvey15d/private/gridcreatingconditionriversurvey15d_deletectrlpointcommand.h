#ifndef GRIDCREATINGCONDITIONRIVERSURVEY15D_DELETECTRLPOINTCOMMAND_H
#define GRIDCREATINGCONDITIONRIVERSURVEY15D_DELETECTRLPOINTCOMMAND_H

#include "../gridcreatingconditionriversurvey15d.h"

#include <vector>

#include <QUndoCommand>

class GeoDataRiverSurveyCtrlPointBackup;

class GridCreatingConditionRiverSurvey15D::DeleteCtrlPointCommand : public QUndoCommand
{
public:
	DeleteCtrlPointCommand(GridCreatingConditionRiverSurvey15D* cond);

	void redo() override;
	void undo() override;

private:
	void executeDeleteCtrlPoints();
	void apply(const std::vector<GeoDataRiverSurveyCtrlPointBackup*>& data);

	GridCreatingConditionRiverSurvey15D* m_condition;
	std::vector<GeoDataRiverSurveyCtrlPointBackup*> m_before;
	std::vector<GeoDataRiverSurveyCtrlPointBackup*> m_after;
};

#endif // GRIDCREATINGCONDITIONRIVERSURVEY15D_DELETECTRLPOINTCOMMAND_H
