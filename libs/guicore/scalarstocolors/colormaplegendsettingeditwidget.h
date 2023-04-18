#ifndef COLORMAPLEGENDSETTINGEDITWIDGET_H
#define COLORMAPLEGENDSETTINGEDITWIDGET_H

#include <QWidget>

namespace Ui {
class ColorMapLegendSettingEditWidget;
}

class ColorMapSettingContainer;
class ColorMapLegendSettingContainer;
class ImageSettingContainer;

class ColorMapLegendSettingEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ColorMapLegendSettingEditWidget(QWidget *parent = nullptr);
	~ColorMapLegendSettingEditWidget();

	ColorMapLegendSettingContainer setting() const;
	void setSetting(const ColorMapLegendSettingContainer& setting);
	void setImageSetting(const ImageSettingContainer& setting);

	void updateAutoNumberOfLabels();
	void updateNumberOfLabelsIfNeeded();
	void setDisableOtherThanVisible(bool disabled);

private slots:
	void handleAutoNumberOfLabels(bool checked);

private:
	ColorMapSettingContainer* m_colorMapSetting;
	bool m_delegateMode;
	Ui::ColorMapLegendSettingEditWidget *ui;
};

#endif // COLORMAPLEGENDSETTINGEDITWIDGET_H
