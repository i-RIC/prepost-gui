#ifndef POST3DWINDOWPARTICLESTRUCTUREDSETTINGDIALOG_H
#define POST3DWINDOWPARTICLESTRUCTUREDSETTINGDIALOG_H

#include <QDialog>
#include <QList>
#include "../post3dwindowdataitem.h"
#include "post3dwindownodevectorparticlegroupstructureddataitem.h"

namespace Ui
{
	class Post3dWindowParticleStructuredSettingDialog;
}
class PostZoneDataContainer;

class Post3dWindowParticleStructuredSettingDialog : public QDialog
{
	Q_OBJECT
public:
	explicit Post3dWindowParticleStructuredSettingDialog(QWidget* parent = nullptr);
	~Post3dWindowParticleStructuredSettingDialog();
	void setZoneData(PostZoneDataContainer* data);
	void setActiveAvailable(bool available) {m_activeAvailable = available;}

	void setSolution(const QString& sol);
	void setTimeMode(Post3dWindowNodeVectorParticleGroupDataItem::TimeMode tm);
	void setTimeSamplingRate(int sr);
	void setTimeDivision(int sd);
	void setRegionMode(StructuredGridRegion::RegionMode mode) {m_regionMode = mode;}
	void setSettings(const QList<Post3dWindowStructuredParticleSetSetting>& settings) {
		m_settings = settings;
		setupSettingList();
	}
	const QString solution();
	Post3dWindowNodeVectorParticleGroupDataItem::TimeMode timeMode();
	int timeSamplingRate();
	int timeDivision();
	StructuredGridRegion::RegionMode regionMode() {return m_regionMode;}
	const QList<Post3dWindowStructuredParticleSetSetting>& settings() {return m_settings;}
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
private:
	void setupNominations();
	void setupSolutionComboBox(PostZoneDataContainer* zoneData);
	void setupSettingList();
	void applySettings();
	void updateRemoveButtonStatus();

	Ui::Post3dWindowParticleStructuredSettingDialog* ui;
	QList<QString> m_solutions;

	Post3dWindowNodeVectorParticleGroupDataItem::TimeMode m_timeMode;
	QList<Post3dWindowStructuredParticleSetSetting> m_settings;
	Post3dWindowStructuredParticleSetSetting* m_activeSetting;
	bool m_activeAvailable;
	bool m_applying;

	QList<int> m_subDivNominations;
	QList<int> m_skipNominations;

	StructuredGridRegion::RegionMode m_regionMode;
};

#endif // POST3DWINDOWPARTICLESTRUCTUREDSETTINGDIALOG_H
