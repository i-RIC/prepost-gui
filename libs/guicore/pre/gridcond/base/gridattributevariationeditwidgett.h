#ifndef GRIDATTRIBUTEVARIATIONEDITWIDGETT_H
#define GRIDATTRIBUTEVARIATIONEDITWIDGETT_H

#include "gridattributevariationeditwidget.h"
#include "gridattributecontainert.h"
#include "../../../solverdef/solverdefinitiongridattributet.h"
#include "../../grid/grid.h"
#include "../../base/preprocessorgriddataiteminterface.h"
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <QObject>
#include <QUndoCommand>
#include <vtkDataArray.h>
#include <vtkDataSetAttributes.h>
#include <vtkSmartPointer.h>

/// Widget to Edit Grid related condition value

class GridAttributeEditVariationCommand : public QUndoCommand
{

public:
	GridAttributeEditVariationCommand(const std::string& name, vtkDataArray* newValues, vtkDataArray* oldValues, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem) :
		QUndoCommand {GridAttributeVariationEditWidget::tr("Edit grid attribute value(s)")},
		m_newValues {newValues},
		m_oldValues {oldValues},
		m_oldCustomModified {false},
		m_attributes {atts},
		m_name (name),
		m_dataItem {dItem}
	{
		m_oldCustomModified = m_dataItem->grid()->gridAttribute(m_name)->isCustomModified();
		m_oldValues->SetName(m_name.c_str());
		m_newValues->SetName(m_name.c_str());
	}
	void redo() override {
		copyData(m_oldValues, true);
	}
	void undo() override {
		copyData(m_oldValues, m_oldCustomModified);
	}

private:
	void copyData(vtkDataArray* data, bool modified) {
		m_attributes->GetArray(m_name.c_str())->DeepCopy(data);
		m_dataItem->updateAttributeActorSettings();
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
class GridAttributeVariationEditWidgetT : public GridAttributeVariationEditWidget
{

public:
	GridAttributeVariationEditWidgetT(QWidget* parent, SolverDefinitionGridAttributeT<V>* cond) :
		GridAttributeVariationEditWidget {parent, cond},
		m_value {0}
	{}
	void setValue(V value) {
		m_value = value;
		setupWidget();
	}
	V value() {
		getValueFromInnerWidget();
		return m_value;
	}
	void setVariantValue(const QVariant& v) {
		SolverDefinitionGridAttributeT<V>* cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(m_gridAttribute);
		V tmpval = cond->fromVariant(v);
		setValue(tmpval);
	}

	QVariant variantValue() override {
		return QVariant(value());
	}

	void applyVariation(GridAttributeContainer* container, QVector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem) override {
		GridAttributeContainerT<V>* c = dynamic_cast<GridAttributeContainerT<V>* >(container);
		vtkDataArray* oldValues = c->dataArrayCopy();
		V val = value();
		for (vtkIdType id : indices) {
			c->setValue(id, c->value(id) + val);
		}
		vtkDataArray* newValues = c->dataArrayCopy();
		iRICUndoStack::instance().push(new GridAttributeEditVariationCommand(c->dataArray()->GetName(), newValues, oldValues, atts, dItem));
		oldValues->Delete();
		newValues->Delete();
	}

protected:
	V m_value {0};
};

#endif // GRIDATTRIBUTEEDITWIDGETT_H
