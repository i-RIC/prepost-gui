#ifndef PARTICLEDATASETTINGEDITWIDGET_H
#define PARTICLEDATASETTINGEDITWIDGET_H

#include "../postbase_global.h"

#include <misc/modifycommandwidget.h>

#include <unordered_map>

namespace Ui {
class ParticleDataSettingEditWidget;
}

class ColorMapSettingContainerI;
class ColorMapSettingEditWidgetI;
class ParticleDataSetting;
class SolverDefinitionGridType;

class POSTBASEDLL_EXPORT ParticleDataSettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit ParticleDataSettingEditWidget(QWidget *parent);
	~ParticleDataSettingEditWidget();

	void setValueNames(const std::unordered_map<std::string, QString>& names);
	void setGridType(SolverDefinitionGridType* gridType);
	void setColorMapSettings(const std::unordered_map<std::string, ColorMapSettingContainerI*>& settings);
	void setSetting(ParticleDataSetting* setting);

	QUndoCommand* createModifyCommand(bool apply) override;

private slots:
	void colorTargetChanged(int index);

private:
	ParticleDataSetting setting() const;
	void setSetting(const ParticleDataSetting& setting);
	std::string colorTarget() const;

	ParticleDataSetting* m_setting;
	std::unordered_map<std::string, ColorMapSettingContainerI*> m_colorMapSettings;
	std::vector<std::string> m_colorTargets;
	ColorMapSettingEditWidgetI* m_colorMapWidget;
	SolverDefinitionGridType* m_gridType;

	Ui::ParticleDataSettingEditWidget *ui;

	class ModifyCommand;
};

#endif // PARTICLEDATASETTINGEDITWIDGET_H
