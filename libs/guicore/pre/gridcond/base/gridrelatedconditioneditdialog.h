#ifndef GRIDRELATEDCONDITIONEDITDIALOG_H
#define GRIDRELATEDCONDITIONEDITDIALOG_H

#include "../../../guicore_global.h"
#include "gridrelatedconditioneditwidget.h"

#include <QDialog>
#include <QVariant>
#include <vtkIdTypeArray.h>
class SolverDefinitionGridRelatedCondition;
class GridRelatedConditionContainer;

namespace Ui
{
	class GridRelatedConditionEditDialog;
}

class GUICOREDLL_EXPORT GridRelatedConditionEditDialog : public QDialog
{
	Q_OBJECT
public:
	GridRelatedConditionEditDialog(QWidget* parent);
	~GridRelatedConditionEditDialog();
	void setWidget(GridRelatedConditionEditWidget* w);
	GridRelatedConditionEditWidget* widget() const {return m_widget;}
	void clearValue() {m_widget->clearValue();}
	/// Hide cancel button.
	void inhibitCancel(bool inhibited = true);
	bool valueSelected() const {return m_widget->valueSelected();}
	void setLabel(const QString& label);
	void setVariantValue(const QVariant& v) {m_widget->setVariantValue(v);}
	void scanAndSetDefault(GridRelatedConditionContainer* container, QVector<vtkIdType>& indices) {
		m_widget->scanAndSetDefault(container, indices);
	}
	void applyValue(GridRelatedConditionContainer* container, QVector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem) {
		m_widget->applyValue(container, indices, atts, dItem);
	}
	QVariant variantValue() const {return m_widget->variantValue();}
	void accept() override;
	void reject() override;

private:
	bool m_cancelInhibited;
	GridRelatedConditionEditWidget* m_widget;
	Ui::GridRelatedConditionEditDialog* ui;
};

#endif // GRIDRELATEDCONDITIONEDITDIALOG_H
