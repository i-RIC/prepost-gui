#include "rawdatapointmap.h"
#include "rawdatapointmapeditptsdialog.h"
#include "ui_rawdatapointmapeditptsdialog.h"

#include <vtkPointData.h>

RawDataPointmapEditPtsDialog::RawDataPointmapEditPtsDialog(RawDataPointmap *pmap, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RawDataPointmapEditPtsDialog)
{
    ui->setupUi(this);

	m_pointMap = pmap;
	QVector<vtkIdType> selectedVertices = m_pointMap->selectedVertices();
	vtkDataArray* vArr = m_pointMap->vtkGrid()->GetPointData()->GetArray("values");

	double min = 0;
	double max = 0;
	double average = 0;

	for (vtkIdType i = 0; i < selectedVertices.count(); ++i){
		double val = vArr->GetTuple1(selectedVertices[i]);
		if (i == 0 || val < min){min = val;}
		if (i == 0 || val > max){max = val;}
		average += val;
	}
	average /= selectedVertices.count();

	ui->maxPointValue->setValue(max);
	ui->minPointValue->setValue(min);
	ui->numPoints->setText(QString::number(selectedVertices.count()));

	if (selectedVertices.count() == 1){
		double pos[3];
		vtkIdType selV = selectedVertices[0];
		m_pointMap->vtkGrid()->GetPoints()->GetPoint(selV, pos);

		ui->xLocEdit->setValue(pos[0]);
		ui->yLocEdit->setValue(pos[1]);
		ui->valEdit->setValue(max);
	} else {
		ui->xLocEdit->setDisabled(true);
		ui->yLocEdit->setDisabled(true);
		ui->xLocEdit->clear();
		ui->yLocEdit->clear();
		ui->valEdit->setValue(average);
	}
}

RawDataPointmapEditPtsDialog::~RawDataPointmapEditPtsDialog()
{
    delete ui;
}

double RawDataPointmapEditPtsDialog::pointX()
{
	return ui->xLocEdit->value();
}

double RawDataPointmapEditPtsDialog::pointY()
{
	return ui->yLocEdit->value();
}

double RawDataPointmapEditPtsDialog::value()
{
	return ui->valEdit->value();
}
