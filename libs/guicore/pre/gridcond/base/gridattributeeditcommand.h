#ifndef GRIDATTRIBUTEEDITCOMMAND_H
#define GRIDATTRIBUTEEDITCOMMAND_H

#include <QUndoCommand>

#include <vtkDataArray.h>
#include <vtkDataSetAttributes.h>
#include <vtkSmartPointer.h>

#include <string>

class PreProcessorGridDataItemInterface;

class GridAttributeEditCommand :  public QUndoCommand
{
public:
	GridAttributeEditCommand(const std::string& name, vtkDataArray* newValues, vtkDataArray* oldValues, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem);

	void redo() override;
	void undo() override;

private:
	void copyValues(vtkDataArray* data, bool modified);

	vtkSmartPointer<vtkDataArray> m_newValues;
	vtkSmartPointer<vtkDataArray> m_oldValues;

	bool m_oldCustomModified;
	vtkSmartPointer<vtkDataSetAttributes> m_attributes;
	std::string m_name;
	PreProcessorGridDataItemInterface* m_dataItem;
};

#endif // GRIDATTRIBUTEEDITCOMMAND_H
