#ifndef PREPROCESSORGEODATADATAITEM_IMPORTSETTINGDIALOG_H
#define PREPROCESSORGEODATADATAITEM_IMPORTSETTINGDIALOG_H

#include "../preprocessorgeodatadataitem.h"

#include <guicore/pre/geodata/geodataimportersetting.h>

#include <QDialog>

namespace Ui {
class PreProcessorGeoDataDataItem_ImportSettingDialog;
}

class PreProcessorGeoDataDataItem::ImportSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ImportSettingDialog(QWidget *parent = nullptr);
	~ImportSettingDialog();

	void setItems(const std::vector<GeoDataImporterSetting::Item>& items);

private:
	Ui::PreProcessorGeoDataDataItem_ImportSettingDialog *ui;
};

#endif // PREPROCESSORGEODATADATAITEM_IMPORTSETTINGDIALOG_H
