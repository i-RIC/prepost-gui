#ifndef GEODATAPOLYDATAGROUPMERGESETTINGDIALOG_H
#define GEODATAPOLYDATAGROUPMERGESETTINGDIALOG_H

#include <QDialog>

#include <vector>

namespace Ui {
class GeoDataPolyDataGroupMergeSettingDialog;
}

class PreProcessorGeoDataDataItemInterface;

class GeoDataPolyDataGroupMergeSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataPolyDataGroupMergeSettingDialog(QWidget *parent = 0);
	~GeoDataPolyDataGroupMergeSettingDialog();

	void setItems(const std::vector<PreProcessorGeoDataDataItemInterface*>& items);
	std::vector<PreProcessorGeoDataDataItemInterface*> selectedItems() const;

private:
	std::vector<PreProcessorGeoDataDataItemInterface*> m_items;

	Ui::GeoDataPolyDataGroupMergeSettingDialog *ui;
};

#endif // GEODATAPOLYDATAGROUPMERGESETTINGDIALOG_H
