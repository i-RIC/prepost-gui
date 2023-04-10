#ifndef COLORMAPLEGENDSETTINGEDITWIDGET_H
#define COLORMAPLEGENDSETTINGEDITWIDGET_H

#include <QWidget>

namespace Ui {
class ColorMapLegendSettingEditWidget;
}

class ColorMapSettingContainer;
class ColorMapLegendSettingContainer;

class ColorMapLegendSettingEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ColorMapLegendSettingEditWidget(QWidget *parent = nullptr);
	~ColorMapLegendSettingEditWidget();

	ColorMapLegendSettingContainer setting() const;
	void setSetting(const ColorMapLegendSettingContainer& setting);

	void updateAutoNumberOfLabels();
	void updateNumberOfLabelsIfNeeded();
	void setDisableOtherThanImageSetting(bool disabled);

private slots:
	void handleAutoNumberOfLabels(bool checked);

private:
	ColorMapSettingContainer* m_colorMapSetting;
	Ui::ColorMapLegendSettingEditWidget *ui;
};

#endif // COLORMAPLEGENDSETTINGEDITWIDGET_H
