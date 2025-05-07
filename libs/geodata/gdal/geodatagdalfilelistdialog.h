#ifndef GEODATAGDALFILELISTDIALOG_H
#define GEODATAGDALFILELISTDIALOG_H

#include <QDialog>
#include <QString>

#include <vector>

namespace Ui {
class GeoDataGdalFileListDialog;
}

class GeoDataGdalFileNameMatcher;

class GeoDataGdalFileListDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataGdalFileListDialog(QWidget *parent = 0);
	~GeoDataGdalFileListDialog();

	void setFileNameMatcher(GeoDataGdalFileNameMatcher* matcher);
	void setFileNames(const std::vector<QString>& fileNames);

	std::vector<QString> selectedFilenames() const;

public slots:
	void accept() override;

private slots:
	void selectAll();
	void deselectAll();

private:
	GeoDataGdalFileNameMatcher* m_fileNameMatcher;
	std::vector<QString> m_fileNames;

	Ui::GeoDataGdalFileListDialog *ui;
};

#endif // GEODATAGDALFILELISTDIALOG_H
