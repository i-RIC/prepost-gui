#ifndef COLORMAPENUMERATELEGENDSETTINGEDITWIDGET_H
#define COLORMAPENUMERATELEGENDSETTINGEDITWIDGET_H

#include <QWidget>

namespace Ui {
class ColorMapEnumerateLegendSettingEditWidget;
}

class ColorMapEnumerateLegendSettingContainer;
class ImageSettingContainer;

class ColorMapEnumerateLegendSettingEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ColorMapEnumerateLegendSettingEditWidget(QWidget *parent = nullptr);
	~ColorMapEnumerateLegendSettingEditWidget();

	ColorMapEnumerateLegendSettingContainer setting() const;
	void setSetting(const ColorMapEnumerateLegendSettingContainer& setting);
	void setImageSetting(const ImageSettingContainer& setting);
	void setDisableOtherThanImageVisible(bool disabled);

private:
	bool m_delegateMode;
	Ui::ColorMapEnumerateLegendSettingEditWidget *ui;
};

#endif // COLORMAPENUMERATELEGENDSETTINGEDITWIDGET_H
