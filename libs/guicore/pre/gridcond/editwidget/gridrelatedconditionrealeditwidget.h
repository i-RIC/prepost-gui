#ifndef GRIDRELATEDCONDITIONREALEDITWIDGET_H
#define GRIDRELATEDCONDITIONREALEDITWIDGET_H

#include "../base/gridrelatedconditioneditwidgett.h"

class RealNumberEditWidget;

class GridRelatedConditionRealEditWidget : public GridRelatedConditionEditWidgetT<double>
{
public:
	GridRelatedConditionRealEditWidget(QWidget* parent, SolverDefinitionGridRelatedConditionT<double>* cond);
	~GridRelatedConditionRealEditWidget();
	QSize sizeHint() const;
	QSize minimumSizeHint() const;
protected:
	void setupWidget();
	void getValueFromInnerWidget();
	RealNumberEditWidget* m_widget;
};

#endif // GRIDRELATEDCONDITIONREALEDITWIDGET_H
