#ifndef RAWDATAPOINTMAPREALIMPORTERFILTERDIALOG_H
#define RAWDATAPOINTMAPREALIMPORTERFILTERDIALOG_H

#include <QDialog>

namespace Ui
{
	class RawDataPointmapRealImporterFilterDialog;
}

class RawDataPointmapRealImporterFilterDialog : public QDialog
{
	Q_OBJECT

public:
	explicit RawDataPointmapRealImporterFilterDialog(QWidget* parent = nullptr);
	~RawDataPointmapRealImporterFilterDialog();
	int filterValue();

private:
	Ui::RawDataPointmapRealImporterFilterDialog* ui;
};

#endif // RAWDATAPOINTMAPREALIMPORTERFILTERDIALOG_H
