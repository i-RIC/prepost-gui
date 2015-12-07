#ifndef GRIDATTRIBUTEINTEGEREDITWIDGET_H
#define GRIDATTRIBUTEINTEGEREDITWIDGET_H

#include "../base/gridattributeeditwidgett.h"
class IntegerNumberEditWidget;

class GridAttributeIntegerEditWidget : public GridAttributeEditWidgetT<int>
{

public:
	GridAttributeIntegerEditWidget(QWidget* parent, SolverDefinitionGridAttributeT<int>* cond);
	~GridAttributeIntegerEditWidget();
	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;

protected:
	void setupWidget() override;
	void getValueFromInnerWidget() override;
	IntegerNumberEditWidget* m_widget;
};

#endif // GRIDATTRIBUTEINTEGEREDITDIALOG_H
