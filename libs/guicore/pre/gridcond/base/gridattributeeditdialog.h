#ifndef GRIDRELATEDCONDITIONEDITDIALOG_H
#define GRIDRELATEDCONDITIONEDITDIALOG_H

#include "../../../guicore_global.h"
#include "gridattributeeditwidget.h"

#include <QDialog>
#include <QVariant>
#include <vtkIdTypeArray.h>
class SolverDefinitionGridAttribute;
class GridAttributeContainer;

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
	void setWidget(GridAttributeEditWidget* w);
	GridAttributeEditWidget* widget() const {return m_widget;}
	void clearValue() {m_widget->clearValue();}
	/// Hide cancel button.
	void inhibitCancel(bool inhibited = true);
	bool valueSelected() const {return m_widget->valueSelected();}
	void setLabel(const QString& label);
	void setVariantValue(const QVariant& v) {m_widget->setVariantValue(v);}
	void scanAndSetDefault(GridAttributeContainer* container, QVector<vtkIdType>& indices) {
		m_widget->scanAndSetDefault(container, indices);
	}
	void applyValue(GridAttributeContainer* container, QVector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem) {
		m_widget->applyValue(container, indices, atts, dItem);
	}
	QVariant variantValue() const {return m_widget->variantValue();}
	void accept() override;
	void reject() override;

private:
	bool m_cancelInhibited;
	GridAttributeEditWidget* m_widget;
	Ui::GridAttributeEditDialog* ui;
};

#endif // GRIDRELATEDCONDITIONEDITDIALOG_H
