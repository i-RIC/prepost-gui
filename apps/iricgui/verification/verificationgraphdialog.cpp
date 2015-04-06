#include "../main/iricmainwindow.h"
#include "ui_verificationgraphdialog.h"
#include "verificationgraphdialog.h"
#include "verificationsettingdialog.h"

#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/project/measureddata.h>
#include <misc/lastiodirectory.h>
#include <misc/stringtool.h>

#include <QBrush>
#include <QFileDialog>
#include <QMessageBox>
#include <QPen>
#include <QSize>
#include <QTextStream>
#include <QVector>
#include <QVector2D>

#include <vtkCell.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkStructuredGrid.h>

VerificationGraphDialog::VerificationGraphDialog(iRICMainWindow *parent) :
	QDialog(parent),
	ui(new Ui::VerificationGraphDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	m_timeStep = 0;

	m_mainWindow = parent;

	m_pointsCurve = 0;
	m_lineCurve = 0;
	m_dotLineCurve = 0;

	m_zeroMarker = new QwtPlotMarker();
	m_zeroMarker->setAxes(QwtPlot::xBottom, QwtPlot::yLeft);
	m_zeroMarker->setLineStyle(QwtPlotMarker::HLine);

	QPen pen;
	pen.setStyle(Qt::DotLine);
	pen.setColor(Qt::gray);
	m_zeroMarker->setLinePen(pen);

	m_graphType = gtSWDvsValues;

	ui->graphView->setAxisAutoScale(QwtPlot::yLeft);
	ui->graphView->setAxisAutoScale(QwtPlot::xBottom);
	ui->graphView->setMinimumSize(100, 100);

	connect(ui->settingButton, SIGNAL(clicked()), this, SLOT(setting()));
	connect(ui->exportButton, SIGNAL(clicked()), this, SLOT(exportData()));
	connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(close()));

	connect(ui->typeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setType(int)));
}

VerificationGraphDialog::~VerificationGraphDialog()
{
	delete ui;
	delete m_zeroMarker;
	clearData();
}

bool VerificationGraphDialog::setting()
{
	VerificationSettingDialog dialog(this);
	dialog.setPostSolutionInfo(m_postSolutionInfo);
	dialog.setMeasuredDatas(m_measuredData);

	dialog.setTimeStep(m_timeStep);
	dialog.setPostZoneDataContainer(m_activePostData);
	dialog.setActiveResult(m_activeResult);

	dialog.setMeasuredData(m_activeMeasuredData);
	dialog.setActiveValue(m_activeValue);

	int result = dialog.exec();
	if (result == QDialog::Rejected){return false;}

	m_timeStep = dialog.timeStep();
	m_activePostData = dialog.postZoneDataContainer();
	m_activeResult = dialog.activeResult();
	m_activeMeasuredData = dialog.measuredData();
	m_activeValue = dialog.activeValue();
	m_mainWindow->setCurrentStep(m_timeStep);

	vtkPointSet* ps = m_activePostData->data();
	vtkStructuredGrid* sgrid = vtkStructuredGrid::SafeDownCast(ps);

	ui->typeComboBox->blockSignals(true);
	ui->typeComboBox->clear();

	if (sgrid == 0){
		// unstructured grid.
	} else {
		// structured grid.
		ui->typeComboBox->addItem(tr("Stream-wise Distance vs. Measured Values and Calculation Result"));
		ui->typeComboBox->addItem(tr("Stream-wise Distance vs. Residual Errors"));
	}
	ui->typeComboBox->addItem(tr("Measured Values vs. Calculation Result"));
	ui->typeComboBox->addItem(tr("Measured Values vs. Residual Errors"));

	ui->typeComboBox->blockSignals(false);
	if (sgrid == 0){
		switch (m_graphType){
		case gtMVvsCR:
			ui->typeComboBox->setCurrentIndex(0);
			break;
		case gtMVvsError:
			ui->typeComboBox->setCurrentIndex(1);
			break;
		default:
			m_graphType = gtMVvsCR;
			ui->typeComboBox->setCurrentIndex(0);
			break;
		}
	} else {
		switch (m_graphType){
		case gtSWDvsValues:
			ui->typeComboBox->setCurrentIndex(0);
			break;
		case gtSWDvsError:
			ui->typeComboBox->setCurrentIndex(1);
			break;
		case gtMVvsCR:
			ui->typeComboBox->setCurrentIndex(2);
			break;
		case gtMVvsError:
			ui->typeComboBox->setCurrentIndex(3);
			break;
		}
	}
	updateGraph();
	return true;
}

