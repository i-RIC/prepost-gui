#ifndef GEODATAPOINTMAPMERGESETTINGDIALOG_H
#define GEODATAPOINTMAPMERGESETTINGDIALOG_H

#include <QDialog>

#include <vector>

namespace Ui {
class GeoDataPointmapMergeSettingDialog;
}

class PreProcessorGeoDataDataItemInterface;

class GeoDataPointmapMergeSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataPointmapMergeSettingDialog(QWidget *parent = 0);
	~GeoDataPointmapMergeSettingDialog();

	void setItems(const std::vector<PreProcessorGeoDataDataItemInterface*>& items);
	std::vector<PreProcessorGeoDataDataItemInterface*> selectedItems() const;

private:
	std::vector<PreProcessorGeoDataDataItemInterface*> m_items;

	Ui::GeoDataPointmapMergeSettingDialog *ui;
};

#endif // GEODATAPOINTMAPMERGESETTINGDIALOG_H
