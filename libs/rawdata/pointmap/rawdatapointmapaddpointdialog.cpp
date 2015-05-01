#include "ui_rawdatapointmapaddpointdialog.h"

#include "rawdatapointmap.h"
#include "rawdatapointmapaddpointdialog.h"

RawDataPointmapAddPointDialog::RawDataPointmapAddPointDialog(RawDataPointmap* pmap, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::RawDataPointmapAddPointDialog)
{
	ui->setupUi(this);
	this->m_pmap = pmap;
	double p[3];

	this->m_pmap->getVtkInterpPolygon()->GetPoints()->GetPoint(1,p);
	p[2] = this->m_pmap->getVtkInterpValue()->GetValue(0);

	ui->m_AddPointValue->setValue(p[2]);

}

RawDataPointmapAddPointDialog::~RawDataPointmapAddPointDialog()
{
	delete ui;
}

void RawDataPointmapAddPointDialog::accept()
{

	double p[3];
	int numPts = this->m_pmap->getVtkInterpPolygon()->GetPoints()->GetNumberOfPoints();
	for (int i = 0; i < numPts; i++) {
		this->m_pmap->getVtkInterpPolygon()->GetPoints()->GetPoint(i,p);
		p[2] = this->m_pmap->getVtkInterpValue()->GetValue(i);
		this->xpoint.push_back(p[0]);
		this->ypoint.push_back(p[1]);
		this->zpoint.push_back(ui->m_AddPointValue->value());
	}

	QDialog::accept();
}