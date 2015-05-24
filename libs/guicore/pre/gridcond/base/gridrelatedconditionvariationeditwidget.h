#ifndef GRIDRELATEDCONDITIONVARIATIONEDITWIDGET_H
#define GRIDRELATEDCONDITIONVARIATIONEDITWIDGET_H

#include <QWidget>
#include <vtkIdList.h>
class SolverDefinitionGridRelatedCondition;
class GridRelatedConditionContainer;
class vtkDataSetAttributes;
class PreProcessorGridDataItemInterface;

class GridRelatedConditionVariationEditWidget : public QWidget
{
public:
	GridRelatedConditionVariationEditWidget(QWidget* parent, SolverDefinitionGridRelatedCondition* condition)
		: QWidget(parent) {
		m_gridRelatedCondition = condition;
	}
	void clearValue() {
		setupWidget();
	}
	SolverDefinitionGridRelatedCondition* condition() {
		return m_gridRelatedCondition;
	}
	virtual void applyVariation(GridRelatedConditionContainer* container, QVector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem) = 0;
	virtual QVariant variantValue() = 0;

protected:
	virtual void setupWidget() = 0;
	virtual void getValueFromInnerWidget() = 0;

	SolverDefinitionGridRelatedCondition* m_gridRelatedCondition;
};

#endif // GRIDRELATEDCONDITIONVARIATIONEDITWIDGET_H
