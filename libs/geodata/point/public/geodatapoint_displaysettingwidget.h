#ifndef GEODATAPOINT_DISPLAYSETTINGWIDGET_H
#define GEODATAPOINT_DISPLAYSETTINGWIDGET_H

#include "../geodatapoint.h"

#include <misc/modifycommandwidget.h>

#include <QPixmap>

namespace Ui {
class GeoDataPoint_DisplaySettingWidget;
}

class GeoDataPoint::DisplaySettingWidget : public ModifyCommandWidget
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
	Ui::GeoDataPoint_DisplaySettingWidget *ui;
};

#endif // GEODATAPOINT_DISPLAYSETTINGWIDGET_H
