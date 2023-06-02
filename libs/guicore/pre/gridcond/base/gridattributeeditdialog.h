#ifndef GRIDATTRIBUTEEDITDIALOG_H
#define GRIDATTRIBUTEEDITDIALOG_H

#include "../../../guicore_global.h"
#include "gridattributeeditwidget.h"

#include <QDialog>
#include <vtkIdTypeArray.h>

class SolverDefinitionGridAttribute;
class GridAttributeContainer;

class QVariant;

namespace Ui
{
	class GridAttributeEditDialog;
}

class GUICOREDLL_EXPORT GridAttributeEditDialog : public QDialog
{
	Q_OBJECT
public:
	GridAttributeEditDialog(QWidget* parent);
	~GridAttributeEditDialog();

	GridAttributeEditWidget* widget() const;
	void setWidget(GridAttributeEditWidget* w);

	void clearValue();
	void disableCancel();

	bool isValueSelected() const;

	void setLabel(const QString& label);
	void setVariantValue(const QVariant& v);
	void scanAndSetDefault(GridAttributeContainer* container, const std::vector<vtkIdType>& indices);
	void applyValue(GridAttributeContainer* container, const std::vector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem);

	QVariant variantValue() const;

	void accept() override;
	void reject() override;

private:
	bool m_cancelDisabled;
	GridAttributeEditWidget* m_widget;
	Ui::GridAttributeEditDialog* ui;
};

#endif // GRIDATTRIBUTEEDITDIALOG_H
