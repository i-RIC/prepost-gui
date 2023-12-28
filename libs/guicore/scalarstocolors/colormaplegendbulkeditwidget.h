#ifndef COLORMAPLEGENDBULKEDITWIDGET_H
#define COLORMAPLEGENDBULKEDITWIDGET_H

#include <misc/modifycommandwidget.h>

#include <vector>

namespace Ui {
class ColorMapLegendBulkEditWidget;
}

class ColorMapLegendSettingContainerI;
class ImagePositionEditWidget;

class ColorMapLegendBulkEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	struct Setting {
		QString title;
		ColorMapLegendSettingContainerI * legendSetting;
	};

	explicit ColorMapLegendBulkEditWidget(QWidget *parent = nullptr);
	~ColorMapLegendBulkEditWidget();

	void setSettings(const std::vector<Setting> settings);

	QUndoCommand* createModifyCommand(bool apply) override;

private:
	std::vector<ImagePositionEditWidget*> m_widgets;
	std::vector<ColorMapLegendSettingContainerI*> m_settings;

	Ui::ColorMapLegendBulkEditWidget *ui;
};

#endif // COLORMAPLEGENDBULKEDITWIDGET_H
