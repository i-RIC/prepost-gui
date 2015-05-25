#ifndef GRIDRELATEDCONDITIONREALVARIATIONEDITWIDGET_H
#define GRIDRELATEDCONDITIONREALVARIATIONEDITWIDGET_H

#include "../base/gridrelatedconditionvariationeditwidgett.h"

class RealNumberEditWidget;

class GridRelatedConditionRealVariationEditWidget : public GridRelatedConditionVariationEditWidgetT<double>
{
public:
	GridRelatedConditionRealVariationEditWidget(QWidget* parent, SolverDefinitionGridRelatedConditionT<double>* cond);
	~GridRelatedConditionRealVariationEditWidget();
	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;
protected:
	void setupWidget() override;
	void getValueFromInnerWidget() override;
	RealNumberEditWidget* m_widget;
};

#endif // GRIDRELATEDCONDITIONREALVARIATIONEDITWIDGET_H
