#ifndef GRIDRELATEDCONDITIONINTEGEREDITWIDGET_H
#define GRIDRELATEDCONDITIONINTEGEREDITWIDGET_H

#include "../base/gridrelatedconditioneditwidgett.h"
class IntegerNumberEditWidget;

class GridRelatedConditionIntegerEditWidget : public GridRelatedConditionEditWidgetT<int>
{
public:
	GridRelatedConditionIntegerEditWidget(QWidget* parent, SolverDefinitionGridRelatedConditionT<int>* cond);
	~GridRelatedConditionIntegerEditWidget();
	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;
protected:
	void setupWidget() override;
	void getValueFromInnerWidget() override;
	IntegerNumberEditWidget* m_widget;
};

#endif // GRIDRELATEDCONDITIONINTEGEREDITDIALOG_H
