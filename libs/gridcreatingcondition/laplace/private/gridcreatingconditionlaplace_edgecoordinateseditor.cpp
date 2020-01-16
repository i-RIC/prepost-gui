#include "gridcreatingconditionlaplace_edgecoordinateseditor.h"
#include "gridcreatingconditionlaplace_impl.h"
#include "../gridcreatingconditionlaplace.h"

#include <guibase/coordinates/coordinatesedit.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/iricundostack.h>

#include <QString>

bool GridCreatingConditionLaplace::EdgeCoordinatesEditor::streamWise = false;
int GridCreatingConditionLaplace::EdgeCoordinatesEditor::indexI = -1;
int GridCreatingConditionLaplace::EdgeCoordinatesEditor::indexJ = -1;
GridCreatingConditionLaplace* GridCreatingConditionLaplace::EdgeCoordinatesEditor::targetCondition = nullptr;
bool GridCreatingConditionLaplace::EdgeCoordinatesEditor::applied = false;

void GridCreatingConditionLaplace::EdgeCoordinatesEditor::edit(bool streamwise, int i, int j, GridCreatingConditionLaplace * condition)
{
	condition->impl->m_regionDefinedMouseEventMode = Impl::RegionDefinedMouseEventMode::EditCoodinatesDialog;
	condition->iricMainWindow()->enterModelessDialogMode();

	streamWise = streamwise;
	indexI = i;
	indexJ = j;
	targetCondition = condition;
	applied = false;

	std::vector<QPointF> coords;
	if (streamwise) {
		coords.push_back(condition->impl->ctrlPoint(i, j));
		for (auto p : condition->impl->edgeLineStreamWise(i, j).polyLine()) {
			coords.push_back(p);
		}
		coords.push_back(condition->impl->ctrlPoint(i + 1, j));
	} else {
		coords.push_back(condition->impl->ctrlPoint(i, j));
		for (auto p : condition->impl->edgeLineCrossSection(i, j).polyLine()) {
			coords.push_back(p);
		}
		coords.push_back(condition->impl->ctrlPoint(i, j + 1));
	}

	auto coords2 = applyOffset(coords);
	CoordinatesEdit::edit(coords2, condition->preProcessorWindow(),
												acceptCallback,
												rejectCallback,
												applyCallback,
												GridCreatingConditionLaplace::tr("Edge Coordinates"));
}

void GridCreatingConditionLaplace::EdgeCoordinatesEditor::acceptCallback(const std::vector<QPointF>& coords)
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
	}
	auto coords2 = unapplyOffset(coords);
	targetCondition->impl->pushEdgeEditCoordinatesCommand(streamWise, indexI, indexJ, coords2);

	targetCondition->impl->m_centerLineOnlyMouseEventMode = Impl::CenterLineOnlyMouseEventMode::Normal;
	targetCondition->iricMainWindow()->exitModelessDialogMode();
}

void GridCreatingConditionLaplace::EdgeCoordinatesEditor::rejectCallback()
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
		targetCondition->renderGraphicsView();
	}
	targetCondition->impl->m_regionDefinedMouseEventMode = Impl::RegionDefinedMouseEventMode::Normal;
	targetCondition->iricMainWindow()->exitModelessDialogMode();
}

void GridCreatingConditionLaplace::EdgeCoordinatesEditor::applyCallback(const std::vector<QPointF>& coords)
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
	}
	auto coords2 = unapplyOffset(coords);
	targetCondition->impl->pushEdgeEditCoordinatesCommand(streamWise, indexI, indexJ, coords2, true);

	applied = true;
}

std::vector<QPointF> GridCreatingConditionLaplace::EdgeCoordinatesEditor::applyOffset(const std::vector<QPointF>& coords)
{
	return CoordinatesEdit::applyOffset(coords, targetCondition->offset());
}

std::vector<QPointF> GridCreatingConditionLaplace::EdgeCoordinatesEditor::unapplyOffset(const std::vector<QPointF>& coords)
{
	return CoordinatesEdit::unapplyOffset(coords, targetCondition->offset());
}
