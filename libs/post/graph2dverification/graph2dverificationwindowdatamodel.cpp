#define _USE_MATH_DEFINES  1  // set to 1 to match qwt_math.h
#include <cmath>

#include "datamodel/graph2dverificationwindowrootdataitem.h"
#include "graph2dverificationsettingdialog.h"
#include "graph2dverificationwindow.h"
#include "graph2dverificationwindowcontrolwidget.h"
#include "graph2dverificationwindowtopwidget.h"
#include <guibase/qwtplotcustomcurve.h>
#include "graph2dverificationwindowdatamodel.h"
#include "graph2dverificationwindowview.h"

#include <qwt_plot_marker.h>

#include <guibase/objectbrowserview.h>
#include <guicore/base/animationcontrollerinterface.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/project/measured/measureddata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/errormessage.h>
#include <misc/lastiodirectory.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QVector2D>
#include <vtkCell.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkStructuredGrid.h>

Graph2dVerificationWindowDataModel::Graph2dVerificationWindowDataModel(Graph2dVerificationWindow* w, ProjectDataItem* parent) :
	Graph2dWindowDataModel(w, parent)
{
	init();
}

Graph2dVerificationWindowDataModel::~Graph2dVerificationWindowDataModel()
{
	m_objectBrowserView->blockSignals(true);
	delete m_rootDataItem;
	m_objectBrowserView->blockSignals(false);
}

void Graph2dVerificationWindowDataModel::init()
{
	m_view = new Graph2dVerificationWindowView(dynamic_cast<QWidget*>(parent()));

	// setup the basic itemModel structure.
	Graph2dVerificationWindowRootDataItem* root = new Graph2dVerificationWindowRootDataItem(dynamic_cast<Graph2dVerificationWindow*>(m_mainWindow), this);
	m_rootDataItem = root;
	root->setupStandardModel(m_itemModel);
	connect(m_itemModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(handleObjectBrowserChange(QStandardItem*)));

	PostSolutionInfo* post = postSolutionInfo();
	connect(post, SIGNAL(currentStepUpdated()), this, SLOT(updateTime()));
	connect(post, SIGNAL(cgnsStepsUpdated()), this, SLOT(updateData()));
	connect(post, SIGNAL(zoneList2DUpdated()), this, SLOT(updateZoneList()));

	m_pointsCurve  = nullptr;
	m_lineCurve    = nullptr;
	m_dotLineCurve = nullptr;

	m_zeroMarker = new QwtPlotMarker();
	m_zeroMarker->setAxes(QwtPlot::xBottom, QwtPlot::yLeft);
	m_zeroMarker->setLineStyle(QwtPlotMarker::HLine);

	QPen pen;
	pen.setStyle(Qt::DotLine);
	pen.setColor(Qt::gray);
	m_zeroMarker->setLinePen(pen);

}

Graph2dVerificationWindowView* Graph2dVerificationWindowDataModel::view() const
{
	return dynamic_cast<Graph2dVerificationWindowView*>(m_view);
}

PostSolutionInfo* Graph2dVerificationWindowDataModel::postSolutionInfo()
{
	return projectData()->mainfile()->postSolutionInfo();
}

const std::vector<MeasuredData*>& Graph2dVerificationWindowDataModel::measuredData() const
{
	return projectData()->mainfile()->measuredDatas();
}

bool Graph2dVerificationWindowDataModel::setupInitialSetting()
{
	PostSolutionInfo* sInfo = postSolutionInfo();
	if (! sInfo->isDataAvailable()) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("No calculation result exists."));
		return false;
	}
	// initially, setup physical value settings.
	bool loaded = m_setting.init(postSolutionInfo(), measuredData());
	if (! loaded) {
		QMessageBox::critical(mainWindow(), tr("Error"), tr("Graph window setup fail. Calculation result is not loaded properly."));
		return false;
	}

	Graph2dVerificationSettingDialog dialog(dynamic_cast<QWidget*>(mainWindow()->parent()));

	dialog.setSetting(m_setting);

	int result = dialog.exec();
	if (result == QDialog::Rejected) {
		return false;
	}

	m_setting = dialog.setting();
	m_setting.postSolutionInfo()->setCurrentStep(m_setting.timeStep());
	iricMainWindow()->animationController()->setCurrentStepIndex(m_setting.timeStep());

	applySettings();

	updateGraph();

	return true;
}

