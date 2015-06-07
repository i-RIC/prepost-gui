#ifndef GRIDRELATEDCONDITIONINTEGERVARIATIONEDITWIDGET_H
#define GRIDRELATEDCONDITIONINTEGERVARIATIONEDITWIDGET_H

#include "../base/gridattributevariationeditwidgett.h"
class IntegerNumberEditWidget;

class GridAttributeIntegerVariationEditWidget : public GridAttributeVariationEditWidgetT<int>
{
public:
	GridAttributeIntegerVariationEditWidget(QWidget* parent, SolverDefinitionGridAttributeT<int>* cond);
	~GridAttributeIntegerVariationEditWidget();
	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;

protected:
	void setupWidget() override;
	void getValueFromInnerWidget() override;
	IntegerNumberEditWidget* m_widget;
};

#endif // GRIDRELATEDCONDITIONINTEGERVARIATIONEDITDIALOG_H
