#ifndef GEODATAPOINTMAP_MAPPINGSETTINGDIALOG_H
#define GEODATAPOINTMAP_MAPPINGSETTINGDIALOG_H

#include "../geodatapointmap.h"
#include "geodatapointmap_mappingsetting.h"

#include <QDialog>

namespace Ui {
class GeoDataPointmap_MappingSettingDialog;
}

class GeoDataPointmap::MappingSettingDialog : public QDialog
{
	Q_OBJECT

public:
	MappingSettingDialog(QWidget *parent = nullptr);
	~MappingSettingDialog();

	MappingSetting setting() const;
	void setSetting(const MappingSetting &setting);

private slots:
	void openDetailDialog();

private:
	MappingSetting m_setting;

	class TemplateDetailDialog;
	Ui::GeoDataPointmap_MappingSettingDialog *ui;
};

#endif // GEODATAPOINTMAP_MAPPINGSETTINGDIALOG_H
