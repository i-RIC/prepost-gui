#ifndef POST2DWINDOWPARTICLESTRUCTUREDSETTINGDIALOG_H
#define POST2DWINDOWPARTICLESTRUCTUREDSETTINGDIALOG_H

#include <QDialog>
#include <QList>
#include "../post2dwindowdataitem.h"
#include "post2dwindownodevectorparticlegroupstructureddataitem.h"

#include <string>
#include <vector>

namespace Ui
{
	class Post2dWindowParticleStructuredSettingDialog;
}

class PostZoneDataContainer;
class ProjectMainFile;

class Post2dWindowParticleStructuredSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Post2dWindowParticleStructuredSettingDialog(QWidget* parent = nullptr);
	~Post2dWindowParticleStructuredSettingDialog();
	void setProjectMainFile(ProjectMainFile* file);
	void setZoneData(PostZoneDataContainer* data);
	void setActiveAvailable(bool available) {m_activeAvailable = available;}

	void setSettings(const Post2dWindowNodeVectorParticleGroupDataItem::Setting& s, const QList<Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting>& sts);
	Post2dWindowNodeVectorParticleGroupDataItem::Setting setting() const;
	const QList<Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting>& stSettings() const {return m_stSettings;}

private slots:
	void activeDataChanged(int index);

	void iMinChanged(int min);
	void iMaxChanged(int max);
	void jMinChanged(int min);
	void jMaxChanged(int max);
	void handleTimeSliderChange(int val);
	void handleSpaceSliderChange(int val);
	void colorChanged(const QColor& color);
	void sizeChanged(int size);

	void addData();
	void removeData();
	void showRegionDialog();
	void editArbitraryTimes();

private:
	void setupNominations();
	void setupSolutionComboBox(PostZoneDataContainer* zoneData);
	void setupSettingList();
	void applySettings();
	void updateRemoveButtonStatus();

	Ui::Post2dWindowParticleStructuredSettingDialog* ui;
	std::vector<std::string> m_solutions;

	Post2dWindowNodeVectorParticleGroupDataItem::Setting m_setting;
	QList<Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting> m_stSettings;
	Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting* m_activeSetting;
	bool m_activeAvailable;
	bool m_applying;

	QList<int> m_subDivNominations;
	QList<int> m_skipNominations;

	StructuredGridRegion::RegionMode m_regionMode;
	ProjectMainFile* m_mainFile;
};

#endif // POST2DWINDOWPARTICLESTRUCTUREDSETTINGDIALOG_H
