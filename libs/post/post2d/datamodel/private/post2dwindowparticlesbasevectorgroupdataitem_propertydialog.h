#ifndef POST2DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_PROPERTYDIALOG_H
#define POST2DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_PROPERTYDIALOG_H

#include "../post2dwindowparticlesbasevectorgroupdataitem.h"

#include <QDialog>

namespace Ui {
class Post2dWindowParticlesBaseVectorGroupDataItem_PropertyDialog;
}

class GeneralFilteringSettingEditWidget;

class QAbstractButton;
class QUndoCommand;

class Post2dWindowParticlesBaseVectorGroupDataItem::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PropertyDialog(Post2dWindowParticlesBaseVectorGroupDataItem* item, QWidget *parent = nullptr);
	~PropertyDialog();

	QUndoCommand* createModifyCommand(bool apply);

	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void apply();

	bool m_applied;
	GeneralFilteringSettingEditWidget* m_filteringEditWidget;

	Post2dWindowParticlesBaseVectorGroupDataItem* m_item;
	Ui::Post2dWindowParticlesBaseVectorGroupDataItem_PropertyDialog *ui;
};

#endif // POST2DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_PROPERTYDIALOG_H
