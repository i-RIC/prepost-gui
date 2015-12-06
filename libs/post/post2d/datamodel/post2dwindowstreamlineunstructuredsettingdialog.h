#ifndef POST2DWINDOWSTREAMLINEUNSTRUCTUREDSETTINGDIALOG_H
#define POST2DWINDOWSTREAMLINEUNSTRUCTUREDSETTINGDIALOG_H

#include <QDialog>
#include <QPointF>
#include "../post2dwindowdataitem.h"
#include "post2dwindownodevectorstreamlinegroupunstructureddataitem.h"

#include <string>
#include <vector>

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

	void setSettings(const Post2dWindowNodeVectorStreamlineGroupDataItem::Setting& s, const std::vector<Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting>& unsts);
	Post2dWindowNodeVectorStreamlineGroupDataItem::Setting setting() const;
	const std::vector<Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting>& settings() const {return m_unstSettings;}

	void informButtonDown(const QPointF &p);
	void informButtonUp(const QPointF &p);

public slots:
	void updateMousePosition(const QVector2D &p);
	void accept() override;
	void reject() override;

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
	std::vector<std::string> m_solutions;
	bool m_isPointsSet;

	QPointF m_oldPoint1;
	QPointF m_oldPoint2;
	int m_oldPoints;
	bool m_pressed;
	bool m_applying;

	Post2dWindowNodeVectorStreamlineGroupDataItem::Setting m_setting;
	std::vector<Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting> m_unstSettings;
	Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting* m_activeSetting;
	bool m_activeAvailable;

	QList<int> m_subDivNominations;
	QList<int> m_skipNominations;

	StructuredGridRegion::RegionMode m_regionMode;
};

#endif // POST2DWINDOWSTREAMLINEUNSTRUCTUREDSETTINGDIALOG_H
