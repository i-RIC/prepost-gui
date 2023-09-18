#include "../preprocessorgriddataitem.h"
#include "preprocessorgridshapedataitem_updatepointscommand.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guicore/grid/v4grid2d.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <misc/qundocommandhelper.h>

PreProcessorGridShapeDataItem::UpdatePointsCommand::UpdatePointsCommand(bool finish, vtkPoints* newPoints, PreProcessorGridDataItem* item) :
	QUndoCommand("Update points"),
	m_finish {finish},
	m_newPoints {newPoints},
	m_oldPoints {vtkPoints::New()},
	m_item {item}
{
	m_newPoints->Register(nullptr);
	m_oldPoints->DeepCopy(item->grid()->grid()->vtkData()->data()->GetPoints());
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
	m_item->grid()->grid()->vtkData()->data()->GetPoints()->DeepCopy(points);

	if (buildLocator) {
		m_item->grid()->grid()->pointsModified();
	}
	m_item->updateVtkObjectsForDrawing();
}
