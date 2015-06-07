#ifndef GRIDRELATEDCONDITIONREALVARIATIONEDITWIDGET_H
#define GRIDRELATEDCONDITIONREALVARIATIONEDITWIDGET_H

#include "../base/gridattributevariationeditwidgett.h"

class RealNumberEditWidget;

class GridAttributeRealVariationEditWidget : public GridAttributeVariationEditWidgetT<double>
{
public:
	GridAttributeRealVariationEditWidget(QWidget* parent, SolverDefinitionGridAttributeT<double>* cond);
	~GridAttributeRealVariationEditWidget();
	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;
protected:
	void setupWidget() override;
	void getValueFromInnerWidget() override;
	RealNumberEditWidget* m_widget;
};

#endif // GRIDRELATEDCONDITIONREALVARIATIONEDITWIDGET_H
