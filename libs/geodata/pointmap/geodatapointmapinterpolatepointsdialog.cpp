#include "ui_geodatapointmapinterpolatepointsdialog.h"

#include "geodatapointmap.h"
#include "geodatapointmapinterpolatepointsdialog.h"

#include <QMessageBox>

GeoDataPointmapInterpolatePointsDialog::GeoDataPointmapInterpolatePointsDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GeoDataPointmapInterpolatePoints)
{
	ui->setupUi(this);
}

GeoDataPointmapInterpolatePointsDialog::~GeoDataPointmapInterpolatePointsDialog()
{
	delete ui;
}

void GeoDataPointmapInterpolatePointsDialog::setLineLength(double length)
{
	m_lineLength = length;
	ui->lineLengthValueLabel->setText(QString::number(length, 'g', 3));
}

double GeoDataPointmapInterpolatePointsDialog::increment() const
{
	return ui->incrementEditWidget->value();
}

void GeoDataPointmapInterpolatePointsDialog::accept()
{
	if (ui->incrementEditWidget->value() <= 0 || ui->incrementEditWidget->value() >= m_lineLength) {
		QMessageBox::warning(this, tr("Warning"), tr("Interpolation increment must be greater 0 and less than interpolation line length"));
		return;
	}
	QDialog::accept();
}
