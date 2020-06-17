#ifndef GRIDATTRIBUTEREALVARIATIONEDITWIDGET_H
#define GRIDATTRIBUTEREALVARIATIONEDITWIDGET_H

#include "../base/gridattributevariationeditwidgett.h"

class RealNumberEditWidget;

class GridAttributeRealVariationEditWidget : public GridAttributeVariationEditWidgetT<double>
{
public:
	GridAttributeRealVariationEditWidget(QWidget* parent, SolverDefinitionGridAttributeT<double>* cond);
	~GridAttributeRealVariationEditWidget();

	void setMode(Mode mode) override;
	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;

private:
	void getValueFromInnerWidget() override;

	RealNumberEditWidget* m_widget;
};

#endif // GRIDATTRIBUTEREALVARIATIONEDITWIDGET_H
