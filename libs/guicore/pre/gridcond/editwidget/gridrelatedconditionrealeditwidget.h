#ifndef GRIDRELATEDCONDITIONREALEDITWIDGET_H
#define GRIDRELATEDCONDITIONREALEDITWIDGET_H

#include "../base/gridrelatedconditioneditwidgett.h"

class RealNumberEditWidget;

class GridRelatedConditionRealEditWidget : public GridRelatedConditionEditWidgetT<double>
{
public:
	GridRelatedConditionRealEditWidget(QWidget* parent, SolverDefinitionGridRelatedConditionT<double>* cond);
	~GridRelatedConditionRealEditWidget();
	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;
protected:
	void setupWidget() override;
	void getValueFromInnerWidget() override;
	RealNumberEditWidget* m_widget;
};

#endif // GRIDRELATEDCONDITIONREALEDITWIDGET_H
