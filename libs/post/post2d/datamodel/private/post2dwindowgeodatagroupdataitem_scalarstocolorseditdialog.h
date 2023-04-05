#ifndef POST2DWINDOWGEODATAGROUPDATAITEM_SCALARSTOCOLORSEDITDIALOG_H
#define POST2DWINDOWGEODATAGROUPDATAITEM_SCALARSTOCOLORSEDITDIALOG_H

#include "../post2dwindowgeodatagroupdataitem.h"

#include <QDialog>

namespace Ui {
class Post2dWindowGeoDataGroupDataItem_ScalarsToColorsEditDialog;
}

class DelegatedColorMapSettingContainer;

class Post2dWindowGeoDataGroupDataItem::ScalarsToColorsEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ScalarsToColorsEditDialog(Post2dWindowGeoDataGroupDataItem* item, QWidget *parent);
	~ScalarsToColorsEditDialog();

	void setSetting(DelegatedColorMapSettingContainer* setting);

	QUndoCommand* createModifyCommand(bool apply = false);

	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void apply();

	bool m_applied;
	Post2dWindowGeoDataGroupDataItem* m_item;

	Ui::Post2dWindowGeoDataGroupDataItem_ScalarsToColorsEditDialog *ui;
};

#endif // POST2DWINDOWGEODATAGROUPDATAITEM_SCALARSTOCOLORSEDITDIALOG_H
