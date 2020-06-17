#ifndef GRIDATTRIBUTEINTEGERVARIATIONEDITWIDGET_H
#define GRIDATTRIBUTEINTEGERVARIATIONEDITWIDGET_H

#include "../base/gridattributevariationeditwidgett.h"
class IntegerNumberEditWidget;

class GridAttributeIntegerVariationEditWidget : public GridAttributeVariationEditWidgetT<int>
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