void VerificationGraphDialog::exportData()
{
	QString iodir = LastIODirectory::get();
	QString fname = QFileDialog::getSaveFileName(
		this, tr("Export CSV file"), iodir, tr("CSV file (*.csv)"));
	if (fname == ""){return;}
	QFile f(fname);
	bool ok = f.open(QIODevice::WriteOnly);
	if (! ok){
		QMessageBox::critical(this, tr("Error"), tr("%1 cound not be opened.").arg(QDir::toNativeSeparators(fname)));
		return;
	}
	QTextStream stream(&f);
	switch (m_graphType){
	case gtSWDvsValues:
		stream << "X, Y, Stream-wise Distance,Measured Values,Calculatioin Result" << endl;
		for (int i = 0; i < m_pointsCurve->dataSize(); ++i){
			stream << xVals[i] << "," << yVals[i] << "," << m_pointsCurve->sample(i).x() << "," << m_pointsCurve->sample(i).y() << "," << m_lineCurve->sample(i).y() << endl;
		}
		m_pointsCurve->data();
		break;
	case gtSWDvsError:
		stream << "Stream-wise Distance,Residual Error" << endl;
		for (int i = 0; i < m_pointsCurve->dataSize(); ++i){
			stream << m_pointsCurve->data()->sample(i).x() << "," << m_pointsCurve->data()->sample(i).y() << "," << endl;
		}
		break;
	case gtMVvsCR:
		stream << "Measured Values,Calculatioin Result" << endl;
		for (int i = 0; i < m_pointsCurve->dataSize(); ++i){
			stream << m_pointsCurve->sample(i).x() << "," << m_pointsCurve->sample(i).y() << "," << endl;
		}
		break;
	case gtMVvsError:
		stream << "Measured Values,Residual Error" << endl;
		for (int i = 0; i < m_pointsCurve->dataSize(); ++i){
			stream << m_pointsCurve->sample(i).x() << "," << m_pointsCurve->sample(i).y() << "," << endl;
		}
		break;
	}
	f.close();
	QFileInfo finfo(fname);
	LastIODirectory::set(finfo.absolutePath());
	QMessageBox::information(this, tr("Information"), tr("Data successfully exported to %1.").arg(QDir::toNativeSeparators(fname)));
}

void VerificationGraphDialog::setType(int type)
{
	vtkPointSet* ps = m_activePostData->data();
	vtkStructuredGrid* sgrid = vtkStructuredGrid::SafeDownCast(ps);

	if (sgrid == 0){
		// unstructured grid.
		switch (type){
		case 0:
			m_graphType = gtMVvsCR;
			break;
		case 1:
			m_graphType = gtMVvsError;
			break;
		}
	} else {
		// structured grid.
		switch (type){
		case 0:
			m_graphType = gtSWDvsValues;
			break;
		case 1:
			m_graphType = gtSWDvsError;
			break;
		case 2:
			m_graphType = gtMVvsCR;
			break;
		case 3:
			m_graphType = gtMVvsError;
			break;
		}
	}
	updateGraph();
}

