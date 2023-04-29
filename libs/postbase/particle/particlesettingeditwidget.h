#ifndef PARTICLESETTINGEDITWIDGET_H
#define PARTICLESETTINGEDITWIDGET_H

#include "../postbase_global.h"

#include "particlesettingcontainer.h"

#include <misc/modifycommandwidget.h>

#include <QWidget>

#include <unordered_map>

namespace Ui {
class ParticleSettingEditWidget;
}

class ParticleSettingContainer;
class ProjectMainFile;

class POSTBASEDLL_EXPORT ParticleSettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit ParticleSettingEditWidget(QWidget *parent = nullptr);
	~ParticleSettingEditWidget();

	void setSolutions(const std::unordered_map<std::string, QString>& solutions);
	void setProjectMainFile(ProjectMainFile* file);

	QUndoCommand* createModifyCommand(bool apply) override;

	ParticleSettingContainer setting() const;
	void setSetting(const ParticleSettingContainer& setting);
	void setSetting(ParticleSettingContainer* setting);

	void setupNominations(int max);

private slots:
	void handleTimeSliderChange(int val);
	void editArbitraryTimes();

private:
	std::vector<std::string> m_solutions;
	std::vector<int> m_arbitraryTimes;

	ParticleSettingContainer* m_setting;
	ProjectMainFile* m_mainFile;

	std::vector<int> m_subDivideNominations;
	std::vector<int> m_skipNominations;

	Ui::ParticleSettingEditWidget *ui;
};

#endif // PARTICLESETTINGEDITWIDGET_H
