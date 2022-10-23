#ifndef MEASUREDDATAVECTORGROUPDATAITEM_PROPERTYDIALOG_H
#define MEASUREDDATAVECTORGROUPDATAITEM_PROPERTYDIALOG_H

#include "../measureddatavectorgroupdataitem.h"

#include <QDialog>

namespace Ui {
class MeasuredDataVectorGroupDataItem_PropertyDialog;
}

class GeneralFilteringSettingEditWidget;

class QAbstractButton;
class QUndoCommand;

class MeasuredDataVectorGroupDataItem::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PropertyDialog(MeasuredDataVectorGroupDataItem* item, QWidget *parent = nullptr);
	~PropertyDialog();

	QUndoCommand* createModifyCommand();

	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void apply();

	bool m_applied;
	GeneralFilteringSettingEditWidget* m_filteringEditWidget;

	MeasuredDataVectorGroupDataItem* m_item;
	Ui::MeasuredDataVectorGroupDataItem_PropertyDialog *ui;
};

#endif // MEASUREDDATAVECTORGROUPDATAITEM_PROPERTYDIALOG_H