void VerificationGraphDialog::updateGraph()
{
	clearData();

	QVector<double> stdDist;

	QVector<double> distanceVals;
	QVector<double> calcVals;
	QVector<double> measuredVals;
	QVector<double> errorVals;

	double meanCalcVal = 0;
	double meanMeasuredVal = 0;

	double errorSquareSum = 0;

	// setup measuredVals.
	vtkPointSet* ps = m_activePostData->data();

	vtkPolyData* pd = m_activeMeasuredData->pointData();
	distanceVals.reserve(pd->GetNumberOfPoints());
	measuredVals.reserve(pd->GetNumberOfPoints());
	calcVals.reserve(pd->GetNumberOfPoints());
	errorVals.reserve(pd->GetNumberOfPoints());
	xVals.reserve(pd->GetNumberOfPoints());
	yVals.reserve(pd->GetNumberOfPoints());
	vtkDoubleArray* mvda = vtkDoubleArray::SafeDownCast(pd->GetPointData()->GetArray(iRIC::toStr(m_activeValue).c_str()));
	vtkDoubleArray* crda = vtkDoubleArray::SafeDownCast(ps->GetPointData()->GetArray(iRIC::toStr(m_activeResult).c_str()));

	vtkStructuredGrid* sgrid = vtkStructuredGrid::SafeDownCast(ps);
	if (sgrid != 0){
		int dimensions[3];
		sgrid->GetDimensions(dimensions);
		int centerJ = dimensions[1] / 2;
		stdDist.reserve(dimensions[0]);
		double distance = 0;
		double point[3];
		QVector2D previousP, currentP;
		vtkIdType index = m_activePostData->nodeIndex(0, centerJ, 0);
		sgrid->GetPoint(index, point);
		previousP = QVector2D(point[0], point[1]);
		for (int i = 0; i < dimensions[0]; ++i){
			index = m_activePostData->nodeIndex(i, centerJ, 0);
			sgrid->GetPoint(index, point);
			currentP = QVector2D(point[0], point[1]);
			distance += (currentP - previousP).length();
			stdDist.append(distance);
			previousP = currentP;
		}
	}

	for (vtkIdType i = 0; i < mvda->GetNumberOfTuples(); ++i){
		double mval = mvda->GetValue(i);

		double point[3];
		pd->GetPoint(i, point);

		// find the cell that contains the measured value.
		vtkIdType cellid;
		double pcoords[4];
		double weights[4];
		int subid;
		cellid = ps->FindCell(point, 0, 0, 1e-4, subid, pcoords, weights);
		if (cellid < 0){
			// this point is outside of the calculation result.
			continue;
		}
		vtkCell* cell = ps->GetCell(cellid);
		double cval = 0;
		for (int j = 0; j < cell->GetNumberOfPoints(); ++j){
			vtkIdType vid = cell->GetPointId(j);
			cval += *(weights + j) * crda->GetValue(vid);
		}
		measuredVals.append(mval);
		calcVals.append(cval);
		errorVals.append(cval - mval);
		xVals.append(point[0]);
		yVals.append(point[1]);

		errorSquareSum += (cval - mval) * (cval - mval);
		meanCalcVal += cval;
		meanMeasuredVal += mval;

		if (sgrid != 0){
			int i, j, k;
			double dist = 0;
			for (int l = 0; l < cell->GetNumberOfPoints(); ++l){
				vtkIdType vid = cell->GetPointId(l);
				m_activePostData->getNodeIJKIndex(vid, &i, &j, &k);
				double tmpdist = stdDist.at(i);
				dist += *(weights + l) * tmpdist;
			}
			distanceVals.append(dist);
		}
	}
	double rmse = sqrt(errorSquareSum / measuredVals.count());
	meanCalcVal /= measuredVals.count();
	meanMeasuredVal /= measuredVals.count();

	double Sxy = 0;
	double Sxx = 0;
	for (vtkIdType i = 0; i < measuredVals.count(); ++i){
		Sxy += (measuredVals.at(i) - meanMeasuredVal) * (calcVals.at(i) - meanCalcVal);
		Sxx += (measuredVals.at(i) - meanMeasuredVal) * (measuredVals.at(i) - meanMeasuredVal);
	}
	double b = Sxy / Sxx;
	double a = meanCalcVal - b * meanMeasuredVal;

	QVector<double> linex;
	QVector<double> liney;
	double xmin;
	double xmax;
	for (int i = 0; i < measuredVals.count(); ++i){
		if (i == 0 || xmin > measuredVals.at(i)){xmin = measuredVals.at(i);}
		if (i == 0 || xmax < measuredVals.at(i)){xmax = measuredVals.at(i);}
	}
	double width = xmax - xmin;
	double x = xmin - width * 0.1;
	double y = a + b * x;
	linex.append(x);
	liney.append(y);
	x = xmax + width * 0.1;
	y = a + b * x;
	linex.append(x);
	liney.append(y);

	QString lineTitle = QString("y = %1 * x").arg(b);
	if (a > 0){
		lineTitle.append(QString(" + %1").arg(a));
	} else {
		lineTitle.append(QString(" - %1").arg(-a));
	}

	QString comment;
	comment = tr("Root Mean Squared Error: %1").arg(rmse);
	QPen dotLinePen;
	dotLinePen.setStyle(Qt::DotLine);
	dotLinePen.setColor(Qt::gray);

	m_zeroMarker->detach();

	QBrush brush(Qt::gray);
	QPen pen(Qt::black);
	QwtSymbol* symbol = new QwtSymbol(QwtSymbol::Ellipse, brush, pen, QSize(6, 6));

	m_pointsCurve = new QwtPlotCurve();
	m_pointsCurve->setSymbol(symbol);
	m_pointsCurve->setStyle(QwtPlotCurve::NoCurve);
	m_pointsCurve->setLegendAttribute(QwtPlotCurve::LegendShowSymbol);
	m_pointsCurve->setLegendIconSize(QSize(7, 7));

	m_lineCurve = new QwtPlotCurve();
	m_lineCurve->setLegendAttribute(QwtPlotCurve::LegendShowLine);

	switch (m_graphType){
	case gtSWDvsValues:
		m_pointsCurve->setSamples(distanceVals, measuredVals);
		m_pointsCurve->setTitle(tr("Measured Values (%1)").arg(m_activeValue));
		m_pointsCurve->attach(ui->graphView);

		m_lineCurve->setSamples(distanceVals, calcVals);
		m_lineCurve->setTitle(tr("Calculation Result (%1)").arg(m_activeResult));
		m_lineCurve->attach(ui->graphView);

		ui->graphView->setAxisTitle(QwtPlot::xBottom, tr("Stream-wise Distance"));
		ui->graphView->setAxisTitle(QwtPlot::yLeft, tr("Values"));
		break;
	case gtSWDvsError:
		m_pointsCurve->setSamples(distanceVals, errorVals);
		m_pointsCurve->setTitle(tr("Residual Error"));
		m_pointsCurve->attach(ui->graphView);
		m_zeroMarker->attach(ui->graphView);

		ui->graphView->setAxisTitle(QwtPlot::xBottom, tr("Stream-wise Distance"));
		ui->graphView->setAxisTitle(QwtPlot::yLeft, tr("Residual Error"));
		break;

	case gtMVvsCR:
		m_pointsCurve->setSamples(measuredVals, calcVals);
		m_pointsCurve->setTitle(tr("Calculation Result (%1)").arg(m_activeResult));
		m_pointsCurve->attach(ui->graphView);

		m_lineCurve->setSamples(linex, linex);
		m_lineCurve->setTitle("y = x");
		m_lineCurve->attach(ui->graphView);

		ui->graphView->setAxisTitle(QwtPlot::xBottom, tr("Measured Value (%1)").arg(m_activeValue));
		ui->graphView->setAxisTitle(QwtPlot::yLeft, tr("Calculation Result (%1)").arg(m_activeResult));
		break;

	case gtMVvsError:
		m_pointsCurve->setSamples(measuredVals, errorVals);
		m_pointsCurve->setTitle(tr("Residual Error"));
		m_pointsCurve->attach(ui->graphView);
		m_zeroMarker->attach(ui->graphView);

		ui->graphView->setAxisTitle(QwtPlot::xBottom, tr("Measured Value (%1)").arg(m_activeValue));
		ui->graphView->setAxisTitle(QwtPlot::yLeft, tr("Residual Error"));
		break;
	}

	ui->graphView->replot();
	ui->commentLabel->setText(comment);
}

void VerificationGraphDialog::clearData()
{
	if (m_pointsCurve != 0){
		m_pointsCurve->detach();
		delete m_pointsCurve;
		m_pointsCurve = 0;
	}
	if (m_lineCurve != 0){
		m_lineCurve->detach();
		delete m_lineCurve;
		m_lineCurve = 0;
	}
	if (m_dotLineCurve != 0){
		m_dotLineCurve->detach();
		delete m_dotLineCurve;
		m_dotLineCurve = 0;
	}
	xVals.clear();
	yVals.clear();
}
