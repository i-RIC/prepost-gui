#ifndef GRIDRELATEDCONDITIONREALEDITWIDGET_H
#define GRIDRELATEDCONDITIONREALEDITWIDGET_H

#include "../base/gridattributeeditwidgett.h"

class RealNumberEditWidget;

class GridAttributeRealEditWidget : public GridAttributeEditWidgetT<double>
{

public:
	GridAttributeRealEditWidget(QWidget* parent, SolverDefinitionGridAttributeT<double>* cond);
	~GridAttributeRealEditWidget();
	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;

protected:
	void setupWidget() override;
	void getValueFromInnerWidget() override;
	RealNumberEditWidget* m_widget;
};

#endif // GRIDRELATEDCONDITIONREALEDITWIDGET_H
