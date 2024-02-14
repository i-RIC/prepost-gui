#ifndef POST2DWINDOWPARTICLEIMAGEDATAITEM_SETTINGEDITWIDGET_H
#define POST2DWINDOWPARTICLEIMAGEDATAITEM_SETTINGEDITWIDGET_H

#include "../post2dwindowparticleimagedataitem.h"
#include "post2dwindowparticleimagedataitem_setting.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class Post2dWindowParticleImageDataItem_SettingEditWidget;
}

class Post2dWindowParticleImageDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	SettingEditWidget(Post2dWindowParticleImageDataItem* item, QWidget *parent);
	~SettingEditWidget();

	QTableView* sizesTable() const;
	QPushButton* removeButton() const;

	QUndoCommand* createModifyCommand(bool apply) override;
	void setSetting(const Setting& setting);

private slots:
	void importImage();
	void clearImage();

	void addValue();
	void removeValue();

private:
	Setting setting() const;
	void updateWidthAndHeight();

	Setting m_setting;
	Post2dWindowParticleImageDataItem* m_item;
	Ui::Post2dWindowParticleImageDataItem_SettingEditWidget *ui;

	class ValueSizePairTableController;
	ValueSizePairTableController* m_valueSizePairTableController;
};

#endif // POST2DWINDOWPARTICLEIMAGEDATAITEM_SETTINGEDITWIDGET_H
