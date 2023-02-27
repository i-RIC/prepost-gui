#ifndef COLORMAPSETTINGEDITWIDGETI_H
#define COLORMAPSETTINGEDITWIDGETI_H

#include "../guicore_global.h"

#include <QWidget>

class QUndoCommand;

class ColorMapLegendSettingContainerI;
class ColorMapSettingContainerI;

class GUICOREDLL_EXPORT ColorMapSettingEditWidgetI : public QWidget
{
	Q_OBJECT

public:
	ColorMapSettingEditWidgetI(QWidget* parent = nullptr);
	virtual ~ColorMapSettingEditWidgetI();

	ColorMapSettingContainerI* setting() const;
	void setSetting(ColorMapSettingContainerI* setting);

	ColorMapLegendSettingContainerI* legendSetting() const;
	void setLegendSetting(ColorMapLegendSettingContainerI* setting);

	virtual QUndoCommand* createModifyCommand() const = 0;

public slots:
	virtual void importSetting() = 0;
	virtual void exportSetting() = 0;

private:
	virtual void setupWidget() = 0;

protected:
	ColorMapSettingContainerI* m_setting;
	ColorMapLegendSettingContainerI* m_legendSetting;
};

#endif // COLORMAPSETTINGEDITWIDGETI_H
