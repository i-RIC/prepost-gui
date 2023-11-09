#ifndef GRIDATTRIBUTEREALEDITWIDGET_H
#define GRIDATTRIBUTEREALEDITWIDGET_H

#include "../base/gridattributeeditwidgett.h"

#include <vtkDoubleArray.h>

class RealNumberEditWidget;

class GridAttributeRealEditWidget : public GridAttributeEditWidgetT<double, vtkDoubleArray>
{
public:
	GridAttributeRealEditWidget(QWidget* parent, SolverDefinitionGridAttributeT<double>* cond);
	~GridAttributeRealEditWidget();

private:
	void setupWidget() override;
	void getValueFromInnerWidget() const override;

	QWidget* editWidget() const override;

	RealNumberEditWidget* m_widget;
};

#endif // GRIDATTRIBUTEREALEDITWIDGET_H
