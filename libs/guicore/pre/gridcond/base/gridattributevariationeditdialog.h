#ifndef GRIDATTRIBUTEVARIATIONEDITDIALOG_H
#define GRIDATTRIBUTEVARIATIONEDITDIALOG_H

#include "../../../guicore_global.h"
#include "gridattributevariationeditwidget.h"

#include <QDialog>
#include <QVariant>
#include <vtkIdTypeArray.h>

class SolverDefinitionGridAttribute;
class GridAttributeVariationEditWidget;
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

	GridAttributeVariationEditWidget* widget() const;
	void setWidget(GridAttributeVariationEditWidget* w);

	void clearValue();
	void setLabel(const QString& label);

	void applyVariation(GridAttributeContainer* container, QVector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem);
	QVariant variantValue() const;

	void accept() override;
	void reject() override;

private:
	GridAttributeVariationEditWidget* m_widget;
	Ui::GridAttributeVariationEditDialog* ui;
};

#endif // GRIDATTRIBUTEVARIATIONEDITDIALOG_H
