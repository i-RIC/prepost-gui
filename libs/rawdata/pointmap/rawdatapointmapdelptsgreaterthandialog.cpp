#include "ui_rawdatapointmapdelptsgreaterthandialog.h"

#include "rawdatapointmap.h"
#include "rawdatapointmapdelptsgreaterthandialog.h"

#include <vtkPointData.h>

RawDataPointmapDelPtsGreaterThanDialog::RawDataPointmapDelPtsGreaterThanDialog(RawDataPointmap* pmap, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::RawDataPointmapDelPtsGreaterThanDialog)
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
	ui->deleteGreaterThanValue->setValue(min);
	ui->numPoints->setText(QString::number(selectedVertices.count()));
}

RawDataPointmapDelPtsGreaterThanDialog::~RawDataPointmapDelPtsGreaterThanDialog()
{
	delete ui;
}

void RawDataPointmapDelPtsGreaterThanDialog::accept()
{
	if (ui->deleteGreaterThanValue->text() == "") {
		m_selectedPoints = m_pointMap->selectedVertices();
		return;
	}
	m_selectedPoints.clear();
	double limitVal = ui->deleteGreaterThanValue->value();

	QVector<vtkIdType> selectedVertices = m_pointMap->selectedVertices();
	vtkDataArray* vArr = m_pointMap->vtkGrid()->GetPointData()->GetArray("values");

	for (vtkIdType i = 0; i < selectedVertices.count(); ++i) {
		double val = vArr->GetTuple1(selectedVertices[i]);
		if (val >= limitVal) {
			m_selectedPoints.append(selectedVertices[i]);
		}
	}
	QDialog::accept();
}

double RawDataPointmapDelPtsGreaterThanDialog::limitValue()
{
	return ui->deleteGreaterThanValue->value();
}
