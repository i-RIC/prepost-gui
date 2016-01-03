#ifndef GRIDATTRIBUTEEDITWIDGETT_H
#define GRIDATTRIBUTEEDITWIDGETT_H

#include "gridattributeeditwidget.h"
#include "gridattributecontainert.h"
#include "../../../solverdef/solverdefinitiongridattributet.h"
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include "../../base/preprocessorgriddataiteminterface.h"
#include <QObject>
#include <QUndoCommand>
#include <vtkDataArray.h>
#include <vtkDataSetAttributes.h>
#include <vtkSmartPointer.h>

/// Widget to Edit Grid related condition value

class GridAttributeEditCommand :  public QUndoCommand
{

public:
	GridAttributeEditCommand(const std::string& name, vtkDataArray* newValues, vtkDataArray* oldValues, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem) :
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
	void redo() override {
		copyValues(m_newValues, true);
	}
	void undo() override {
		copyValues(m_newValues, m_oldCustomModified);
	}

private:
	void copyValues(vtkDataArray* data, bool modified) {
		m_attributes->GetArray(m_name.c_str())->DeepCopy(data);
		m_dataItem->updateSimplifiedGrid();
		m_dataItem->informGridAttributeChange(m_name);
		m_dataItem->grid()->setModified();
		m_dataItem->grid()->gridAttribute(m_name)->setCustomModified(modified);
	}
	vtkSmartPointer<vtkDataArray> m_newValues;
	vtkSmartPointer<vtkDataArray> m_oldValues;
	bool m_oldCustomModified;
	vtkSmartPointer<vtkDataSetAttributes> m_attributes;
	std::string m_name;
	PreProcessorGridDataItemInterface* m_dataItem;
};

template <class V>
class GridAttributeEditWidgetT : public GridAttributeEditWidget
{
public:
	GridAttributeEditWidgetT(QWidget* parent, SolverDefinitionGridAttributeT<V>* cond);

	V value() const;
	void setValue(V value);

	QVariant variantValue() const override;
	void setVariantValue(const QVariant& v) override;

	void scanAndSetDefault(GridAttributeContainer* container, QVector<vtkIdType>& indices) override;
	void applyValue(GridAttributeContainer* container, QVector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem) override;

protected:
	mutable V m_value {0};
};

#include "private/gridattributeeditwidgett_detail.h"

#endif // GRIDATTRIBUTEEDITWIDGETT_H
