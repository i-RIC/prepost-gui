#ifndef GEODATANETCDFFILELISTDIALOG_H
#define GEODATANETCDFFILELISTDIALOG_H

#include <QDialog>
#include <QString>

#include <vector>

namespace Ui {
class GeoDataNetcdfFileListDialog;
}

class GeoDataNetcdfFileListDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataNetcdfFileListDialog(QWidget *parent = 0);
	~GeoDataNetcdfFileListDialog();

	void setFilenames(const std::vector<QString>& filenames);

private:
	Ui::GeoDataNetcdfFileListDialog *ui;
};

#endif // GEODATANETCDFFILELISTDIALOG_H
