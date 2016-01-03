#ifndef GRIDATTRIBUTEINTEGEREDITWIDGET_H
#define GRIDATTRIBUTEINTEGEREDITWIDGET_H

#include "../base/gridattributeeditwidgett.h"
class IntegerNumberEditWidget;

class GridAttributeIntegerEditWidget : public GridAttributeEditWidgetT<int>
{

public:
	GridAttributeIntegerEditWidget(QWidget* parent, SolverDefinitionGridAttributeT<int>* cond);
	~GridAttributeIntegerEditWidget();

private:
	void setupWidget() override;
	void getValueFromInnerWidget() const override;

	QWidget* editWidget() const override;

	IntegerNumberEditWidget* m_widget;
};

#endif // GRIDATTRIBUTEINTEGEREDITDIALOG_H
