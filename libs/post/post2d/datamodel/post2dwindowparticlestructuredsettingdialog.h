#ifndef POST2DWINDOWPARTICLESTRUCTUREDSETTINGDIALOG_H
#define POST2DWINDOWPARTICLESTRUCTUREDSETTINGDIALOG_H

#include <QDialog>
#include <QList>
#include "../post2dwindowdataitem.h"
#include "post2dwindownodevectorparticlegroupstructureddataitem.h"

namespace Ui {
    class Post2dWindowParticleStructuredSettingDialog;
}
class PostZoneDataContainer;

class Post2dWindowParticleStructuredSettingDialog : public QDialog
{
    Q_OBJECT
public:
	explicit Post2dWindowParticleStructuredSettingDialog(QWidget *parent = nullptr);
	~Post2dWindowParticleStructuredSettingDialog();
	void setZoneData(PostZoneDataContainer* data);
	void setActiveAvailable(bool available){m_activeAvailable = available;}

	void setSolution(const QString& sol);
	void setTimeMode(Post2dWindowNodeVectorParticleGroupDataItem::TimeMode tm);
	void setTimeSamplingRate(int sr);
	void setTimeDivision(int sd);
	void setRegionMode(StructuredGridRegion::RegionMode mode){m_regionMode = mode;}
	void setSettings(const QList<Post2dWindowStructuredParticleSetSetting>& settings){
		m_settings = settings;
		setupSettingList();
	}
	const QString solution();
	Post2dWindowNodeVectorParticleGroupDataItem::TimeMode timeMode();
	int timeSamplingRate();
	int timeDivision();
	StructuredGridRegion::RegionMode regionMode(){return m_regionMode;}
	const QList<Post2dWindowStructuredParticleSetSetting>& settings(){return m_settings;}
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
private:
	void setupNominations();
	void setupSolutionComboBox(PostZoneDataContainer* zoneData);
	void setupSettingList();
	void applySettings();
	void updateRemoveButtonStatus();

    Ui::Post2dWindowParticleStructuredSettingDialog *ui;
	QList<QString> m_solutions;

	Post2dWindowNodeVectorParticleGroupDataItem::TimeMode m_timeMode;
	QList<Post2dWindowStructuredParticleSetSetting> m_settings;
	Post2dWindowStructuredParticleSetSetting* m_activeSetting;
	bool m_activeAvailable;
	bool m_applying;

	QList<int> m_subDivNominations;
	QList<int> m_skipNominations;

	StructuredGridRegion::RegionMode m_regionMode;
};

#endif // POST2DWINDOWPARTICLESTRUCTUREDSETTINGDIALOG_H
