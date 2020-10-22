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
	/*

	int numPts = this->m_pmap->getVtkInterpPolygon()->GetNumberOfPoints();
	numZPts = static_cast<int>(zpoint.size());
	numInterpPts = static_cast<int>(stot / ui->incrementEditWidget->value()) + 1;

	temp.resize(numInterpPts);
	yp.resize(numInterpPts);
	xinterp.resize(numInterpPts);
	yinterp.resize(numInterpPts);
	zinterp.resize(numInterpPts);

	for (int i = 0; i < numInterpPts; i++) {
		sout.push_back(i*(stot/(numInterpPts-1)));
	}
	int nump = static_cast<int>(xpoint.size());
	if (nump < 3) {
		std::vector<double> sitmp;
		sitmp.push_back(si[0]);
		sitmp.push_back(si[1]/2.);
		sitmp.push_back(si[1]);

		std::vector<double> txctmp;
		txctmp.push_back(xpoint[0]);
		txctmp.push_back(xpoint[0] + (xpoint[1]-xpoint[0])/2.);
		txctmp.push_back(xpoint[1]);
		std::vector<double> tyctmp;
		tyctmp.push_back(ypoint[0]);
		tyctmp.push_back(ypoint[0] + (ypoint[1]-ypoint[0])/2.);
		tyctmp.push_back(ypoint[1]);

		TSplineSTL(sitmp,txctmp,3,sout,xinterp,numInterpPts,5.,yp,temp);
		TSplineSTL(sitmp,tyctmp,3,sout,yinterp,numInterpPts,5.,yp,temp);
	} else {
		TSplineSTL(si,xpoint,numPts,sout,xinterp,numInterpPts,5.,yp,temp);
		TSplineSTL(si,ypoint,numPts,sout,yinterp,numInterpPts,5.,yp,temp);
	}

	yp.clear();
	temp.clear();
	temp.resize(numInterpPts);
	yp.resize(numInterpPts);

	if (numZPts < 3) {
		std::vector<double> szitmp;
		szitmp.push_back(szi[0]);
		szitmp.push_back(szi[1]/2.);
		szitmp.push_back(szi[1]);

		std::vector<double> tzctmp;
		tzctmp.push_back(zpoint[0]);
		tzctmp.push_back(zpoint[0] + (zpoint[1]-zpoint[0])/2.);
		tzctmp.push_back(zpoint[1]);

		TSplineSTL(szitmp,tzctmp,3,sout,zinterp,numInterpPts,10.,yp,temp);
	} else {

		TSplineSTL(szi,zpoint,numZPts,sout,zinterp,numInterpPts,10.,yp,temp);
	}
	*/
	QDialog::accept();
}
