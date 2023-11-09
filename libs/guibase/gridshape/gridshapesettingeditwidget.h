#ifndef GRIDSHAPESETTINGEDITWIDGET_H
#define GRIDSHAPESETTINGEDITWIDGET_H

#include "../guibase_global.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class GridShapeSettingEditWidget;
}

class GridShapeSettingContainer;

class GUIBASEDLL_EXPORT GridShapeSettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit GridShapeSettingEditWidget(QWidget *parent);
	~GridShapeSettingEditWidget();

	void hideShape();
	void hideIndex();

	GridShapeSettingContainer setting() const;
	void setSetting(GridShapeSettingContainer* setting);
	void setSetting(const GridShapeSettingContainer& setting);

	QUndoCommand* createModifyCommand(bool allowMerge) override;

private slots:
	void handleOutlineOnlyClick();

private:
	GridShapeSettingContainer* m_setting;

	Ui::GridShapeSettingEditWidget *ui;
};

#endif // GRIDSHAPESETTINGEDITWIDGET_H
