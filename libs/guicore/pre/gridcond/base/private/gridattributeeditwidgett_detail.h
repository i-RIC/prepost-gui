#ifndef GRIDATTRIBUTEEDITWIDGETT_DETAIL_H
#define GRIDATTRIBUTEEDITWIDGETT_DETAIL_H

#include "../gridattributeeditwidgett.h"

template <class V>
GridAttributeEditWidgetT<V>::GridAttributeEditWidgetT(QWidget* parent, SolverDefinitionGridAttributeT<V>* cond) :
	GridAttributeEditWidget {parent, cond}
{}

template <class V>
V GridAttributeEditWidgetT<V>::value() const
{
	getValueFromInnerWidget();
	return m_value;
}

template <class V>
void GridAttributeEditWidgetT<V>::setValue(V value)
{
	m_value = value;
	setValueCleared(false);
	setValueSelected(true);
	setupWidget();
}


template <class V>
QVariant GridAttributeEditWidgetT<V>::variantValue() const
{
	return QVariant(value());
}

template <class V>
void GridAttributeEditWidgetT<V>::setVariantValue(const QVariant& v)
{
	SolverDefinitionGridAttributeT<V>* cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(gridAttribute());
	V tmpval = cond->fromVariant(v);
	setValue(tmpval);
}


template <class V>
void GridAttributeEditWidgetT<V>::scanAndSetDefault(GridAttributeContainer* container, QVector<vtkIdType>& indices)
{
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

template <class V>
void GridAttributeEditWidgetT<V>::applyValue(GridAttributeContainer* container, QVector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem)
{
	if (! isValueSelected()) {return;}

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

#endif // GRIDATTRIBUTEEDITWIDGETT_DETAIL_H
