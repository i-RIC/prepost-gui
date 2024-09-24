#ifndef GRIDATTRIBUTEEDITCOMMAND_H
#define GRIDATTRIBUTEEDITCOMMAND_H

#include "../../../guicore_global.h"

#include <QUndoCommand>

#include <vtkAbstractArray.h>
#include <vtkDataSetAttributes.h>
#include <vtkSmartPointer.h>

#include <string>

class PreProcessorGridDataItemI;

class GUICOREDLL_EXPORT GridAttributeEditCommand :  public QUndoCommand
{
public:
	GridAttributeEditCommand(const std::string& name, vtkAbstractArray* newValues, vtkAbstractArray* oldValues, vtkDataSetAttributes* atts, PreProcessorGridDataItemI* dItem);

	void redo() override;
	void undo() override;

private:
	void copyValues(vtkAbstractArray* data, bool modified);

	vtkSmartPointer<vtkAbstractArray> m_newValues;
	vtkSmartPointer<vtkAbstractArray> m_oldValues;

	bool m_oldCustomModified;
	vtkSmartPointer<vtkDataSetAttributes> m_attributes;
	std::string m_name;
	PreProcessorGridDataItemI* m_dataItem;
};

#endif // GRIDATTRIBUTEEDITCOMMAND_H
