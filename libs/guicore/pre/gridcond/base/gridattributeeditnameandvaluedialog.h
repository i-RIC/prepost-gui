#ifndef GRIDATTRIBUTEEDITNAMEANDVALUEDIALOG_H
#define GRIDATTRIBUTEEDITNAMEANDVALUEDIALOG_H

#include "../../../guicore_global.h"
#include "gridattributeeditwidget.h"

#include <QDialog>

namespace Ui {
class GridAttributeEditNameAndValueDialog;
}

class GUICOREDLL_EXPORT GridAttributeEditNameAndValueDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridAttributeEditNameAndValueDialog(QWidget *parent);
	~GridAttributeEditNameAndValueDialog();

	GridAttributeEditWidget* widget() const;
	void setWidget(GridAttributeEditWidget* w);

	void clearValue();
	void disableCancel();

	bool isValueSelected() const;

	void setName(const QString& name);
	QString name() const;

	void setVariantValue(const QVariant& v);
	QVariant variantValue() const;

	void accept() override;
	void reject() override;

private:
	bool m_cancelDisabled;
	GridAttributeEditWidget* m_widget;
	Ui::GridAttributeEditNameAndValueDialog *ui;
};

#endif // GRIDATTRIBUTEEDITNAMEANDVALUEDIALOG_H
