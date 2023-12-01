#ifndef DIRECTIONSETTINGEDITWIDGET_H
#define DIRECTIONSETTINGEDITWIDGET_H

#include "../guicore_global.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class DirectionSettingEditWidget;
}

class DirectionSettingContainer;

class GUICOREDLL_EXPORT DirectionSettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	DirectionSettingEditWidget(QWidget *parent = nullptr);
	~DirectionSettingEditWidget();

	DirectionSettingContainer setting() const;
	void setSetting(const DirectionSettingContainer& setting);
	void setSetting(DirectionSettingContainer* setting);

	QUndoCommand* createModifyCommand(bool apply) override;

private:
	DirectionSettingContainer* m_setting;
	Ui::DirectionSettingEditWidget *ui;
};

#endif // DIRECTIONSETTINGEDITWIDGET_H
