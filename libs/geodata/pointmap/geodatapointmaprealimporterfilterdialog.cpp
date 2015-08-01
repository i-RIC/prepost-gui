#include "ui_geodatapointmaprealimporterfilterdialog.h"

#include "geodatapointmaprealimporterfilterdialog.h"

GeoDataPointmapRealImporterFilterDialog::GeoDataPointmapRealImporterFilterDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GeoDataPointmapRealImporterFilterDialog)
{
	ui->setupUi(this);
}

GeoDataPointmapRealImporterFilterDialog::~GeoDataPointmapRealImporterFilterDialog()
{
	delete ui;
}

int GeoDataPointmapRealImporterFilterDialog::filterValue()
{
	return ui->filterSpinBox->value();
}
