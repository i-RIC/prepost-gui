#ifndef COLORMAPENUMERATELEGENDSETTINGEDITWIDGET_H
#define COLORMAPENUMERATELEGENDSETTINGEDITWIDGET_H

#include <QWidget>

namespace Ui {
class ColorMapEnumerateLegendSettingEditWidget;
}

class ColorMapEnumerateLegendSettingContainer;

class ColorMapEnumerateLegendSettingEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ColorMapEnumerateLegendSettingEditWidget(QWidget *parent = nullptr);
	~ColorMapEnumerateLegendSettingEditWidget();

	ColorMapEnumerateLegendSettingContainer setting() const;
	void setSetting(const ColorMapEnumerateLegendSettingContainer& setting);
	void setDisableOtherThanImageSetting(bool disabled);

private:
	Ui::ColorMapEnumerateLegendSettingEditWidget *ui;
};

#endif // COLORMAPENUMERATELEGENDSETTINGEDITWIDGET_H
