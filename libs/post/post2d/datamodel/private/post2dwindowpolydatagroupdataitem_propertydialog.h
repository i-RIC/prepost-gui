#ifndef POST2DWINDOWPOLYDATAGROUPDATAITEM_PROPERTYDIALOG_H
#define POST2DWINDOWPOLYDATAGROUPDATAITEM_PROPERTYDIALOG_H

#include "../post2dwindowpolydatagroupdataitem.h"

#include <QDialog>

#include <postbase/polydata/polydatasetting.h>


namespace Ui {
class Post2dWindowPolyDataGroupDataItem_PropertyDialog;
}

class QUndoCommand;

class Post2dWindowPolyDataGroupDataItem::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PropertyDialog(Post2dWindowPolyDataGroupDataItem* item, QWidget *parent = nullptr);
	~PropertyDialog();

	QUndoCommand* createModifyCommand(bool apply = false);

	void accept() override;
	void reject() override;

private slots:
	void colorTargetChanged(int index);
	void handleButtonClick(QAbstractButton* button);

private:
	PolyDataSetting setting() const;
	void setSetting(const PolyDataSetting& setting);
	void apply();

	void setupColorTargets();
	std::string colorTarget() const;

	bool m_applied;

	std::vector<std::string> m_colorTargets;

	Post2dWindowPolyDataGroupDataItem* m_item;

	Ui::Post2dWindowPolyDataGroupDataItem_PropertyDialog *ui;
};

#endif // POST2DWINDOWPOLYDATAGROUPDATAITEM_PROPERTYDIALOG_H
