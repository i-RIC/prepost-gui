#ifndef CONTOURSETTINGEDITWIDGET_H
#define CONTOURSETTINGEDITWIDGET_H

#include "../guicore_global.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class ContourSettingEditWidget;
}

class ContourSettingContainer;

class GUICOREDLL_EXPORT ContourSettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit ContourSettingEditWidget(QWidget *parent);
	~ContourSettingEditWidget();

	ContourSettingContainer setting() const;
	void setSetting(const ContourSettingContainer& setting);
	void setSetting(ContourSettingContainer* setting);

	QUndoCommand* createModifyCommand(bool apply) override;

private:
	ContourSettingContainer* m_setting;
	Ui::ContourSettingEditWidget *ui;
};

#endif // CONTOURSETTINGEDITWIDGET_H
