#ifndef POST2DWINDOWPARTICLEUNSTRUCTUREDSETTINGDIALOG_H
#define POST2DWINDOWPARTICLEUNSTRUCTUREDSETTINGDIALOG_H

#include <QDialog>
#include <QList>
#include <QPointF>
#include "../post2dwindowdataitem.h"
#include "post2dwindownodevectorparticlegroupunstructureddataitem.h"

namespace Ui
{
	class Post2dWindowParticleUnstructuredSettingDialog;
}

class QAbstractButton;
class PostZoneDataContainer;

class Post2dWindowParticleUnstructuredSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Post2dWindowParticleUnstructuredSettingDialog(QWidget* parent = nullptr);
	~Post2dWindowParticleUnstructuredSettingDialog();
	void setDataItem(Post2dWindowNodeVectorParticleGroupUnstructuredDataItem* item) {
		m_dataItem = item;
	}
	void setZoneData(PostZoneDataContainer* data);
	void setActiveAvailable(bool available) {m_activeAvailable = available;}

//	void setSolution(const QString& sol);
//	void setTimeMode(Post2dWindowNodeVectorParticleGroupDataItem::TimeMode tm);
//	void setTimeSamplingRate(int sr);
//	void setTimeDivision(int sd);
//	void setRegionMode(StructuredGridRegion::RegionMode mode) {m_regionMode = mode;}
	void setSettings(const Post2dWindowNodeVectorParticleGroupDataItem::Setting& s, const QList<Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting> & unsts);
//	void setSettings(const QList<Post2dWindowUnstructuredParticleSetSetting>& settings) {
//		m_settings = settings;
//		setupSettingList();
//	}
//	QString solution() const;
//	Post2dWindowNodeVectorParticleGroupDataItem::TimeMode timeMode() const;
//	int timeSamplingRate() const;
//	int timeDivision() const;
//	StructuredGridRegion::RegionMode regionMode() const {return m_regionMode;}
	Post2dWindowNodeVectorParticleGroupDataItem::Setting setting() const;
	const QList<Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting>& unstSettings() const {return m_unstSettings;}

	void informButtonDown(const QPointF& p);
	void informButtonUp(const QPointF& p);

public slots:
	void updateMousePosition(const QVector2D &p);
	void accept() override;
	void reject() override;

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

	Ui::Post2dWindowParticleUnstructuredSettingDialog* ui;
	Post2dWindowNodeVectorParticleGroupUnstructuredDataItem* m_dataItem;
	QList<QString> m_solutions;

	QPointF m_oldPoint1;
	QPointF m_oldPoint2;
	int m_oldPoints;
	int m_oldPointSize;
	bool m_pressed;
	bool m_applying;

	Post2dWindowNodeVectorParticleGroupDataItem::Setting m_setting;
	QList<Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting> m_unstSettings;
	Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting* m_activeSetting;
	bool m_activeAvailable;

	QList<int> m_subDivNominations;
	QList<int> m_skipNominations;
};

#endif // POST2DWINDOWPARTICLEUNSTRUCTUREDSETTINGDIALOG_H
