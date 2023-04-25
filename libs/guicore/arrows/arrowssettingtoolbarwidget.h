#ifndef ARROWSSETTINGTOOLBARWIDGET_H
#define ARROWSSETTINGTOOLBARWIDGET_H

#include "../guicore_global.h"

#include <QWidget>

#include <unordered_map>

namespace Ui {
class ArrowsSettingToolBarWidget;
}

class ArrowsSettingContainer;
class ColorMapSettingContainer;

class GUICOREDLL_EXPORT ArrowsSettingToolBarWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ArrowsSettingToolBarWidget(QWidget *parent);
	~ArrowsSettingToolBarWidget();

	void setColorMapSettings(const std::unordered_map<std::string, ColorMapSettingContainer*>& settings);
	void setSetting(ArrowsSettingContainer* setting);

	void hideLegendCheckbox();

	ArrowsSettingContainer modifiedSetting() const;
	ColorMapSettingContainer modifiedColorMapSetting() const;

signals:
	void updated();

public slots:
	void handleValueChange(int index);
	void applySetting();

private:
	ArrowsSettingContainer* m_setting;
	std::unordered_map<std::string, ColorMapSettingContainer*> m_colorMapSettings;
	std::vector<std::string> m_colorMapNames;

	Ui::ArrowsSettingToolBarWidget *ui;
};

#endif // ARROWSSETTINGTOOLBARWIDGET_H
