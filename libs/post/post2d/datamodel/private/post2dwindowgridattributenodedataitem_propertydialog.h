#ifndef POST2DWINDOWGRIDATTRIBUTENODEDATAITEM_PROPERTYDIALOG_H
#define POST2DWINDOWGRIDATTRIBUTENODEDATAITEM_PROPERTYDIALOG_H

#include "../post2dwindowgridattributenodedataitem.h"

#include <QDialog>

class ColorMapSettingEditWidgetI;
class OpacityContainer;
class Post2dWindowGridAttributeNodeGroupDataItem;

namespace Ui {
class Post2dWindowGridAttributeNodeDataItem_PropertyDialog;
}

class Post2dWindowGridAttributeNodeDataItem::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	PropertyDialog(Post2dWindowGridAttributeNodeGroupDataItem* item, QWidget *parent);
	~PropertyDialog();

	ColorMapSettingEditWidgetI* widget() const;
	void setWidget(ColorMapSettingEditWidgetI* w);

	OpacityContainer opacity() const;
	void setOpacity(const OpacityContainer& opacity);

	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void apply();
	QUndoCommand* createModifyCommand(bool apply);

	bool m_applied;
	Post2dWindowGridAttributeNodeGroupDataItem* m_item;

	Ui::Post2dWindowGridAttributeNodeDataItem_PropertyDialog *ui;
};

#endif // POST2DWINDOWGRIDATTRIBUTENODEDATAITEM_PROPERTYDIALOG_H
