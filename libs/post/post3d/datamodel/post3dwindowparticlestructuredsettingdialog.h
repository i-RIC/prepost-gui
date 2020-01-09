#ifndef POST3DWINDOWPARTICLESTRUCTUREDSETTINGDIALOG_H
#define POST3DWINDOWPARTICLESTRUCTUREDSETTINGDIALOG_H

#include <QDialog>
#include <QList>
#include "../post3dwindowdataitem.h"
#include "post3dwindownodevectorparticlegroupstructureddataitem.h"

#include <vector>

namespace Ui
{
	class Post3dWindowParticleStructuredSettingDialog;
}
class PostZoneDataContainer;
class ProjectMainFile;

class Post3dWindowParticleStructuredSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Post3dWindowParticleStructuredSettingDialog(QWidget* parent = nullptr);
	~Post3dWindowParticleStructuredSettingDialog();

	void setProjectMainFile(ProjectMainFile* file);
	void setZoneData(PostZoneDataContainer* data);
	void setActiveAvailable(bool available);

	void setSetting(const Post3dWindowNodeVectorParticleGroupStructuredDataItem::Setting& setting);
	void setSettings(const QList<Post3dWindowStructuredParticleSetSetting>& settings);

	Post3dWindowNodeVectorParticleGroupStructuredDataItem::Setting setting() const;
	const QList<Post3dWindowStructuredParticleSetSetting>& settings() const;

private slots:
	void activeDataChanged(int index);

	void iMinChanged(int min);
	void iMaxChanged(int max);
	void jMinChanged(int min);
	void jMaxChanged(int max);
	void kMinChanged(int min);
	void kMaxChanged(int max);
	void handleTimeSliderChange(int val);
	void handleSpaceSliderChange(int val);
	void colorChanged(const QColor& color);
	void sizeChanged(int size);

	void addData();
	void removeData();
	void editArbitraryTimes();

private:
	void setupNominations();
	void setupSolutionComboBox(PostZoneDataContainer* zoneData);
	void setupSettingList();
	void applySettings();
	void updateRemoveButtonStatus();

	Ui::Post3dWindowParticleStructuredSettingDialog* ui;
	std::vector<std::string> m_solutions;

	Post3dWindowNodeVectorParticleGroupStructuredDataItem::Setting m_setting;
	QList<Post3dWindowStructuredParticleSetSetting> m_settings;
	Post3dWindowStructuredParticleSetSetting* m_activeSetting;
	bool m_activeAvailable;
	bool m_applying;

	QList<int> m_subDivNominations;
	QList<int> m_skipNominations;

	ProjectMainFile* m_mainFile;
};

#endif // POST3DWINDOWPARTICLESTRUCTUREDSETTINGDIALOG_H
