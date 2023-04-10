#ifndef GEODATAPOINTMAP_MAPPINGSETTINGDIALOG_TEMPLATEDETAILDIALOG_H
#define GEODATAPOINTMAP_MAPPINGSETTINGDIALOG_TEMPLATEDETAILDIALOG_H

#include "geodatapointmap_mappingsettingdialog.h"

#include <QDialog>

namespace Ui {
class GeoDataPointmap_MappingSettingDialog_TemplateDetailDialog;
}

class GeoDataPointmap::MappingSettingDialog::TemplateDetailDialog : public QDialog
{
	Q_OBJECT

public:
	explicit TemplateDetailDialog(QWidget *parent);
	~TemplateDetailDialog();

	MappingSetting setting() const;
	void setSetting(const MappingSetting& setting);

private:
	Ui::GeoDataPointmap_MappingSettingDialog_TemplateDetailDialog *ui;
};

#endif // GEODATAPOINTMAP_MAPPINGSETTINGDIALOG_TEMPLATEDETAILDIALOG_H