void Graph2dVerificationWindowDataModel::getYAxisValueRange(Graph2dWindowDataModel::AxisSide as, double* min, double* max)
{
	Graph2dWindowView* v = view();
	QwtScaleDiv sDiv;
	if (as == Graph2dWindowDataModel::asLeft) {
		sDiv = v->axisScaleDiv(QwtPlot::yLeft);
	} else {
		sDiv = v->axisScaleDiv(QwtPlot::yRight);
	}
	*min = qMin(sDiv.lowerBound(), sDiv.upperBound());
	*max = qMax(sDiv.lowerBound(), sDiv.upperBound());
}

void Graph2dVerificationWindowDataModel::updateTime()
{
	updateGraph();
	updateData();
	view()->replot();
}

void Graph2dVerificationWindowDataModel::dataSourceSetting()
{
	Graph2dVerificationSettingDialog* dialog = new Graph2dVerificationSettingDialog(dynamic_cast<QWidget*>(mainWindow()->parent()));

	dialog->setSetting(m_setting);
	dialog->setTimeStep(m_setting.postSolutionInfo()->currentStep());
	int ret = dialog->exec();

	if (ret == QDialog::Rejected) {return;}

	const Graph2dVerificationWindowResultSetting& newSetting = dialog->setting();
	m_setting = newSetting;
	m_setting.postSolutionInfo()->setCurrentStep(m_setting.timeStep());
	iricMainWindow()->animationController()->setCurrentStepIndex(m_setting.timeStep());

	applySettings();
	view()->replot();

	delete dialog;
}

void Graph2dVerificationWindowDataModel::applySettings()
{
	// update axis setting.
	applyAxisSetting();

	updateData();
	updateTime();

	Graph2dVerificationWindow* w = dynamic_cast<Graph2dVerificationWindow*>(mainWindow());

	w->controlWidget()->setSetting(m_setting);
	w->topWidget()->setSetting(m_setting);
}

void Graph2dVerificationWindowDataModel::updateData()
{
	static bool updating = false;
	if (updating == true) {
		return;
	}
	updating = true;
	Graph2dVerificationWindowRootDataItem* root = dynamic_cast<Graph2dVerificationWindowRootDataItem*>(m_rootDataItem);
	root->updateData();
	updating = false;
}

void Graph2dVerificationWindowDataModel::doLoadFromProjectMainFile(const QDomNode& node)
{
	bool ok = m_setting.init(postSolutionInfo(), measuredData());
	if (! ok) {
		throw ErrorMessage("No solution found.");
	}
	QDomNode sNode = iRIC::getChildNode(node, "Setting");
	if (! sNode.isNull()) {
		m_setting.loadFromProjectMainFile(sNode);
	}
	Graph2dWindowDataModel::doLoadFromProjectMainFile(node);

	applySettings();
	updateData();
	view()->replot();
}

void Graph2dVerificationWindowDataModel::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	Graph2dWindowDataModel::doSaveToProjectMainFile(writer);
	writer.writeStartElement("Setting");
	m_setting.saveToProjectMainFile(writer);
	writer.writeEndElement();
}

