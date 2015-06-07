#ifndef GRIDRELATEDCONDITIONVARIATIONEDITDIALOG_H
#define GRIDRELATEDCONDITIONVARIATIONEDITDIALOG_H

#include "../../../guicore_global.h"
#include "gridattributevariationeditwidget.h"

#include <QDialog>
#include <QVariant>
#include <vtkIdTypeArray.h>
class SolverDefinitionGridAttribute;
class GridAttributeContainer;

namespace Ui
{
	class GridAttributeVariationEditDialog;
}

class GUICOREDLL_EXPORT GridAttributeVariationEditDialog : public QDialog
{
	Q_OBJECT
public:
	GridAttributeVariationEditDialog(QWidget* parent);
	~GridAttributeVariationEditDialog();
	void setWidget(GridAttributeVariationEditWidget* w);
	GridAttributeVariationEditWidget* widget() const {return m_widget;}
	void clearValue() {
		m_widget->clearValue();
	}
	void setLabel(const QString& label);
	void applyVariation(GridAttributeContainer* container, QVector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem) {
		m_widget->applyVariation(container, indices, atts, dItem);
	}
	QVariant variantValue() const {return m_widget->variantValue();}
	void accept() override;
	void reject() override;

private:
	GridAttributeVariationEditWidget* m_widget;
	Ui::GridAttributeVariationEditDialog* ui;
};


#endif // GRIDRELATEDCONDITIONVARIATIONEDITDIALOG_H
