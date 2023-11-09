#ifndef GRIDATTRIBUTEEDITWIDGETT_H
#define GRIDATTRIBUTEEDITWIDGETT_H

#include "gridattributeeditwidget.h"
#include "../../../solverdef/solverdefinitiongridattributet.h"

/// Widget to Edit Grid related condition value

template <class V, class DA>
class GridAttributeEditWidgetT : public GridAttributeEditWidget
{
public:
	GridAttributeEditWidgetT(QWidget* parent, SolverDefinitionGridAttributeT<V>* cond);

	V value() const;
	void setValue(V value);

	QVariant variantValue() const override;
	void setVariantValue(const QVariant& v) override;

	void scanAndSetDefault(GridAttributeContainer* container, const std::vector<vtkIdType>& indices) override;
	void applyValue(GridAttributeContainer* container, const std::vector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemI* dItem) override;

protected:
	mutable V m_value {0};
};

#include "private/gridattributeeditwidgett_detail.h"

#endif // GRIDATTRIBUTEEDITWIDGETT_H
