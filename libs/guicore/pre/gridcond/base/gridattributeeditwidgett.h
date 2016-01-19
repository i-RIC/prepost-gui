#ifndef GRIDATTRIBUTEEDITWIDGETT_H
#define GRIDATTRIBUTEEDITWIDGETT_H

#include "gridattributeeditwidget.h"
#include "../../../solverdef/solverdefinitiongridattributet.h"

/// Widget to Edit Grid related condition value

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
