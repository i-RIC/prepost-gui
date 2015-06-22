#include "ui_geodatapointmapdelptslessthandialog.h"

#include "geodatapointmap.h"
#include "geodatapointmapdelptslessthandialog.h"

#include <vtkPointData.h>

GeoDataPointMapDelPtsLessThanDialog::GeoDataPointMapDelPtsLessThanDialog(GeoDataPointmap* pmap, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GeoDataPointMapDelPtsLessThanDialog)
{
	ui->setupUi(this);

	m_pointMap = pmap;
	QVector<vtkIdType> selectedVertices = m_pointMap->selectedVertices();
	vtkDataArray* vArr = m_pointMap->vtkGrid()->GetPointData()->GetArray("values");

	double min = 0;
	double max = 0;

	for (vtkIdType i = 0; i < selectedVertices.count(); ++i) {
		double val = vArr->GetTuple1(selectedVertices[i]);
		if (i == 0 || val < min) {min = val;}
		if (i == 0 || val > max) {max = val;}
	}
	ui->maxPointValue->setValue(max);
	ui->minPointValue->setValue(min);
	ui->deleteLessThanValue->setValue(max);
	ui->numPoints->setText(QString::number(selectedVertices.count()));
}

GeoDataPointMapDelPtsLessThanDialog::~GeoDataPointMapDelPtsLessThanDialog()
{
	delete ui;
}

void GeoDataPointMapDelPtsLessThanDialog::accept()
{
	if (ui->deleteLessThanValue->text() == "") {
		m_selectedPoints = m_pointMap->selectedVertices();
		return;
	}
	m_selectedPoints.clear();
	double limitVal = ui->deleteLessThanValue->value();

	QVector<vtkIdType> selectedVertices = m_pointMap->selectedVertices();
	vtkDataArray* vArr = m_pointMap->vtkGrid()->GetPointData()->GetArray("values");

	for (vtkIdType i = 0; i < selectedVertices.count(); ++i) {
		double val = vArr->GetTuple1(selectedVertices[i]);
		if (val <= limitVal) {
			m_selectedPoints.append(selectedVertices[i]);
		}
	}
	QDialog::accept();
}

double GeoDataPointMapDelPtsLessThanDialog::limitValue()
{
	return ui->deleteLessThanValue->value();
}
