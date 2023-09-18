#include "gridattributecontainer.h"
#include "gridattributeeditvariationcommand.h"
#include "gridattributevariationeditwidget.h"
#include "../../base/preprocessorgriddataitemi.h"

GridAttributeEditVariationCommand::GridAttributeEditVariationCommand(const std::string& name, vtkDataArray* newValues, vtkDataArray* oldValues, vtkDataSetAttributes* atts, PreProcessorGridDataItemI* dItem) :
	QUndoCommand {GridAttributeVariationEditWidget::tr("Edit grid attribute value(s)")},
	m_newValues {newValues},
	m_oldValues {oldValues},
	m_oldCustomModified {false},
	m_attributes {atts},
	m_name (name),
	m_dataItem {dItem}
{
	m_oldCustomModified = m_dataItem->grid()->attribute(m_name)->isCustomModified();
	m_oldValues->SetName(m_name.c_str());
	m_newValues->SetName(m_name.c_str());
}

void GridAttributeEditVariationCommand::redo()
{
	copyValues(m_newValues, true);
}

void GridAttributeEditVariationCommand::undo()
{
	copyValues(m_oldValues, m_oldCustomModified);
}

void GridAttributeEditVariationCommand::copyValues(vtkDataArray* data, bool modified)
{
	m_attributes->GetArray(m_name.c_str())->DeepCopy(data);
	m_dataItem->updateSimplifiedGrid();
	m_dataItem->informGridAttributeChange(m_name);
	m_dataItem->grid()->setIsModified(true);
	m_dataItem->grid()->attribute(m_name)->setCustomModified(modified);
}
