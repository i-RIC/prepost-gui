#ifndef POST2DWINDOWSTREAMLINESTRUCTUREDSETTINGDIALOG_H
#define POST2DWINDOWSTREAMLINESTRUCTUREDSETTINGDIALOG_H

#include <QDialog>
#include <QList>
#include "../post2dwindowdataitem.h"
#include "post2dwindownodevectorstreamlinegroupstructureddataitem.h"

namespace Ui {
	class Post2dWindowStreamlineStructuredSettingDialog;
}
class PostZoneDataContainer;

class Post2dWindowStreamlineStructuredSettingDialog : public QDialog
{
    Q_OBJECT
public:
	explicit Post2dWindowStreamlineStructuredSettingDialog(QWidget *parent = 0);
	~Post2dWindowStreamlineStructuredSettingDialog();
	void setZoneData(PostZoneDataContainer* data);
	void setActiveAvailable(bool available){m_activeAvailable = available;}

	void setSolution(const QString& sol);
	void setRegionMode(StructuredGridRegion::RegionMode mode){m_regionMode = mode;}
	void setSettings(const QList<Post2dWindowStructuredStreamlineSetSetting>& settings){
		m_settings = settings;
		setupSettingList();
	}
	const QString solution();
	StructuredGridRegion::RegionMode regionMode(){return m_regionMode;}
	const QList<Post2dWindowStructuredStreamlineSetSetting>& settings(){return m_settings;}

private slots:
	void activeDataChanged(int index);
	void iMinChanged(int min);
	void iMaxChanged(int max);
	void jMinChanged(int min);
	void jMaxChanged(int max);
	void handleSpaceSliderChange(int val);
	void colorChanged(const QColor& color);
	void widthChanged(int size);

	void addData();
	void removeData();
	void showRegionDialog();

private:
	void setupNominations();
	void setupSolutionComboBox(PostZoneDataContainer* zoneData);
	void setupSettingList();
	void applySettings();
	void updateRemoveButtonStatus();

	Ui::Post2dWindowStreamlineStructuredSettingDialog *ui;
	QList<QString> m_solutions;

	QList<Post2dWindowStructuredStreamlineSetSetting> m_settings;
	Post2dWindowStructuredStreamlineSetSetting* m_activeSetting;
	bool m_activeAvailable;

	QList<int> m_subDivNominations;
	QList<int> m_skipNominations;

	StructuredGridRegion::RegionMode m_regionMode;
	bool m_applying;
};

#endif // POST2DWINDOWSTREAMLINESTRUCTUREDSETTINGDIALOG_H
