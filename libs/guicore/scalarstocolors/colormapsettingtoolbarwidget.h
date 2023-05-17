#ifndef COLORMAPSETTINGTOOLBARWIDGET_H
#define COLORMAPSETTINGTOOLBARWIDGET_H

#include "../guicore_global.h"
#include "colormaplegendsettingcontaineri.h"

#include <QWidget>

namespace Ui {
class ColorMapSettingToolBarWidget;
}

class GUICOREDLL_EXPORT ColorMapSettingToolBarWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ColorMapSettingToolBarWidget(QWidget *parent);
	~ColorMapSettingToolBarWidget();

	void setSetting(ColorMapSettingContainerI* setting);
	ColorMapSettingContainerI* modifiedSetting();

	bool visible() const;
	ColorMapLegendSettingContainerI::Direction direction() const;

signals:
	void updated();

private slots:
	void applySetting();

private:
	ColorMapSettingContainerI* m_setting;
	Ui::ColorMapSettingToolBarWidget *ui;
};

#endif // COLORMAPSETTINGTOOLBARWIDGET_H
