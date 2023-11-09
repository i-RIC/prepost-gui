#ifndef GRIDATTRIBUTEVARIATIONEDITWIDGETT_DETAIL_H
#define GRIDATTRIBUTEVARIATIONEDITWIDGETT_DETAIL_H

#include "../gridattributecontainert.h"
#include "../gridattributeeditvariationcommand.h"
#include "../gridattributevariationeditwidgett.h"

#include <misc/iricundostack.h>

template <class V, class DA>
GridAttributeVariationEditWidgetT<V, DA>::GridAttributeVariationEditWidgetT(QWidget* parent, SolverDefinitionGridAttributeT<V>* cond) :
	GridAttributeVariationEditWidget {parent, cond},
	m_value {0}
{}

template <class V, class DA>
void GridAttributeVariationEditWidgetT<V, DA>::applyVariation(GridAttributeContainer* container, const std::vector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemI* dItem)
{
	getValueFromInnerWidget();

	auto c = dynamic_cast<GridAttributeContainerT<V, DA>* >(container);
	DA* oldValues = c->dataArrayCopy();
	V val = m_value;
	for (vtkIdType id : indices) {
		if (m_mode == Difference) {
			c->setValue(id, c->value(id) + val);
		} else if (m_mode == Ratio) {
			c->setValue(id, c->value(id) * val);
		}
	}
	DA* newValues = c->dataArrayCopy();
	iRICUndoStack::instance().push(new GridAttributeEditVariationCommand(c->dataArray()->GetName(), newValues, oldValues, atts, dItem));
	oldValues->Delete();
	newValues->Delete();
}

#endif // GRIDATTRIBUTEVARIATIONEDITWIDGETT_DETAIL_H
