#ifndef GEODATAPOINTMAPMERGESETTINGDIALOG_H
#define GEODATAPOINTMAPMERGESETTINGDIALOG_H

#include <QDialog>

#include <vector>

namespace Ui {
class GeoDataPointmapMergeSettingDialog;
}

class PreProcessorGeoDataDataItemI;

class GeoDataPointmapMergeSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataPointmapMergeSettingDialog(QWidget *parent = 0);
	~GeoDataPointmapMergeSettingDialog();

	void setItems(const std::vector<PreProcessorGeoDataDataItemI*>& items);
	std::vector<PreProcessorGeoDataDataItemI*> selectedItems() const;

private:
	std::vector<PreProcessorGeoDataDataItemI*> m_items;

	Ui::GeoDataPointmapMergeSettingDialog *ui;
};

#endif // GEODATAPOINTMAPMERGESETTINGDIALOG_H
