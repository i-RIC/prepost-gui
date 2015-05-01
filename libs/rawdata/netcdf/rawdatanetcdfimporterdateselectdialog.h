#ifndef RAWDATANETCDFIMPORTERDATESELECTDIALOG_H
#define RAWDATANETCDFIMPORTERDATESELECTDIALOG_H

#include <QDialog>

namespace Ui
{
	class RawDataNetcdfImporterDateSelectDialog;
}

class RawDataNetcdfImporterDateSelectDialog : public QDialog
{
	Q_OBJECT

public:
	explicit RawDataNetcdfImporterDateSelectDialog(QWidget* parent = 0);
	~RawDataNetcdfImporterDateSelectDialog();
	void setUnit(const QString& unit);
	void setOriginalDateTime(const QDateTime& orig);
	QDateTime originalDateTime() const;

private:
	Ui::RawDataNetcdfImporterDateSelectDialog* ui;
};

#endif // RAWDATANETCDFIMPORTERDATESELECTDIALOG_H
