#ifndef ARROWSLEGENDSETTINGEDITWIDGET_H
#define ARROWSLEGENDSETTINGEDITWIDGET_H

#include "../guicore_global.h"

#include <QWidget>

namespace Ui {
class ArrowsLegendSettingEditWidget;
}

class ArrowsLegendSettingContainer;
class ImageSettingContainer;

class GUICOREDLL_EXPORT ArrowsLegendSettingEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ArrowsLegendSettingEditWidget(QWidget *parent = nullptr);
	~ArrowsLegendSettingEditWidget();

	ArrowsLegendSettingContainer setting() const;
	void setSetting(const ArrowsLegendSettingContainer& setting);
	void setImageSetting(const ImageSettingContainer& setting);

	void setTitle(const QString& title);

private:
	Ui::ArrowsLegendSettingEditWidget *ui;
};

#endif // ARROWSLEGENDSETTINGEDITWIDGET_H
