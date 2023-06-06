#include "../preprocessorgriddataitem.h"
#include "preprocessorgridshapedataitem_updatepointscommand.h"

#include <guicore/pre/grid/grid.h>
#include <misc/qundocommandhelper.h>

PreProcessorGridShapeDataItem::UpdatePointsCommand::UpdatePointsCommand(bool finish, vtkPoints* newPoints, PreProcessorGridDataItem* item) :
	QUndoCommand("Update points"),
	m_finish {finish},
	m_newPoints {newPoints},
	m_oldPoints {item->grid()->vtkGrid()->GetPoints()},
	m_item {item}
{
	m_newPoints->Register(nullptr);
	m_oldPoints->Register(nullptr);
}

PreProcessorGridShapeDataItem::UpdatePointsCommand::~UpdatePointsCommand()
{
	m_newPoints->Delete();
	m_oldPoints->Delete();
}

void PreProcessorGridShapeDataItem::UpdatePointsCommand::redo()
{
	setPoints(m_newPoints, m_finish);
}

void PreProcessorGridShapeDataItem::UpdatePointsCommand::undo()
{
	setPoints(m_oldPoints, true);
}

int PreProcessorGridShapeDataItem::UpdatePointsCommand::id() const
{
	return iRIC::generateCommandId("PreProcessorGridShapeDataItem::UpdatePointsCommand");
}

bool PreProcessorGridShapeDataItem::UpdatePointsCommand::mergeWith(const QUndoCommand* other)
{
	if (m_finish) {
		return false;
	}

	const auto other2 = dynamic_cast<const UpdatePointsCommand*>(other);
	if (other2 == nullptr) {return false;}
	if (other2->m_item != m_item) {return false;}

	m_newPoints->Delete();

	m_newPoints = other2->m_newPoints;
	m_newPoints->Register(nullptr);
	return true;
}

void PreProcessorGridShapeDataItem::UpdatePointsCommand::setPoints(vtkPoints* points, bool buildLocator)
{
	m_item->grid()->setPoints(points);
	if (buildLocator) {
		m_item->grid()->pointLocator()->BuildLocator();
	}
	m_item->updateVtkObjectsForDrawing();
}
