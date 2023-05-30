#ifndef PARTICLEDATASETTINGEDITWIDGET_H
#define PARTICLEDATASETTINGEDITWIDGET_H

#include "../postbase_global.h"

#include <misc/modifycommandwidget.h>

#include <unordered_map>

namespace Ui {
class ParticleDataSettingEditWidget;
}

class ColorMapSettingContainer;
class ColorMapSettingEditWidget;
class ParticleDataSetting;

class POSTBASEDLL_EXPORT ParticleDataSettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit ParticleDataSettingEditWidget(QWidget *parent);
	~ParticleDataSettingEditWidget();

	void setValueNames(const std::unordered_map<std::string, QString>& names);
	void setColorMapSettings(const std::unordered_map<std::string, ColorMapSettingContainer*>& settings);
	void setSetting(ParticleDataSetting* setting);

	QUndoCommand* createModifyCommand(bool apply) override;

private slots:
	void colorTargetChanged(int index);

private:
	ParticleDataSetting setting() const;
	void setSetting(const ParticleDataSetting& setting);
	std::string colorTarget() const;

	ParticleDataSetting* m_setting;
	std::unordered_map<std::string, ColorMapSettingContainer*> m_colorMapSettings;
	std::vector<std::string> m_colorTargets;
	ColorMapSettingEditWidget* m_colorMapWidget;

	Ui::ParticleDataSettingEditWidget *ui;

	class ModifyCommand;
};

#endif // PARTICLEDATASETTINGEDITWIDGET_H
