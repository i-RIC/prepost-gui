#ifndef POSTSTRINGRESULTSETTINGEDITWIDGET_H
#define POSTSTRINGRESULTSETTINGEDITWIDGET_H

#include "poststringresultengine.h"
#include "poststringresultsettingcontainer.h"

#include <misc/modifycommandwidget.h>
#include <QWidget>

namespace Ui {
class PostStringResultSettingEditWidget;
}

class PostStringResultSettingContainer;
class v4PostZoneDataContainer;

class PostStringResultSettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit PostStringResultSettingEditWidget(QWidget *parent = nullptr);
	~PostStringResultSettingEditWidget();

	void setZoneDataContainer(v4PostZoneDataContainer* container);

	PostStringResultSettingContainer setting() const;
	void setSetting(const PostStringResultSettingContainer& setting);
	void setSetting(PostStringResultSettingContainer* setting);

	QUndoCommand* createModifyCommand(bool apply) override;

public slots:
	void importSetting();
	void exportSetting();

private slots:
	void addArgument();
	void editSelectedArgument();
	void deleteSelectedArgument();

	void test();
	void updateImageSetting();

private:
	void applyScript();
	void updateArgumentsTable();

	static QString typeString(const PostStringResultArgumentContainer& arg);
	QString indexString(const PostStringResultArgumentContainer& arg);

	PostStringResultSettingContainer m_newSetting;
	PostStringResultSettingContainer* m_setting;
	v4PostZoneDataContainer* m_zoneDataContainer;

	PostStringResultEngine m_engine;

	Ui::PostStringResultSettingEditWidget *ui;
};

#endif // POSTSTRINGRESULTSETTINGEDITWIDGET_H
