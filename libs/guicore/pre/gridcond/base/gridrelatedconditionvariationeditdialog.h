#ifndef GRIDRELATEDCONDITIONVARIATIONEDITDIALOG_H
#define GRIDRELATEDCONDITIONVARIATIONEDITDIALOG_H

#include "../../../guicore_global.h"
#include "gridrelatedconditionvariationeditwidget.h"

#include <QDialog>
#include <QVariant>
#include <vtkIdTypeArray.h>
class SolverDefinitionGridRelatedCondition;
class GridRelatedConditionContainer;

namespace Ui
{
	class GridRelatedConditionVariationEditDialog;
}

class GUICOREDLL_EXPORT GridRelatedConditionVariationEditDialog : public QDialog
{
	Q_OBJECT
public:
	GridRelatedConditionVariationEditDialog(QWidget* parent);
	~GridRelatedConditionVariationEditDialog();
	void setWidget(GridRelatedConditionVariationEditWidget* w);
	GridRelatedConditionVariationEditWidget* widget() {return m_widget;}
	void clearValue() {
		m_widget->clearValue();
	}
	void setLabel(const QString& label);
	void applyVariation(GridRelatedConditionContainer* container, QVector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem) {
		m_widget->applyVariation(container, indices, atts, dItem);
	}
	QVariant variantValue() {return m_widget->variantValue();}
	void accept();
	void reject();

private:
	GridRelatedConditionVariationEditWidget* m_widget;
	Ui::GridRelatedConditionVariationEditDialog* ui;
};


#endif // GRIDRELATEDCONDITIONVARIATIONEDITDIALOG_H
