#ifndef POST3DWINDOWSTREAMLINESTRUCTUREDSETTINGDIALOG_H
#define POST3DWINDOWSTREAMLINESTRUCTUREDSETTINGDIALOG_H

#include <QDialog>
#include <QList>
#include "../post3dwindowdataitem.h"
#include "post3dwindownodevectorstreamlinegroupstructureddataitem.h"

namespace Ui
{
	class Post3dWindowStreamlineStructuredSettingDialog;
}
class PostZoneDataContainer;

class Post3dWindowStreamlineStructuredSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Post3dWindowStreamlineStructuredSettingDialog(QWidget* parent = nullptr);
	~Post3dWindowStreamlineStructuredSettingDialog();
	void setZoneData(PostZoneDataContainer* data);
	void setActiveAvailable(bool available) {m_activeAvailable = available;}

	void setSolution(const QString& sol);
	void setRegionMode(StructuredGridRegion::RegionMode mode) {m_regionMode = mode;}
	void setSettings(const QList<Post3dWindowStructuredStreamlineSetSetting>& settings) {
		m_settings = settings;
		setupSettingList();
	}
	QString solution() const;
	StructuredGridRegion::RegionMode regionMode() const {return m_regionMode;}
	const QList<Post3dWindowStructuredStreamlineSetSetting>& settings() const {return m_settings;}

private slots:
	void activeDataChanged(int index);
	void iMinChanged(int min);
	void iMaxChanged(int max);
	void jMinChanged(int min);
	void jMaxChanged(int max);
	void kMinChanged(int min);
	void kMaxChanged(int max);
	void handleSpaceSliderChange(int val);
	void colorChanged(const QColor& color);
	void widthChanged(int size);

	void addData();
	void removeData();

private:
	void setupNominations();
	void setupSolutionComboBox(PostZoneDataContainer* zoneData);
	void setupSettingList();
	void applySettings();
	void updateRemoveButtonStatus();

	Ui::Post3dWindowStreamlineStructuredSettingDialog* ui;
	QList<QString> m_solutions;

	QList<Post3dWindowStructuredStreamlineSetSetting> m_settings;
	Post3dWindowStructuredStreamlineSetSetting* m_activeSetting;
	bool m_activeAvailable;

	QList<int> m_subDivNominations;
	QList<int> m_skipNominations;

	StructuredGridRegion::RegionMode m_regionMode;
	bool m_applying;
};

#endif // POST3DWINDOWSTREAMLINESTRUCTUREDSETTINGDIALOG_H
