#ifndef POST2DWINDOWSTREAMLINEUNSTRUCTUREDSETTINGDIALOG_H
#define POST2DWINDOWSTREAMLINEUNSTRUCTUREDSETTINGDIALOG_H

#include <QDialog>
#include <QList>
#include <QVector2D>
#include "../post2dwindowdataitem.h"
#include "post2dwindownodevectorstreamlinegroupunstructureddataitem.h"

namespace Ui
{
	class Post2dWindowStreamlineUnstructuredSettingDialog;
}

class QAbstractButton;
class PostZoneDataContainer;
class Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem;

class Post2dWindowStreamlineUnstructuredSettingDialog : public QDialog
{
	Q_OBJECT
public:
	explicit Post2dWindowStreamlineUnstructuredSettingDialog(QWidget* parent = nullptr);
	~Post2dWindowStreamlineUnstructuredSettingDialog();
	void setDataItem(Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem* item) {
		m_dataItem = item;
	}
	void setZoneData(PostZoneDataContainer* data);
	void setActiveAvailable(bool available) {m_activeAvailable = available;}

	void setSolution(const QString& sol);
	void setRegionMode(StructuredGridRegion::RegionMode mode) {m_regionMode = mode;}
	void setSettings(const QList<Post2dWindowUnstructuredStreamlineSetSetting>& settings) {
		m_settings = settings;
		setupSettingList();
	}
	const QString solution() const;
	StructuredGridRegion::RegionMode regionMode() const {return m_regionMode;}
	const QList<Post2dWindowUnstructuredStreamlineSetSetting>& settings() const {return m_settings;}

	void informButtonDown(const QVector2D& p);
	void informButtonUp(const QVector2D& p);
public slots:
	void updateMousePosition(const QVector2D& p);
	void accept();
	void reject();
private slots:
	void activeDataChanged(int index);
	void pointsEdited();
	void handleButtonPress(QAbstractButton* button);
	void numPointsChanged(int num);
	void colorChanged(const QColor& color);
	void widthChanged(int width);

	void addData();
	void removeData();
	void showRegionDialog();
private:
	void apply();
	void setupSolutionComboBox(PostZoneDataContainer* zoneData);
	void setupSettingList();
	void applySettings();
	void updateRemoveButtonStatus();

	Ui::Post2dWindowStreamlineUnstructuredSettingDialog* ui;
	Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem* m_dataItem;
	QList<QString> m_solutions;
	bool m_isPointsSet;

	QVector2D m_oldPoint1;
	QVector2D m_oldPoint2;
	int m_oldPoints;
	bool m_pressed;
	bool m_applying;

	QList<Post2dWindowUnstructuredStreamlineSetSetting> m_settings;
	Post2dWindowUnstructuredStreamlineSetSetting* m_activeSetting;
	bool m_activeAvailable;

	QList<int> m_subDivNominations;
	QList<int> m_skipNominations;

	StructuredGridRegion::RegionMode m_regionMode;
};

#endif // POST2DWINDOWSTREAMLINEUNSTRUCTUREDSETTINGDIALOG_H
