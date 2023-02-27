#ifndef PREPROCESSORGEODATAGROUPDATAITEM_VARIATIONSETTINGDIALOG_H
#define PREPROCESSORGEODATAGROUPDATAITEM_VARIATIONSETTINGDIALOG_H

#include "../preprocessorgeodatagroupdataitem.h"

#include <QDialog>

#include <vector>

namespace Ui {
class PreProcessorGeoDataGroupDataItem_VariationSettingDialog;
}

class PreProcessorGeoDataGroupDataItem::VariationSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit VariationSettingDialog(QWidget *parent = nullptr);
	~VariationSettingDialog();

	void setVariations(const std::map<std::string, QString> variations);

	VariationSetting setting() const;
	void setSetting(const VariationSetting& setting);

private:
	std::vector<std::string> m_variations;

	Ui::PreProcessorGeoDataGroupDataItem_VariationSettingDialog *ui;
};

#endif // PREPROCESSORGEODATAGROUPDATAITEM_VARIATIONSETTINGDIALOG_H
