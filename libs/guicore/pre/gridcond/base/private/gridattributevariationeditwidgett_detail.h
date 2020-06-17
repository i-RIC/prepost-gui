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
void GridAttributeVariationEditWidgetT<V>::applyVariation(GridAttributeContainer* container, QVector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem)
{
	getValueFromInnerWidget();

	GridAttributeContainerT<V>* c = dynamic_cast<GridAttributeContainerT<V>* >(container);
	vtkDataArray* oldValues = c->dataArrayCopy();
	V val = m_value;
	for (vtkIdType id : indices) {
		if (m_mode == Difference) {
			c->setValue(id, c->value(id) + val);
		} else if (m_mode == Ratio) {
			c->setValue(id, c->value(id) * val);
		}
	}
	vtkDataArray* newValues = c->dataArrayCopy();
	iRICUndoStack::instance().push(new GridAttributeEditVariationCommand(c->dataArray()->GetName(), newValues, oldValues, atts, dItem));
	oldValues->Delete();
	newValues->Delete();
}

#endif // GRIDATTRIBUTEVARIATIONEDITWIDGETT_DETAIL_H
