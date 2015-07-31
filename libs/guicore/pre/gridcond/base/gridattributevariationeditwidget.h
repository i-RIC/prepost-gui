#ifndef GRIDRELATEDCONDITIONVARIATIONEDITWIDGET_H
#define GRIDRELATEDCONDITIONVARIATIONEDITWIDGET_H

#include <QWidget>
#include <vtkIdList.h>
class SolverDefinitionGridAttribute;
class GridAttributeContainer;
class vtkDataSetAttributes;
class PreProcessorGridDataItemInterface;

class GridAttributeVariationEditWidget : public QWidget
{
	Q_OBJECT

public:
	GridAttributeVariationEditWidget(QWidget* parent, SolverDefinitionGridAttribute* condition) :
		QWidget {parent},
		m_gridRelatedCondition {condition}
	{}
	void clearValue() {
		setupWidget();
	}
	SolverDefinitionGridAttribute* condition() {return m_gridRelatedCondition;}
	virtual void applyVariation(GridAttributeContainer* container, QVector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem) = 0;
	virtual QVariant variantValue() = 0;

protected:
	virtual void setupWidget() = 0;
	virtual void getValueFromInnerWidget() = 0;

	SolverDefinitionGridAttribute* m_gridRelatedCondition;
};

#endif // GRIDRELATEDCONDITIONVARIATIONEDITWIDGET_H
