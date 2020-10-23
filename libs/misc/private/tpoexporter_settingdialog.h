#ifndef TPOEXPORTER_SETTINGDIALOG_H
#define TPOEXPORTER_SETTINGDIALOG_H

#include "../tpoexporter.h"

#include <QDialog>

namespace Ui {
class TpoExporter_SettingDialog;
}

class TpoExporter::SettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SettingDialog(QWidget *parent);
	~SettingDialog();

	int xPrecision() const;
	int yPrecision() const;
	int zPrecision() const;

private:
	Ui::TpoExporter_SettingDialog *ui;
};

#endif // TPOEXPORTER_SETTINGDIALOG_H
