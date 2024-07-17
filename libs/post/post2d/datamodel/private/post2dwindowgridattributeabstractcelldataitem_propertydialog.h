#ifndef POST2DWINDOWGRIDATTRIBUTEABSTRACTCELLDATAITEM_PROPERTYDIALOG_H
#define POST2DWINDOWGRIDATTRIBUTEABSTRACTCELLDATAITEM_PROPERTYDIALOG_H

#include "../post2dwindowgridattributeabstractcelldataitem.h"

#include <QDialog>

class ColorMapSettingEditWidgetI;
class GridAttributeCellSetting;
class Post2dWindowGridAttributeAbstractCellGroupDataItem;

namespace Ui {
class Post2dWindowGridAttributeAbstractCellDataItem_PropertyDialog;
}

class Post2dWindowGridAttributeAbstractCellDataItem::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	PropertyDialog(Post2dWindowGridAttributeAbstractCellGroupDataItem* item, QWidget *parent);
	~PropertyDialog();

	void hideLineWidth();

	ColorMapSettingEditWidgetI* widget() const;
	void setWidget(ColorMapSettingEditWidgetI* w);

	void setSetting(GridAttributeCellSetting* setting);

	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void apply();
	QUndoCommand* createModifyCommand(bool apply);

	bool m_applied;
	Post2dWindowGridAttributeAbstractCellGroupDataItem* m_item;

	Ui::Post2dWindowGridAttributeAbstractCellDataItem_PropertyDialog *ui;
};

#endif // POST2DWINDOWGRIDATTRIBUTEABSTRACTCELLDATAITEM_PROPERTYDIALOG_H
