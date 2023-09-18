#ifndef GRIDATTRIBUTEINTEGERVARIATIONEDITWIDGET_H
#define GRIDATTRIBUTEINTEGERVARIATIONEDITWIDGET_H

#include "../base/gridattributevariationeditwidgett.h"

#include <vtkIntArray.h>

class IntegerNumberEditWidget;

class GridAttributeIntegerVariationEditWidget : public GridAttributeVariationEditWidgetT<int, vtkIntArray>
{
public:
	GridAttributeIntegerVariationEditWidget(QWidget* parent, SolverDefinitionGridAttributeT<int>* cond);
	~GridAttributeIntegerVariationEditWidget();

	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;

private:
	void getValueFromInnerWidget() override;

	IntegerNumberEditWidget* m_widget;
};

#endif // GRIDATTRIBUTEINTEGERVARIATIONEDITDIALOG_H
