#ifndef GRIDRELATEDCONDITIONINTEGEREDITWIDGET_H
#define GRIDRELATEDCONDITIONINTEGEREDITWIDGET_H

#include "../base/gridrelatedconditioneditwidgett.h"
class IntegerNumberEditWidget;

class GridRelatedConditionIntegerEditWidget : public GridRelatedConditionEditWidgetT<int>
{
public:
	GridRelatedConditionIntegerEditWidget(QWidget* parent, SolverDefinitionGridRelatedConditionT<int>* cond);
	~GridRelatedConditionIntegerEditWidget();
	QSize sizeHint() const;
	QSize minimumSizeHint() const;
protected:
	void setupWidget();
	void getValueFromInnerWidget();
	IntegerNumberEditWidget* m_widget;
};

#endif // GRIDRELATEDCONDITIONINTEGEREDITDIALOG_H
