#ifndef GRIDATTRIBUTEEDITWIDGETT_DETAIL_H
#define GRIDATTRIBUTEEDITWIDGETT_DETAIL_H

#include "../gridattributecontainert.h"
#include "../gridattributeeditcommand.h"
#include "../gridattributeeditwidgett.h"

#include <misc/iricundostack.h>

template <class V, class DA>
GridAttributeEditWidgetT<V, DA>::GridAttributeEditWidgetT(QWidget* parent, SolverDefinitionGridAttributeT<V>* cond) :
	GridAttributeEditWidget {parent, cond}
{}

template <class V, class DA>
V GridAttributeEditWidgetT<V, DA>::value() const
{
	getValueFromInnerWidget();
	return m_value;
}

template <class V, class DA>
void GridAttributeEditWidgetT<V, DA>::setValue(V value)
{
	m_value = value;
	setValueCleared(false);
	setValueSelected(true);
	setupWidget();
}


template <class V, class DA>
QVariant GridAttributeEditWidgetT<V, DA>::variantValue() const
{
	return QVariant(value());
}

template <class V, class DA>
void GridAttributeEditWidgetT<V, DA>::setVariantValue(const QVariant& v)
{
	SolverDefinitionGridAttributeT<V>* cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(gridAttribute());
	V tmpval = cond->fromVariant(v);
	setValue(tmpval);
}


template <class V, class DA>
void GridAttributeEditWidgetT<V, DA>::scanAndSetDefault(GridAttributeContainer* container, const std::vector<vtkIdType>& indices)
{
	auto c = dynamic_cast<GridAttributeContainerT<V, DA>* >(container);
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

template <class V, class DA>
void GridAttributeEditWidgetT<V, DA>::applyValue(GridAttributeContainer* container, const std::vector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemI* dItem)
{
	if (! isValueSelected()) {return;}

	auto c = dynamic_cast<GridAttributeContainerT<V, DA>*>(container);
	DA* oldValues = c->dataArrayCopy();
	V val = value();
	for (auto index : indices) {
		c->setValue(index, val);
	}
	vtkDataArray* newValues = c->dataArrayCopy();
	iRICUndoStack::instance().push(new GridAttributeEditCommand(c->dataArray()->GetName(), newValues, oldValues, atts, dItem));
	oldValues->Delete();
	newValues->Delete();
}

#endif // GRIDATTRIBUTEEDITWIDGETT_DETAIL_H
