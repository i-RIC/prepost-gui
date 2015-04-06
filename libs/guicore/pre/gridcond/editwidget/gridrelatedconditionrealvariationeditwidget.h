#ifndef GRIDRELATEDCONDITIONREALVARIATIONEDITWIDGET_H
#define GRIDRELATEDCONDITIONREALVARIATIONEDITWIDGET_H

#include "../base/gridrelatedconditionvariationeditwidgett.h"

class RealNumberEditWidget;

class GridRelatedConditionRealVariationEditWidget : public GridRelatedConditionVariationEditWidgetT<double>
{
public:
	GridRelatedConditionRealVariationEditWidget(QWidget* parent, SolverDefinitionGridRelatedConditionT<double>* cond);
	~GridRelatedConditionRealVariationEditWidget();
	QSize sizeHint() const;
	QSize minimumSizeHint() const;
protected:
	void setupWidget();
	void getValueFromInnerWidget();
	RealNumberEditWidget* m_widget;
};

#endif // GRIDRELATEDCONDITIONREALVARIATIONEDITWIDGET_H
