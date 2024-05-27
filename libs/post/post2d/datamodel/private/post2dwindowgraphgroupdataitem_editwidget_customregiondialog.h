#ifndef POST2DWINDOWGRAPHGROUPDATAITEM_EDITWIDGET_CUSTOMREGIONDIALOG_H
#define POST2DWINDOWGRAPHGROUPDATAITEM_EDITWIDGET_CUSTOMREGIONDIALOG_H

#include "post2dwindowgraphgroupdataitem_editwidget.h"

#include <QDialog>

namespace Ui {
class Post2dWindowGraphGroupDataItem_EditWidget_CustomRegionDialog;
}

class Post2dWindowGraphGroupDataItem::EditWidget::CustomRegionDialog : public QDialog
{
	Q_OBJECT

public:
	explicit CustomRegionDialog(QWidget *parent);
	~CustomRegionDialog();

	void setIndexCount(int count);

	std::vector<int> setting() const;
	void setSetting(const std::vector<int>& setting);

private slots:
	void selectAll();
	void unselectAll();

private:
	Ui::Post2dWindowGraphGroupDataItem_EditWidget_CustomRegionDialog *ui;
};

#endif // POST2DWINDOWGRAPHGROUPDATAITEM_EDITWIDGET_CUSTOMREGIONDIALOG_H
