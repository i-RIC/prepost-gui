#ifndef GRIDCREATINGCONDITIONRIVERSURVEYINTERFACE_H
#define GRIDCREATINGCONDITIONRIVERSURVEYINTERFACE_H

#include "gd_riversurvey_global.h"
#include "geodatariverpathpoint.h"

#include <guicore/pre/gridcreatingcondition/gridcreatingcondition.h>
#include <guicore/misc/qundocommandhelper.h>

#include <QUndoCommand>

#include <list>

class GridCreatingConditionCtrlPointMoveCommand;

class GD_RIVERSURVEY_EXPORT GridCreatingConditionRiverSurveyInterface : public GridCreatingCondition
{
	Q_OBJECT

public:
	struct GridCtrlOffsetInfo {
		QVector2D direction;
		double length;
		double lowerLimit;
		double upperLimit;
		QVector<double> points;
	};
	GridCreatingConditionRiverSurveyInterface(ProjectDataItem* parent, GridCreatingConditionCreator* creator)
		: GridCreatingCondition(parent, creator) {
	}
	virtual ~GridCreatingConditionRiverSurveyInterface() {}
	virtual GridCtrlOffsetInfo& GCPOffsetInfo() = 0;
	virtual std::list<CtrlPointSelectionInfo>& selectedCtrlPointInfoList() = 0;
	virtual void updateShapeData() = 0;
	virtual void cancelBackgroundGridUpdate() = 0;
	virtual void informCtrlPointUpdateToCrosssectionWindows() = 0;
	virtual void invalidateSelectedCtrlPoints() = 0;

protected:
	std::list<CtrlPointSelectionInfo> m_selectedCtrlPointInfoList;
	GridCtrlOffsetInfo m_GCPOffsetInfo;

public:
	friend class GridCreatingConditionCtrlPointMoveCommand;
};

class GridCreatingConditionCtrlPointMoveCommand : public QUndoCommand
{

public:
	GridCreatingConditionCtrlPointMoveCommand(
		bool apply,
		double offset,
		GridCreatingConditionRiverSurveyInterface* cond, bool noMerge = false)
		: QUndoCommand(GridCreatingConditionRiverSurveyInterface::tr("Move Control Points")) {
		m_apply = apply;
		m_offset = offset;
		m_condition = cond;
		std::list<CtrlPointSelectionInfo> infoList = m_condition->m_selectedCtrlPointInfoList;
		CtrlPointSelectionInfo Info = infoList.front();
		m_point = Info.Point;
		m_position = Info.Position;
		m_before = m_condition->m_GCPOffsetInfo.points;
		m_noMerge = noMerge;

		updateAfter();
	}

	void undo() override {
		m_condition->cancelBackgroundGridUpdate();
		m_point->CtrlPoints(m_position) = m_before;
		m_condition->invalidateSelectedCtrlPoints();
		if (! m_apply) {
			m_condition->informCtrlPointUpdateToCrosssectionWindows();
			m_condition->updateShapeData();
			m_condition->renderGraphicsView();
		}
	}

	void redo() override {
		m_condition->cancelBackgroundGridUpdate();
		m_point->CtrlPoints(m_position) = m_after;
		if (! m_apply) {
			m_condition->updateShapeData();
			m_condition->renderGraphicsView();
		} else {
			m_condition->informCtrlPointUpdateToCrosssectionWindows();
		}
		if (m_noMerge) {
			m_condition->invalidateSelectedCtrlPoints();
		}
	}

	int id() const override {
		return iRIC::generateCommandId("GridCreatingConditionCtrlPointMove");
	}
	bool mergeWith(const QUndoCommand* other) override {
		const GridCreatingConditionCtrlPointMoveCommand* other2 = dynamic_cast<const GridCreatingConditionCtrlPointMoveCommand*>(other);
		if (other2 == 0) {return false;}
		if (m_condition != other2->m_condition) {return false;}
		if (m_point != other2->m_point) {return false;}
		if (m_position != other2->m_position) {return false;}
		if (m_noMerge) {return false;}
		m_offset = other2->m_offset;
		m_apply = other2->m_apply;
		m_noMerge = other2->m_noMerge;

		updateAfter();
		return true;
	}

private:
	void updateAfter() {
		std::list<CtrlPointSelectionInfo> infoList = m_condition->m_selectedCtrlPointInfoList;
		m_after = m_before;

		int minindex = infoList.front().Index;
		int maxindex = infoList.back().Index;

		for (int i = minindex; i <= maxindex; ++i) {
			m_after[i] += m_offset;
		}
	}

private:
	bool m_apply;
	double m_offset;
	GridCreatingConditionRiverSurveyInterface* m_condition;
	GeoDataRiverPathPoint* m_point;
	GeoDataRiverPathPoint::CtrlPointPosition m_position;
	QVector<double> m_before;
	QVector<double> m_after;
	bool m_noMerge;
};

#endif // GRIDCREATINGCONDITIONRIVERSURVEYINTERFACE_H
