#ifndef GRIDRELATEDCONDITIONEDITWIDGET_H
#define GRIDRELATEDCONDITIONEDITWIDGET_H

#include "../../../guicore_global.h"
#include <QWidget>
#include <vtkIdList.h>
class SolverDefinitionGridRelatedCondition;
class GridRelatedConditionContainer;
class vtkDataSetAttributes;
class PreProcessorGridDataItemInterface;

class GUICOREDLL_EXPORT GridRelatedConditionEditWidget : public QWidget
{
public:
	GridRelatedConditionEditWidget(QWidget* parent, SolverDefinitionGridRelatedCondition* condition)
		: QWidget(parent) {
		m_valueCleared = false;
		m_valueSelected = true;
		m_gridRelatedCondition = condition;
	}
	void clearValue() {
		m_valueCleared = true;
		m_valueSelected = false;
		setupWidget();
	}
	bool valueSelected() {
		getValueFromInnerWidget();
		return m_valueSelected;
	}
	SolverDefinitionGridRelatedCondition* condition() {
		return m_gridRelatedCondition;
	}
	virtual void setVariantValue(const QVariant& v) = 0;
	virtual void scanAndSetDefault(GridRelatedConditionContainer* container, QVector<vtkIdType>& indices) = 0;
	virtual void applyValue(GridRelatedConditionContainer* container, QVector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem) = 0;
	virtual QVariant variantValue() = 0;
protected:
	virtual void setupWidget() = 0;
	virtual void getValueFromInnerWidget() = 0;
	bool m_valueCleared;
	bool m_valueSelected;

	SolverDefinitionGridRelatedCondition* m_gridRelatedCondition;
};

#endif // GRIDRELATEDCONDITIONEDITDIALOG_H
