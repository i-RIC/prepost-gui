#ifndef GRIDATTRIBUTEVARIATIONEDITWIDGETT_DETAIL_H
#define GRIDATTRIBUTEVARIATIONEDITWIDGETT_DETAIL_H

#include "../gridattributecontainert.h"
#include "../gridattributeeditvariationcommand.h"
#include "../gridattributevariationeditwidgett.h"

#include <misc/iricundostack.h>

template <class V>
GridAttributeVariationEditWidgetT<V>::GridAttributeVariationEditWidgetT(QWidget* parent, SolverDefinitionGridAttributeT<V>* cond) :
	GridAttributeVariationEditWidget {parent, cond},
	m_value {0}
{}

template <class V>
void GridAttributeVariationEditWidgetT<V>::setValue(V value)
{
	m_value = value;
	setupWidget();
}

template <class V>
V GridAttributeVariationEditWidgetT<V>::value()
{
	getValueFromInnerWidget();
	return m_value;
}

template <class V>
void GridAttributeVariationEditWidgetT<V>::setVariantValue(const QVariant& v)
{
	SolverDefinitionGridAttributeT<V>* cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(m_gridAttribute);
	V tmpval = cond->fromVariant(v);
	setValue(tmpval);
}

template <class V>
QVariant GridAttributeVariationEditWidgetT<V>::variantValue()
{
	return QVariant(value());
}

template <class V>
void GridAttributeVariationEditWidgetT<V>::applyVariation(GridAttributeContainer* container, QVector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem)
{
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

#endif // GRIDATTRIBUTEVARIATIONEDITWIDGETT_DETAIL_H
