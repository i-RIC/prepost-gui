#ifndef GRIDRELATEDCONDITIONEDITWIDGETT_H
#define GRIDRELATEDCONDITIONEDITWIDGETT_H

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
		m_dataItem->informgridRelatedConditionChange(m_name);
		m_dataItem->grid()->setModified();
		m_dataItem->grid()->gridRelatedCondition(m_name)->setCustomModified(modified);
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
	GridAttributeEditWidgetT(QWidget* parent, SolverDefinitionGridAttributeT<V>* cond) :
		GridAttributeEditWidget {parent, cond}
	{}
	void setValue(V value) {
		m_value = value;
		m_valueCleared = false;
		m_valueSelected = true;
		setupWidget();
	}
	V value() {
		getValueFromInnerWidget();
		return m_value;
	}
	void setVariantValue(const QVariant& v) override {
		SolverDefinitionGridAttributeT<V>* cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(m_gridRelatedCondition);
		V tmpval = cond->fromVariant(v);
		setValue(tmpval);
	}

	QVariant variantValue() override {
		return QVariant(value());
	}
	void scanAndSetDefault(GridAttributeContainer* container, QVector<vtkIdType>& indices) override {
		GridAttributeContainerT<V>* c = dynamic_cast<GridAttributeContainerT<V>* >(container);
		bool same = true;
		V val;
		auto it = indices.begin();
		if (it == indices.end()) {
			// no point is selected!
			clearValue();
			return;
		}
		val = c->value(*it);
		++it;
		while (same && it != indices.end()) {
			same &= (val == c->value(*it));
			++it;
		}
		if (same) {
			setValue(val);
		} else {
			clearValue();
		}
	}

	void applyValue(GridAttributeContainer* container, QVector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem) override {
		if (! m_valueSelected) {return;}
		GridAttributeContainerT<V>* c = dynamic_cast<GridAttributeContainerT<V>* >(container);
		vtkDataArray* oldValues = c->dataArrayCopy();
		V val = value();
		for (auto it = indices.begin(); it != indices.end(); ++it) {
			c->setValue(*it, val);
		}
		vtkDataArray* newValues = c->dataArrayCopy();
		iRICUndoStack::instance().push(new GridAttributeEditCommand(c->dataArray()->GetName(), newValues, oldValues, atts, dItem));
		oldValues->Delete();
		newValues->Delete();
	}

protected:
	V m_value {0};
};

#endif // GRIDRELATEDCONDITIONEDITWIDGETT_H