void Graph2dVerificationWindowDataModel::exportData()
{
	QString iodir = LastIODirectory::get();
	QString fname = QFileDialog::getSaveFileName(
		this->mainWindow(), tr("Export CSV file"), iodir, tr("CSV file (*.csv)"));
	if (fname == "") { return; }
	QFile f(fname);
	bool ok = f.open(QIODevice::WriteOnly);
	if (!ok) {
		QMessageBox::critical(this->mainWindow(), tr("Error"), tr("%1 cound not be opened.").arg(QDir::toNativeSeparators(fname)));
		return;
	}
	QTextStream stream(&f);
	switch (this->m_setting.graphType()) {
	case Graph2dVerificationWindowResultSetting::gtSWDvsValues:
		stream << "X, Y, Stream-wise Distance,Measured Values,Calculatioin Result" << endl;
		for (int i = 0; i < m_pointsCurve->dataSize(); ++i) {
			stream << xVals[i] << "," << yVals[i] << "," << m_pointsCurve->sample(i).x() << "," << m_pointsCurve->sample(i).y() << "," << m_lineCurve->sample(i).y() << endl;
		}
		m_pointsCurve->data();
		break;
	case Graph2dVerificationWindowResultSetting::gtSWDvsError:
		stream << "Stream-wise Distance,Residual Error" << endl;
		for (size_t i = 0; i < m_pointsCurve->dataSize(); ++i) {
			stream << m_pointsCurve->data()->sample(i).x() << "," << m_pointsCurve->data()->sample(i).y() << "," << endl;
		}
		break;
	case Graph2dVerificationWindowResultSetting::gtMVvsCR:
		stream << "Measured Values,Calculatioin Result" << endl;
		for (size_t i = 0; i < m_pointsCurve->dataSize(); ++i) {
			stream << m_pointsCurve->sample(i).x() << "," << m_pointsCurve->sample(i).y() << "," << endl;
		}
		break;
	case Graph2dVerificationWindowResultSetting::gtMVvsError:
		stream << "Measured Values,Residual Error" << endl;
		for (size_t i = 0; i < m_pointsCurve->dataSize(); ++i) {
			stream << m_pointsCurve->sample(i).x() << "," << m_pointsCurve->sample(i).y() << "," << endl;
		}
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	f.close();
	QFileInfo finfo(fname);
	LastIODirectory::set(finfo.absolutePath());
	QMessageBox::information(this->mainWindow(), tr("Information"), tr("Data successfully exported to %1.").arg(QDir::toNativeSeparators(fname)));
}

void Graph2dVerificationWindowDataModel::setType(int type)
{
	vtkPointSet* ps = m_setting.activePostData()->data()->data();
	vtkStructuredGrid* sgrid = vtkStructuredGrid::SafeDownCast(ps);

	if (sgrid == nullptr) {
		// unstructured grid.
		switch (type) {
		case 0:
			m_setting.setGraphType(Graph2dVerificationWindowResultSetting::gtMVvsCR);
			break;
		case 1:
			m_setting.setGraphType(Graph2dVerificationWindowResultSetting::gtMVvsError);
			break;
		}
	}
	else {
		// structured grid.
		switch (type) {
		case 0:
			m_setting.setGraphType(Graph2dVerificationWindowResultSetting::gtSWDvsValues);
			break;
		case 1:
			m_setting.setGraphType(Graph2dVerificationWindowResultSetting::gtSWDvsError);
			break;
		case 2:
			m_setting.setGraphType(Graph2dVerificationWindowResultSetting::gtMVvsCR);
			break;
		case 3:
			m_setting.setGraphType(Graph2dVerificationWindowResultSetting::gtMVvsError);
			break;
		}
	}
	updateGraph();
}

void Graph2dVerificationWindowDataModel::updateGraph()
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
	m_setting.updateActivePostData();
	PostZoneDataContainer *cont = m_setting.activePostData();
	if (cont == nullptr) {
		return;
	}
	vtkPointSet* ps = m_setting.activePostData()->data()->data();
	if (ps == nullptr) {
		return;
	}

	vtkPolyData* pd = m_setting.activeMeasuredData()->pointData();
	distanceVals.reserve(pd->GetNumberOfPoints());
	measuredVals.reserve(pd->GetNumberOfPoints());
	calcVals.reserve(pd->GetNumberOfPoints());
	errorVals.reserve(pd->GetNumberOfPoints());
	xVals.reserve(pd->GetNumberOfPoints());
	yVals.reserve(pd->GetNumberOfPoints());
	vtkDoubleArray* mvda = vtkDoubleArray::SafeDownCast(pd->GetPointData()->GetArray(iRIC::toStr(m_setting.activeValue()).c_str()));
	vtkDoubleArray* crda = vtkDoubleArray::SafeDownCast(ps->GetPointData()->GetArray(iRIC::toStr(m_setting.activeResult()).c_str()));

	vtkStructuredGrid* sgrid = vtkStructuredGrid::SafeDownCast(ps);
	if (sgrid != nullptr) {
		int dimensions[3];
		sgrid->GetDimensions(dimensions);
		int centerJ = dimensions[1] / 2;
		stdDist.reserve(dimensions[0]);
		double distance = 0;
		double point[3];
		QVector2D previousP, currentP;
		vtkIdType index = m_setting.activePostData()->nodeIndex(0, centerJ, 0);
		sgrid->GetPoint(index, point);
		previousP = QVector2D(point[0], point[1]);
		for (int i = 0; i < dimensions[0]; ++i) {
			index = m_setting.activePostData()->nodeIndex(i, centerJ, 0);
			sgrid->GetPoint(index, point);
			currentP = QVector2D(point[0], point[1]);
			distance += (currentP - previousP).length();
			stdDist.append(distance);
			previousP = currentP;
		}
	}

	for (vtkIdType i = 0; i < mvda->GetNumberOfTuples(); ++i) {
		double mval = mvda->GetValue(i);

		double point[3];
		pd->GetPoint(i, point);

		// find the cell that contains the measured value.
		vtkIdType cellid;
		double pcoords[4];
		double weights[4];
		int subid;
		cellid = ps->FindCell(point, 0, 0, 1e-4, subid, pcoords, weights);
		if (cellid < 0) {
			// this point is outside of the calculation result.
			continue;
		}
		vtkCell* cell = ps->GetCell(cellid);
		double cval = 0;
		for (int j = 0; j < cell->GetNumberOfPoints(); ++j) {
			vtkIdType vid = cell->GetPointId(j);
			cval += *(weights + j) * crda->GetValue(vid);
		}
		measuredVals.append(mval);
		calcVals.append(cval);
		errorVals.append(cval - mval);
		xVals.push_back(point[0]);
		yVals.push_back(point[1]);

		errorSquareSum += (cval - mval) * (cval - mval);
		meanCalcVal += cval;
		meanMeasuredVal += mval;

		if (sgrid != nullptr) {
			int i, j, k;
			double dist = 0;
			for (int l = 0; l < cell->GetNumberOfPoints(); ++l) {
				vtkIdType vid = cell->GetPointId(l);
				m_setting.activePostData()->getNodeIJKIndex(vid, &i, &j, &k);
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
	for (vtkIdType i = 0; i < measuredVals.count(); ++i) {
		Sxy += (measuredVals.at(i) - meanMeasuredVal) * (calcVals.at(i) - meanCalcVal);
		Sxx += (measuredVals.at(i) - meanMeasuredVal) * (measuredVals.at(i) - meanMeasuredVal);
	}
	double b = Sxy / Sxx;
	double a = meanCalcVal - b * meanMeasuredVal;

	QVector<double> linex;
	QVector<double> liney;
	double xmin = 0;
	double xmax = 0;
	for (int i = 0; i < measuredVals.count(); ++i) {
		if (i == 0 || xmin > measuredVals.at(i)) { xmin = measuredVals.at(i); }
		if (i == 0 || xmax < measuredVals.at(i)) { xmax = measuredVals.at(i); }
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
	if (a > 0) {
		lineTitle.append(QString(" + %1").arg(a));
	}
	else {
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

	Q_ASSERT(m_pointsCurve == nullptr);
	m_pointsCurve = new QwtPlotCustomCurve();
	m_pointsCurve->setSymbol(symbol);
	m_pointsCurve->setStyle(QwtPlotCurve::NoCurve);
	m_pointsCurve->setLegendAttribute(QwtPlotCurve::LegendShowSymbol);
	m_pointsCurve->setLegendIconSize(QSize(7, 7));

	Q_ASSERT(m_lineCurve == nullptr);
	m_lineCurve = new QwtPlotCustomCurve();
	m_lineCurve->setLegendAttribute(QwtPlotCurve::LegendShowLine);

	view()->setAxisAutoScale(QwtPlot::yLeft);
	view()->setAxisAutoScale(QwtPlot::xBottom);
	switch (m_setting.graphType()) {
	case Graph2dVerificationWindowResultSetting::gtSWDvsValues:
		m_pointsCurve->setSamples(distanceVals, measuredVals);
		m_pointsCurve->setTitle(tr("Measured Values (%1)").arg(m_setting.activeValue()));
		m_pointsCurve->attach(view());

		m_lineCurve->setSamples(distanceVals, calcVals);
		m_lineCurve->setTitle(tr("Calculation Result (%1)").arg(m_setting.activeResult()));
		m_lineCurve->attach(view());

		view()->setAxisTitle(QwtPlot::xBottom, tr("Stream-wise Distance"));
		view()->setAxisTitle(QwtPlot::yLeft, tr("Values"));
		break;
	case Graph2dVerificationWindowResultSetting::gtSWDvsError:
		m_pointsCurve->setSamples(distanceVals, errorVals);
		m_pointsCurve->setTitle(tr("Residual Error"));
		m_pointsCurve->attach(view());
		m_zeroMarker->attach(view());

		view()->setAxisTitle(QwtPlot::xBottom, tr("Stream-wise Distance"));
		view()->setAxisTitle(QwtPlot::yLeft, tr("Residual Error"));
		break;

	case Graph2dVerificationWindowResultSetting::gtMVvsCR:
		m_pointsCurve->setSamples(measuredVals, calcVals);
		m_pointsCurve->setTitle(tr("Calculation Result (%1)").arg(m_setting.activeResult()));
		m_pointsCurve->attach(view());

		m_lineCurve->setSamples(linex, linex);
		m_lineCurve->setTitle("y = x");
		m_lineCurve->attach(view());

		view()->setAxisTitle(QwtPlot::xBottom, tr("Measured Value (%1)").arg(m_setting.activeValue()));
		view()->setAxisTitle(QwtPlot::yLeft, tr("Calculation Result (%1)").arg(m_setting.activeResult()));
		break;

	case Graph2dVerificationWindowResultSetting::gtMVvsError:
		m_pointsCurve->setSamples(measuredVals, errorVals);
		m_pointsCurve->setTitle(tr("Residual Error"));
		m_pointsCurve->attach(view());
		m_zeroMarker->attach(view());

		view()->setAxisTitle(QwtPlot::xBottom, tr("Measured Value (%1)").arg(m_setting.activeValue()));
		view()->setAxisTitle(QwtPlot::yLeft, tr("Residual Error"));
		break;
	}

	view()->replot();

	Graph2dVerificationWindow* w = dynamic_cast<Graph2dVerificationWindow*>(mainWindow());
	w->setComment(comment);
}

void Graph2dVerificationWindowDataModel::clearData()
{
	if (m_pointsCurve != nullptr) {
		m_pointsCurve->detach();
		delete m_pointsCurve;
		m_pointsCurve = nullptr;
	}
	if (m_lineCurve != nullptr) {
		m_lineCurve->detach();
		delete m_lineCurve;
		m_lineCurve = nullptr;
	}
	if (m_dotLineCurve != nullptr) {
		m_dotLineCurve->detach();
		delete m_dotLineCurve;
		m_dotLineCurve = nullptr;
	}
	xVals.clear();
	yVals.clear();

	Graph2dVerificationWindow* w = dynamic_cast<Graph2dVerificationWindow*>(mainWindow());
	w->setComment("");
}

void Graph2dVerificationWindowDataModel::updateZoneList()
{
	m_setting.setActivePostData(nullptr);
}
