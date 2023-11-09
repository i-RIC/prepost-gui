#ifndef GRIDATTRIBUTEEDITVARIATIONCOMMAND_H
#define GRIDATTRIBUTEEDITVARIATIONCOMMAND_H

#include <QUndoCommand>

#include <vtkDataArray.h>
#include <vtkDataSetAttributes.h>
#include <vtkSmartPointer.h>

#include <string>

class PreProcessorGridDataItemI;

class GridAttributeEditVariationCommand : public QUndoCommand
{
public:
	GridAttributeEditVariationCommand(const std::string& name, vtkDataArray* newValues, vtkDataArray* oldValues, vtkDataSetAttributes* atts, PreProcessorGridDataItemI* dItem);

	void redo() override;
	void undo() override;

private:
	void copyValues(vtkDataArray* data, bool modified);

	vtkSmartPointer<vtkDataArray> m_newValues;
	vtkSmartPointer<vtkDataArray> m_oldValues;

	bool m_oldCustomModified;
	vtkSmartPointer<vtkDataSetAttributes> m_attributes;
	std::string m_name;
	PreProcessorGridDataItemI* m_dataItem;
};

#endif // GRIDATTRIBUTEEDITVARIATIONCOMMAND_H
