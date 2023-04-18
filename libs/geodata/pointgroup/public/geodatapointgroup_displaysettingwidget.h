#ifndef GEODATAPOINTGROUP_DISPLAYSETTINGWIDGET_H
#define GEODATAPOINTGROUP_DISPLAYSETTINGWIDGET_H

#include "../geodatapointgroup.h"

#include <misc/modifycommandwidget.h>

#include <QPixmap>

namespace Ui {
class GeoDataPointGroup_DisplaySettingWidget;
}

class GeoDataPointGroup::DisplaySettingWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit DisplaySettingWidget(QWidget *parent = nullptr);
	~DisplaySettingWidget();

	QUndoCommand* createModifyCommand(bool apply) override;

	DisplaySetting setting() const;
	void setSetting(const DisplaySetting& setting);

	void setSetting(DisplaySetting* setting);
	void setIsReferenceInformation(bool isReference);

private slots:
	void importImage();
	void updateImage();
	void clearImage();

private:
	QPixmap m_pixmap;
	DisplaySetting* m_displaySetting;
	Ui::GeoDataPointGroup_DisplaySettingWidget *ui;
};

#endif // GEODATAPOINTGROUP_DISPLAYSETTINGWIDGET_H
