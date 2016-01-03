#ifndef GRIDATTRIBUTEREALEDITWIDGET_H
#define GRIDATTRIBUTEREALEDITWIDGET_H

#include "../base/gridattributeeditwidgett.h"

class RealNumberEditWidget;

class GridAttributeRealEditWidget : public GridAttributeEditWidgetT<double>
{
public:
	GridAttributeRealEditWidget(QWidget* parent, SolverDefinitionGridAttributeT<double>* cond);
	~GridAttributeRealEditWidget();

	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;

private:
	void setupWidget() override;
	void getValueFromInnerWidget() const override;

	RealNumberEditWidget* m_widget;
};

#endif // GRIDATTRIBUTEREALEDITWIDGET_H
