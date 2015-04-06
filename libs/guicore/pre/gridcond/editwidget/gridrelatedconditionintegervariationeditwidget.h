#ifndef GRIDRELATEDCONDITIONINTEGERVARIATIONEDITWIDGET_H
#define GRIDRELATEDCONDITIONINTEGERVARIATIONEDITWIDGET_H

#include "../base/gridrelatedconditionvariationeditwidgett.h"
class IntegerNumberEditWidget;

class GridRelatedConditionIntegerVariationEditWidget : public GridRelatedConditionVariationEditWidgetT<int>
{
public:
	GridRelatedConditionIntegerVariationEditWidget(QWidget* parent, SolverDefinitionGridRelatedConditionT<int>* cond);
	~GridRelatedConditionIntegerVariationEditWidget();
	QSize sizeHint() const;
	QSize minimumSizeHint() const;
protected:
	void setupWidget();
	void getValueFromInnerWidget();
	IntegerNumberEditWidget* m_widget;
};

#endif // GRIDRELATEDCONDITIONINTEGERVARIATIONEDITDIALOG_H
