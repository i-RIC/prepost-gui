#ifndef POLYDATASETTINGTOOLBARWIDGET_H
#define POLYDATASETTINGTOOLBARWIDGET_H

#include "../postbase_global.h"

#include <QWidget>

#include <unordered_map>

namespace Ui {
class PolyDataSettingToolBarWidget;
}

class PolyDataSetting;
class ColorMapSettingContainerI;

class POSTBASEDLL_EXPORT PolyDataSettingToolBarWidget : public QWidget
{
	Q_OBJECT

public:
	explicit PolyDataSettingToolBarWidget(QWidget *parent);
	~PolyDataSettingToolBarWidget();

	void setColorMapSettings(const std::unordered_map<std::string, ColorMapSettingContainerI*>& settings);
	void setSetting(PolyDataSetting* setting);

	PolyDataSetting modifiedSetting() const;
	ColorMapSettingContainerI* modifiedColorMapSetting() const;

signals:
	void updated();

public slots:
	void handleValueChange(int index);
	void applySetting();

private:
	PolyDataSetting* m_setting;
	std::unordered_map<std::string, ColorMapSettingContainerI*> m_colorMapSettings;
	std::vector<std::string> m_colorMapNames;

	Ui::PolyDataSettingToolBarWidget *ui;
};

#endif // POLYDATASETTINGTOOLBARWIDGET_H
