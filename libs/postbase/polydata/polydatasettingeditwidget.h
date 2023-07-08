#ifndef POLYDATASETTINGEDITWIDGET_H
#define POLYDATASETTINGEDITWIDGET_H

#include "../postbase_global.h"

#include <misc/modifycommandwidget.h>

#include <string>
#include <unordered_map>

namespace Ui {
class PolyDataSettingEditWidget;
}

class ColorMapSettingContainerI;
class ColorMapSettingEditWidgetI;
class PolyDataSetting;
class SolverDefinitionGridType;

class POSTBASEDLL_EXPORT PolyDataSettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit PolyDataSettingEditWidget(QWidget *parent);
	~PolyDataSettingEditWidget();

	void setGridType(SolverDefinitionGridType* gridType);
	void setValueNames(const std::unordered_map<std::string, QString>& names);
	void setColorMapSettings(const std::unordered_map<std::string, ColorMapSettingContainerI*>& settings);
	void setSetting(PolyDataSetting* setting);

	QUndoCommand* createModifyCommand(bool apply) override;

private slots:
	void colorTargetChanged(int index);

private:
	PolyDataSetting setting() const;
	void setSetting(const PolyDataSetting& setting);
	std::string colorTarget() const;

	PolyDataSetting* m_setting;
	std::unordered_map<std::string, ColorMapSettingContainerI*> m_colorMapSettings;
	std::vector<std::string> m_colorTargets;
	ColorMapSettingEditWidgetI* m_colorMapWidget;
	SolverDefinitionGridType* m_gridType;
	Ui::PolyDataSettingEditWidget *ui;

	class ModifyCommand;
};

#endif // POLYDATASETTINGEDITWIDGET_H
