#include "gridattributecontainer.h"
#include "gridattributeeditcommand.h"
#include "gridattributeeditwidget.h"
#include "../../base/preprocessorgriddataitemi.h"

GridAttributeEditCommand::GridAttributeEditCommand(const std::string& name, vtkDataArray* newValues, vtkDataArray* oldValues, vtkDataSetAttributes* atts, PreProcessorGridDataItemI* dItem) :
	QUndoCommand(GridAttributeEditWidget::tr("Edit grid attribute value(s)")),
	m_newValues {newValues},
	m_oldValues {oldValues},
	m_oldCustomModified {false},
	m_attributes {atts},
	m_name (name),
	m_dataItem {dItem}
{
	m_oldValues->SetName(m_name.c_str());
	m_newValues->SetName(m_name.c_str());
}

void GridAttributeEditCommand::redo()
{
	copyValues(m_newValues, true);
}

void GridAttributeEditCommand::undo()
{
	copyValues(m_oldValues, m_oldCustomModified);
}

void GridAttributeEditCommand::copyValues(vtkDataArray* data, bool modified)
{
	m_attributes->GetArray(m_name.c_str())->DeepCopy(data);
	m_dataItem->updateSimplifiedGrid();
	m_dataItem->informGridAttributeChange(m_name);
	m_dataItem->grid()->setIsModified(true);
	m_dataItem->grid()->attribute(m_name)->setCustomModified(modified);
}
