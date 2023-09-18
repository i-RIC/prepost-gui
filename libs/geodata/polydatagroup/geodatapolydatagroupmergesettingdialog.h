#ifndef GEODATAPOLYDATAGROUPMERGESETTINGDIALOG_H
#define GEODATAPOLYDATAGROUPMERGESETTINGDIALOG_H

#include <QDialog>

#include <vector>

namespace Ui {
class GeoDataPolyDataGroupMergeSettingDialog;
}

class PreProcessorGeoDataDataItemI;

class GeoDataPolyDataGroupMergeSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataPolyDataGroupMergeSettingDialog(QWidget *parent = 0);
	~GeoDataPolyDataGroupMergeSettingDialog();

	void setItems(const std::vector<PreProcessorGeoDataDataItemI*>& items);
	std::vector<PreProcessorGeoDataDataItemI*> selectedItems() const;

private:
	std::vector<PreProcessorGeoDataDataItemI*> m_items;

	Ui::GeoDataPolyDataGroupMergeSettingDialog *ui;
};

#endif // GEODATAPOLYDATAGROUPMERGESETTINGDIALOG_H
