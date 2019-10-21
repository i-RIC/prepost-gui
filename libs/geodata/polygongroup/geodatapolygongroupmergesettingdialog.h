#ifndef GEODATAPOLYGONGROUPMERGESETTINGDIALOG_H
#define GEODATAPOLYGONGROUPMERGESETTINGDIALOG_H

#include <QDialog>

#include <vector>

namespace Ui {
class GeoDataPolygonGroupMergeSettingDialog;
}

class PreProcessorGeoDataDataItemInterface;

class GeoDataPolygonGroupMergeSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataPolygonGroupMergeSettingDialog(QWidget *parent = 0);
	~GeoDataPolygonGroupMergeSettingDialog();

	void setItems(const std::vector<PreProcessorGeoDataDataItemInterface*>& items);
	std::vector<PreProcessorGeoDataDataItemInterface*> selectedItems() const;

private:
	std::vector<PreProcessorGeoDataDataItemInterface*> m_items;

	Ui::GeoDataPolygonGroupMergeSettingDialog *ui;
};

#endif // GEODATAPOLYGONGROUPMERGESETTINGDIALOG_H
