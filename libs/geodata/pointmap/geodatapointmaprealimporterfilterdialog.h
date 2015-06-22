#ifndef GEODATAPOINTMAPREALIMPORTERFILTERDIALOG_H
#define GEODATAPOINTMAPREALIMPORTERFILTERDIALOG_H

#include <QDialog>

namespace Ui
{
	class GeoDataPointmapRealImporterFilterDialog;
}

class GeoDataPointmapRealImporterFilterDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataPointmapRealImporterFilterDialog(QWidget* parent = nullptr);
	~GeoDataPointmapRealImporterFilterDialog();
	int filterValue();

private:
	Ui::GeoDataPointmapRealImporterFilterDialog* ui;
};

#endif // GEODATAPOINTMAPREALIMPORTERFILTERDIALOG_H
