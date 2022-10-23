#ifndef POST3DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_PROPERTYDIALOG_H
#define POST3DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_PROPERTYDIALOG_H

#include "../post3dwindowparticlesbasevectorgroupdataitem.h"

#include <QDialog>

namespace Ui {
class Post3dWindowParticlesBaseVectorGroupDataItem_PropertyDialog;
}

class GeneralFilteringSettingEditWidget;

class QAbstractButton;
class QUndoCommand;

class Post3dWindowParticlesBaseVectorGroupDataItem::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PropertyDialog(Post3dWindowParticlesBaseVectorGroupDataItem* item, QWidget *parent = nullptr);
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

	Post3dWindowParticlesBaseVectorGroupDataItem* m_item;
	Ui::Post3dWindowParticlesBaseVectorGroupDataItem_PropertyDialog *ui;
};

#endif // POST3DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_PROPERTYDIALOG_H
