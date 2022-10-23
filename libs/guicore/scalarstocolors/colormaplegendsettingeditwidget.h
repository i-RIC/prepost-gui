#ifndef COLORMAPLEGENDSETTINGEDITWIDGET_H
#define COLORMAPLEGENDSETTINGEDITWIDGET_H

#include <QWidget>

namespace Ui {
class ColorMapLegendSettingEditWidget;
}

class ColorMapLegendSettingContainer;

class ColorMapLegendSettingEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ColorMapLegendSettingEditWidget(QWidget *parent = nullptr);
	~ColorMapLegendSettingEditWidget();

	ColorMapLegendSettingContainer setting() const;
	void setSetting(const ColorMapLegendSettingContainer& setting);

private:
	Ui::ColorMapLegendSettingEditWidget *ui;
};

#endif // COLORMAPLEGENDSETTINGEDITWIDGET_H
