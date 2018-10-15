#include "ui_geodatapointmapinterpolatepoints.h"

#include "geodatapointmap.h"
#include "geodatapointmapinterpolatepoints.h"

#include <QMessageBox>

#include <vtkDoubleArray.h>
#include <vtkPolygon.h>

void TSplineSTL(std::vector<double>& x, std::vector<double>& y, int n,
								std::vector<double>& xout, std::vector<double>& yout, int iout, float sigma,
								std::vector<double>& yp, std::vector<double>& temp)
{

	// subroutine variables
	double delx1, dx1, delx2, delx12, c1, c2, c3, slpp1, deln, delnm1;
	double delnn, slppn, sigmap, dels, exps, sinhs, sinhin;
	double diag1, diag2, diagin, spdiag, dx2;
	double a, b, del1, del2, exps1, sinhd1, sinhd2;
	double tmp;
	int nm1, np1, i, ibak, nj;

	Q_UNUSED(tmp)

	nm1 = n-2; //may need to make adjustments here because of c arrays 0-n vs 1-n for fortran
	np1 = n;
	delx1 = x[1]-x[0];
	dx1 = (y[1]-y[0])/delx1;
	delx2 = x[2]-x[1];
	delx12 = x[2]-x[0];
	c1 = -(delx12+delx1)/delx12/delx1;
	c2 = delx12/delx1/delx2;
	c3 = -delx1/delx12/delx2;
	slpp1 = c1*y[0]+c2*y[1]+c3*y[2];
	deln = x[n-1]-x[nm1];
	delnm1 = x[nm1]-x[n-3];
	delnn = x[n-1]-x[n-3];
	c1 = (delnn+deln)/delnn/deln;
	c2 = -delnn/deln/delnm1;
	c3 = deln/delnn/delnm1;
	slppn = c3*y[n-3]+c2*y[nm1]+c1*y[n-1];
	sigmap = fabs(sigma)*(double(n-2))/(x[n-1]-x[0]);
	dels = sigmap*delx1;
	exps = exp(dels);
	sinhs = 0.5*(exps-1./exps);
	sinhin = 1./(delx1*sinhs);
	diag1 = sinhin*(dels*0.5*(exps+1./exps)-sinhs);
	diagin = 1./diag1;
	yp[0] = diagin*(dx1-slpp1);
	spdiag = sinhin*(sinhs-dels);
	temp[0] = diagin* spdiag;
	for (i = 1; i <= nm1; i++) {
		delx2 = x[i+1] - x[i];
		dx2 = (y[i+1]-y[i])/delx2;
		dels = sigmap*delx2;
		exps = exp(dels);
		sinhs = 0.5*(exps-1./exps);
		sinhin = 1./(delx2*sinhs);
		diag2 = sinhin*(dels*(0.5*(exps+1./exps))-sinhs);
		diagin = 1./(diag1+diag2-spdiag*temp[i-1]);
		yp[i] = diagin*(dx2-dx1-spdiag*yp[i-1]);
		spdiag = sinhin*(sinhs-dels);
		temp[i] = diagin*spdiag;
		dx1 = dx2;
		diag1 = diag2;
	}
	diagin = 1./(diag1-spdiag*temp[nm1]);
	yp[n-1] = diagin*(slppn-dx2-spdiag*yp[nm1]);
	for (i = 1; i <= n - 1; i++) {
		ibak = np1-i-1;
		yp[ibak] =yp[ibak]-temp[ibak]*yp[ibak+1];
	}
	a = x[0];
	b = x[1];
	nj = 1;
	for (i = 0; i <= iout - 1; i++) {
		while (xout[i] > b) {
			a = b;
			nj = nj+1;
			if (nj < n) {
				b = x[nj];
			}
		}
		del1 = xout[i] - a;
		del2 = b-xout[i];
		dels = b-a;
		exps1 = exp(sigmap*del1);
		sinhd1 = 0.5*(exps1-1./exps1);
		exps = exp(sigmap*del2);
		sinhd2 = 0.5*(exps-1./exps);
		exps = exps*exps1;
		sinhs = 0.5*(exps-1./exps);
		tmp = (yp[nj]*sinhd1+yp[nj-1]*sinhd2)/
					sinhs+((y[nj]-yp[nj])*del1+(y[nj-1]-yp[nj-1])*del2)/dels;
		yout[i] = (yp[nj]*sinhd1+yp[nj-1]*sinhd2)/
							sinhs+((y[nj]-yp[nj])*del1+(y[nj-1]-yp[nj-1])*del2)/dels;
	}
}

GeoDataPointmapInterpolatePoints::GeoDataPointmapInterpolatePoints(GeoDataPointmap* pmap, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GeoDataPointmapInterpolatePoints)
{
	ui->setupUi(this);
	ui->incrementEditWidget->setValue(1.0);
	this->m_pmap = pmap;
	int numPts = this->m_pmap->getVtkInterpPolygon()->GetNumberOfPoints();
	double p[3];
	double ds;
	for (int i = 0; i < numPts; i++) {
		this->m_pmap->getVtkInterpPolygon()->GetPoints()->GetPoint(i,p);
		p[2] = this->m_pmap->getVtkInterpValue()->GetValue(i);
		this->xpoint.push_back(p[0]);
		this->ypoint.push_back(p[1]);
		if (p[2] > -9999.0) {
			zpoint.push_back(p[2]);
			zindex.push_back(i);
			izindex.push_back(0);
		} else {
			izindex.push_back(-1);
		}
	}
	for (int i = 0; i < numPts; i++) {
		if (i == 0) {
			si.push_back(0);
			szi.push_back(0);
		} else {
			ds = pow((pow((xpoint[i] - xpoint[i-1]),2)+pow((ypoint[i]-ypoint[i-1]),2)),0.5);
			si.push_back(si[i-1]+ds);
			if (izindex[i] >= 0) { szi.push_back(si[i-1]+ds); }
		}
	}
	stot = si[numPts-1];

	ui->lengthLineEdit->setValue(stot);
}

GeoDataPointmapInterpolatePoints::~GeoDataPointmapInterpolatePoints()
{
	delete ui;
}

void GeoDataPointmapInterpolatePoints::reject()
{
	QDialog::reject();
}

void GeoDataPointmapInterpolatePoints::accept()
{
	if (ui->incrementEditWidget->value() <= 0 || ui->incrementEditWidget->value() >= stot) {
		QMessageBox msgBox;
		msgBox.setText("Interpolation increment must be greater 0 and less than interpolation line length");
		msgBox.setInformativeText("Would you like to retry or cancel the interpolation points operation?");
		msgBox.setStandardButtons(QMessageBox::Retry);
		msgBox.setDefaultButton(QMessageBox::Retry);
		if (msgBox.exec() == QMessageBox::Retry) {
			return;
		} else {
			return;
		}
	}

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

	QDialog::accept();
}
