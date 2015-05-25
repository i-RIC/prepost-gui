#ifndef GRIDRELATEDCONDITIONINTEGERVARIATIONEDITWIDGET_H
#define GRIDRELATEDCONDITIONINTEGERVARIATIONEDITWIDGET_H

#include "../base/gridrelatedconditionvariationeditwidgett.h"
class IntegerNumberEditWidget;

class GridRelatedConditionIntegerVariationEditWidget : public GridRelatedConditionVariationEditWidgetT<int>
{
public:
	GridRelatedConditionIntegerVariationEditWidget(QWidget* parent, SolverDefinitionGridRelatedConditionT<int>* cond);
	~GridRelatedConditionIntegerVariationEditWidget();
	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;
protected:
	void setupWidget() override;
	void getValueFromInnerWidget() override;
	IntegerNumberEditWidget* m_widget;
};

#endif // GRIDRELATEDCONDITIONINTEGERVARIATIONEDITDIALOG_H
