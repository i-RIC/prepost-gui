#ifndef POST2DWINDOWPARTICLEUNSTRUCTUREDSETTINGDIALOG_H
#define POST2DWINDOWPARTICLEUNSTRUCTUREDSETTINGDIALOG_H

#include <QDialog>
#include <QList>
#include <QVector2D>
#include "../post2dwindowdataitem.h"
#include "post2dwindownodevectorparticlegroupunstructureddataitem.h"

namespace Ui {
	class Post2dWindowParticleUnstructuredSettingDialog;
}

class QAbstractButton;
class PostZoneDataContainer;
class Post2dWindowNodeVectorParticleGroupUnstructuredDataItem;

class Post2dWindowParticleUnstructuredSettingDialog : public QDialog
{
    Q_OBJECT
public:
	explicit Post2dWindowParticleUnstructuredSettingDialog(QWidget *parent = 0);
	~Post2dWindowParticleUnstructuredSettingDialog();
	void setDataItem(Post2dWindowNodeVectorParticleGroupUnstructuredDataItem* item){
		m_dataItem = item;
	}
	void setZoneData(PostZoneDataContainer* data);
	void setActiveAvailable(bool available){m_activeAvailable = available;}

	void setSolution(const QString& sol);
	void setTimeMode(Post2dWindowNodeVectorParticleGroupDataItem::TimeMode tm);
	void setTimeSamplingRate(int sr);
	void setTimeDivision(int sd);
	void setRegionMode(StructuredGridRegion::RegionMode mode){m_regionMode = mode;}
	void setSettings(const QList<Post2dWindowUnstructuredParticleSetSetting>& settings){
		m_settings = settings;
		setupSettingList();
	}
	const QString solution();
	Post2dWindowNodeVectorParticleGroupDataItem::TimeMode timeMode();
	int timeSamplingRate();
	int timeDivision();
	StructuredGridRegion::RegionMode regionMode(){return m_regionMode;}
	const QList<Post2dWindowUnstructuredParticleSetSetting>& settings(){return m_settings;}

	void informButtonDown(const QVector2D& p);
	void informButtonUp(const QVector2D& p);

public slots:
	void updateMousePosition(const QVector2D& p);
	void accept();
	void reject();

private slots:
	void activeDataChanged(int index);
	void handleTimeSliderChange(int val);
	void pointsEdited();
	void handleButtonPress(QAbstractButton* button);
	void numPointsChanged(int num);
	void colorChanged(const QColor& color);
	void sizeChanged(int size);

	void addData();
	void removeData();
	void showRegionDialog();

private:
	void apply();
	void setupNominations();
	void setupSolutionComboBox(PostZoneDataContainer* zoneData);
	void setupSettingList();
	void applySettings();
	void updateRemoveButtonStatus();

	Ui::Post2dWindowParticleUnstructuredSettingDialog *ui;
	Post2dWindowNodeVectorParticleGroupUnstructuredDataItem* m_dataItem;
	QList<QString> m_solutions;

	QVector2D m_oldPoint1;
	QVector2D m_oldPoint2;
	int m_oldPoints;
	int m_oldPointSize;
	bool m_pressed;
	bool m_applying;

	Post2dWindowNodeVectorParticleGroupDataItem::TimeMode m_timeMode;
	QList<Post2dWindowUnstructuredParticleSetSetting> m_settings;
	Post2dWindowUnstructuredParticleSetSetting* m_activeSetting;
	bool m_activeAvailable;

	QList<int> m_subDivNominations;
	QList<int> m_skipNominations;

	StructuredGridRegion::RegionMode m_regionMode;
};

#endif // POST2DWINDOWPARTICLEUNSTRUCTUREDSETTINGDIALOG_H
