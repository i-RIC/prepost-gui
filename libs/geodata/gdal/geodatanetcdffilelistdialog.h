#ifndef GEODATANETCDFFILELISTDIALOG_H
#define GEODATANETCDFFILELISTDIALOG_H

#include <QDialog>
#include <QString>

#include <vector>

namespace Ui {
class GeoDataNetcdfFileListDialog;
}

class GeoDataNetcdfFileNameMatcher;

class GeoDataNetcdfFileListDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataNetcdfFileListDialog(QWidget *parent = 0);
	~GeoDataNetcdfFileListDialog();

	void setFileNameMatcher(GeoDataNetcdfFileNameMatcher* matcher);
	void setFileNames(const std::vector<QString>& fileNames);

	std::vector<QString> selectedFilenames() const;

public slots:
	void accept() override;

private slots:
	void selectAll();
	void deselectAll();

private:
	GeoDataNetcdfFileNameMatcher* m_fileNameMatcher;
	std::vector<QString> m_fileNames;

	Ui::GeoDataNetcdfFileListDialog *ui;
};

#endif // GEODATANETCDFFILELISTDIALOG_H
