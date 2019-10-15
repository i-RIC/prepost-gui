#define _USE_MATH_DEFINES  1  // set to 1 to match qwt_math.h
#include <cmath>

#include "datamodel/graph2dverificationwindowrootdataitem.h"
#include "geodata/polyline/geodatapolyline.h"
#include "graph2dverificationsettingdialog.h"
#include "graph2dverificationwindow.h"
#include "graph2dverificationwindowcontrolwidget.h"
#include "graph2dverificationwindowtopwidget.h"
#include <guibase/qwtplotcustomcurve.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/project/measured/measureddata.h>
#include "graph2dverificationwindowdatamodel.h"
#include "graph2dverificationwindowdatasourcedialog.h"
#include "graph2dverificationwindowprojectdataitem.h"
#include "graph2dverificationwindowview.h"

#include <qwt_plot_curve.h>
#include <qwt_symbol.h>

#include <qwt_plot_marker.h>
#include <qwt_scale_engine.h>


#include <gridcreatingcondition/riversurvey/gridcreatingconditionriversurvey.h>
#include <gui/main/iRICMainWindow.h>
#include <guibase/objectbrowserview.h>
#include <guicore/base/animationcontrollerinterface.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/misc/cgnsfileopener.h>
#include <guicore/post/postzoneselectingdialog.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/posttimesteps.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/project/measured/measureddata.h>
#include <guicore/pre/base/preprocessordatamodelinterface.h>
#include <guicore/pre/base/preprocessorgridandgridcreatingconditiondataiteminterface.h>
#include <guicore/pre/base/preprocessorgridcreatingconditiondataiteminterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/errormessage.h>
#include <misc/lastiodirectory.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>
#include <post/graph2d/datamodel/graph2dwindowmarkergroupdataitem.h>
#include <post/graph2d/graph2dwindowmarkersetting.h>

#include <QApplication>
#include <QDomNode>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QTextStream>
#include <QMessageBox>
#include <QPen>
#include <QSize>
#include <QVector2D>
#include <QVector>
#include <vtkCell.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkStructuredGrid.h>

#include <cgnslib.h>

#include <stdexcept>

Graph2dVerificationWindowDataModel::Graph2dVerificationWindowDataModel(Graph2dVerificationWindow* w, ProjectDataItem* parent)
	: Graph2dWindowDataModel(w, parent)
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
	connect(post, SIGNAL(cgnsStepsUpdated(int)), this, SLOT(updateData(int)));

#if SKIP
	m_regionMode = Graph2dVerificationWindowContinuousExportDialog::rmCurrentOnly;
	m_timeMode = Graph2dVerificationWindowContinuousExportDialog::tmCurrentOnly;
#endif
	m_prefix = "Snapshot";
	m_csvPrefix = "ExportData";

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

void Graph2dVerificationWindowDataModel::axisSetting()
{
	showSettingDialog();
}

#if SKIP
void Graph2dVerificationWindowDataModel::specialSnapshot()
{
	int dims[4];

	PostSolutionInfo* sol = postSolutionInfo();

	Graph2dVerificationWindowResultSetting::DataTypeInfo* tinfo = m_setting.targetDataTypeInfo();
	PostZoneDataContainer* cont = sol->zoneContainer(tinfo->dimension, tinfo->zoneName);
	if (cont != nullptr) {
		vtkStructuredGrid* sGrid = dynamic_cast<vtkStructuredGrid*>(cont->data());
		if (sGrid != nullptr) {
			// structured
			sGrid->GetDimensions(dims);
			if (tinfo->gridLocation == CellCenter) {
				sGrid->GetCellDims(dims);
			}
			dims[3] = 1;
		} else {
			// unstructured
			dims[0] = 1;
			dims[1] = 1;
			dims[2] = 1;
			dims[3] = cont->data()->GetNumberOfPoints();
			if (tinfo->gridLocation == CellCenter) {
				dims[3] = cont->data()->GetNumberOfCells();
			}
		}
	} else {
		dims[0] = 1;
		dims[1] = 1;
		dims[2] = 1;
		dims[3] = 1;
	}
	Graph2dVerificationWindowContinuousExportDialog dialog(mainWindow());
	dialog.setSetting(m_setting, dims);
	dialog.setTimesteps(projectData()->mainfile()->postSolutionInfo()->timeSteps()->timesteps());
	dialog.setCurrentStep(projectData()->mainfile()->postSolutionInfo()->currentStep());

	dialog.setFolder(LastIODirectory::get());
	dialog.setPrefix(m_prefix);
	dialog.setRegionMode(m_regionMode);
	dialog.setIMin(m_iMin);
	dialog.setIMax(m_iMax);
	dialog.setJMin(m_jMin);
	dialog.setJMax(m_jMax);
	dialog.setKMin(m_kMin);
	dialog.setKMax(m_kMax);
	dialog.setIndexMin(m_indexMin);
	dialog.setIndexMax(m_indexMax);
	dialog.setTimeMode(m_timeMode);
	dialog.setStartTimeStep(m_timeStart);
	dialog.setEndTimeStep(m_timeEnd);
	dialog.setTimeSkip(m_timeSkip);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	QString folder = dialog.folder();
	LastIODirectory::set(folder);
	m_prefix = dialog.prefix();
	m_regionMode = dialog.regionMode();
	m_iMin = dialog.iMin();
	m_iMax = dialog.iMax();
	m_jMin = dialog.jMin();
	m_jMax = dialog.jMax();
	m_kMin = dialog.kMin();
	m_kMax = dialog.kMax();
	m_indexMin = dialog.indexMin();
	m_indexMax = dialog.indexMax();
	m_timeMode = dialog.timeMode();
	m_timeStart = dialog.startTimeStep();
	m_timeEnd = dialog.endTimeStep();
	m_timeSkip = dialog.timeSkip();

	// execute export.
	int timeStepCount = (m_timeEnd - m_timeStart) / m_timeSkip + 1;
	int dataCount = 1;

	switch (tinfo->dataType) {
	case Graph2dVerificationWindowResultSetting::dtBaseIterative:
		dataCount = 1;
		break;
	case Graph2dVerificationWindowResultSetting::dtDim1DStructured:
		if (m_setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaTime) {
			dataCount = (m_iMax - m_iMin + 1);
		}
		break;
	case Graph2dVerificationWindowResultSetting::dtDim2DStructured:
		if (m_setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaTime) {
			dataCount = (m_iMax - m_iMin + 1) * (m_jMax - m_jMin + 1);
		} else if (m_setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaI) {
			dataCount = (m_jMax - m_jMin + 1);
		} else if (m_setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaJ) {
			dataCount = (m_iMax - m_iMin + 1);
		}
		break;
	case Graph2dVerificationWindowResultSetting::dtDim3DStructured:
		if (m_setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaTime) {
			dataCount = (m_iMax - m_iMin + 1) * (m_jMax - m_jMin + 1) * (m_kMax - m_kMin + 1);
		} else if (m_setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaI) {
			dataCount = (m_jMax - m_jMin + 1) * (m_kMax - m_kMin + 1);
		} else if (m_setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaJ) {
			dataCount = (m_iMax - m_iMin + 1) * (m_kMax - m_kMin + 1);
		} else if (m_setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaK) {
			dataCount = (m_iMax - m_iMin + 1) * (m_jMax - m_jMin + 1);
		}
		break;
	case Graph2dVerificationWindowResultSetting::dtDim1DUnstructured:
	case Graph2dVerificationWindowResultSetting::dtDim2DUnstructured:
	case Graph2dVerificationWindowResultSetting::dtDim3DUnstructured:
		dataCount = (m_indexMax - m_indexMin + 1);
		break;
	}
	QProgressDialog pdialog(mainWindow());
	pdialog.setRange(0, timeStepCount * dataCount);
	pdialog.setWindowTitle(tr("Snapshot"));
	pdialog.setLabelText(tr("Saving snapshots..."));
	pdialog.setModal(true);
	pdialog.show();

	int timeStep = m_timeStart;
	int imageIndex = 0;
	while (timeStep <= m_timeEnd) {
		iricMainWindow()->animationController()->setCurrentStepIndex(timeStep);

		pdialog.setValue(imageIndex);
		qApp->processEvents();
		if (pdialog.wasCanceled()) {
			return;
		}
		Graph2dVerificationWindow* window = dynamic_cast<Graph2dVerificationWindow*>(mainWindow());
		QPixmap pixmap;
		QString filename;
		bool ok;
		switch (tinfo->dataType) {
		case Graph2dVerificationWindowResultSetting::dtBaseIterative:
			pixmap = window->snapshot();
			filename = QDir(folder).absoluteFilePath(m_prefix);
			filename.append(QString("_Time=%1.png").arg(timeStep + 1));
			ok = savePixmap(pixmap, filename);
			if (! ok) {
				showErrorMessage(filename);
				return;
			}
			++ imageIndex;
			break;
		case Graph2dVerificationWindowResultSetting::dtDim1DStructured:
			if (m_setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaTime) {
				for (int i = m_iMin; i <= m_iMax; ++i) {
					window->controlWidget()->setIValue(i);
					pixmap = window->snapshot();
					filename = QDir(folder).absoluteFilePath(m_prefix);
					filename.append(QString("_Time=%1_I=%2.png").arg(formattedNumber(timeStep + 1, m_timeEnd)).arg(formattedNumber(i + 1, m_iMax)));
					ok = savePixmap(pixmap, filename);
					if (! ok) {
						showErrorMessage(filename);
						return;
					}
					qApp->processEvents();
					if (pdialog.wasCanceled()) {
						return;
					}
					++ imageIndex;
				}
			}
			break;
		case Graph2dVerificationWindowResultSetting::dtDim2DStructured:
			if (m_setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaTime) {
				for (int i = m_iMin; i <= m_iMax; ++i) {
					window->controlWidget()->setIValue(i);
					for (int j = m_jMin; j <= m_jMax; ++j) {
						window->controlWidget()->setJValue(j);
						pixmap = window->snapshot();
						filename = QDir(folder).absoluteFilePath(m_prefix);
						filename.append(QString("_Time=%1_I=%2_J=%3.png").arg(formattedNumber(timeStep + 1, m_timeEnd)).arg(formattedNumber(i + 1, m_iMax)).arg(formattedNumber(j + 1, m_jMax)));
						ok = savePixmap(pixmap, filename);
						if (! ok) {
							showErrorMessage(filename);
							return;
						}
						qApp->processEvents();
						if (pdialog.wasCanceled()) {
							return;
						}
						++ imageIndex;
					}
					pdialog.setValue(imageIndex);
				}
			} else if (m_setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaI) {
				for (int j = m_jMin; j <= m_jMax; ++j) {
					window->controlWidget()->setJValue(j);
					pixmap = window->snapshot();
					filename = QDir(folder).absoluteFilePath(m_prefix);
					filename.append(QString("_Time=%1_J=%2.png").arg(formattedNumber(timeStep + 1, m_timeEnd)).arg(formattedNumber(j + 1, m_jMax)));
					ok = savePixmap(pixmap, filename);
					if (! ok) {
						showErrorMessage(filename);
						return;
					}
					qApp->processEvents();
					if (pdialog.wasCanceled()) {
						return;
					}
					++ imageIndex;
				}
			} else if (m_setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaJ) {
				for (int i = m_iMin; i <= m_iMax; ++i) {
					window->controlWidget()->setIValue(i);
					pixmap = window->snapshot();
					filename = QDir(folder).absoluteFilePath(m_prefix);
					filename.append(QString("_Time=%1_I=%2.png").arg(formattedNumber(timeStep + 1, m_timeEnd)).arg(formattedNumber(i + 1, m_iMax)));
					ok = savePixmap(pixmap, filename);
					if (! ok) {
						showErrorMessage(filename);
						return;
					}
					qApp->processEvents();
					if (pdialog.wasCanceled()) {
						return;
					}
					++ imageIndex;
				}
			}
			break;
		case Graph2dVerificationWindowResultSetting::dtDim3DStructured:
			if (m_setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaTime) {
				for (int i = m_iMin; i <= m_iMax; ++i) {
					window->controlWidget()->setIValue(i);
					for (int j = m_jMin; j <= m_jMax; ++j) {
						window->controlWidget()->setJValue(j);
						for (int k = m_kMin; k <= m_kMax; ++k) {
							window->controlWidget()->setKValue(k);
							pixmap = window->snapshot();
							filename = QDir(folder).absoluteFilePath(m_prefix);
							filename.append(QString("_Time=%1_I=%2_J=%3_K=%4.png").arg(formattedNumber(timeStep + 1, m_timeEnd)).arg(formattedNumber(i + 1, m_iMax)).arg(formattedNumber(j + 1, m_jMax)).arg(formattedNumber(k + 1, m_kMax)));
							ok = savePixmap(pixmap, filename);
							if (! ok) {
								showErrorMessage(filename);
								return;
							}
							qApp->processEvents();
							if (pdialog.wasCanceled()) {
								return;
							}
							++ imageIndex;
						}
					}
					pdialog.setValue(imageIndex);
				}
			} else if (m_setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaI) {
				for (int j = m_jMin; j <= m_jMax; ++j) {
					window->controlWidget()->setJValue(j);
					for (int k = m_kMin; k <= m_kMax; ++k) {
						window->controlWidget()->setKValue(k);
						pixmap = window->snapshot();
						filename = QDir(folder).absoluteFilePath(m_prefix);
						filename.append(QString("_Time=%1_J=%2_K=%3.png").arg(formattedNumber(timeStep + 1, m_timeEnd)).arg(formattedNumber(j + 1, m_jMax)).arg(formattedNumber(k + 1, m_kMax)));
						ok = savePixmap(pixmap, filename);
						if (! ok) {
							showErrorMessage(filename);
							return;
						}
						qApp->processEvents();
						if (pdialog.wasCanceled()) {
							return;
						}
						++ imageIndex;
					}
					pdialog.setValue(imageIndex);
				}
			} else if (m_setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaJ) {
				for (int i = m_iMin; i <= m_iMax; ++i) {
					window->controlWidget()->setIValue(i);
					for (int k = m_kMin; k <= m_kMax; ++k) {
						window->controlWidget()->setKValue(k);
						pixmap = window->snapshot();
						filename = QDir(folder).absoluteFilePath(m_prefix);
						filename.append(QString("_Time=%1_I=%2_K=%3.png").arg(formattedNumber(timeStep + 1, m_timeEnd)).arg(formattedNumber(i + 1, m_iMax)).arg(formattedNumber(k + 1, m_kMax)));
						ok = savePixmap(pixmap, filename);
						if (! ok) {
							showErrorMessage(filename);
							return;
						}
						qApp->processEvents();
						if (pdialog.wasCanceled()) {
							return;
						}
						++ imageIndex;
					}
					pdialog.setValue(imageIndex);
				}
			} else if (m_setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaK) {
				for (int i = m_iMin; i <= m_iMax; ++i) {
					window->controlWidget()->setIValue(i);
					for (int j = m_jMin; j <= m_jMax; ++j) {
						window->controlWidget()->setJValue(j);
						pixmap = window->snapshot();
						filename = QDir(folder).absoluteFilePath(m_prefix);
						filename.append(QString("_Time=%1_I=%2_J=%3.png").arg(formattedNumber(timeStep + 1, m_timeEnd)).arg(formattedNumber(i + 1, m_iMax)).arg(formattedNumber(j + 1, m_jMax)));
						ok = savePixmap(pixmap, filename);
						if (! ok) {
							showErrorMessage(filename);
							return;
						}
						qApp->processEvents();
						if (pdialog.wasCanceled()) {
							return;
						}
						++ imageIndex;
					}
					pdialog.setValue(imageIndex);
				}
			}
			break;
		case Graph2dVerificationWindowResultSetting::dtDim1DUnstructured:
		case Graph2dVerificationWindowResultSetting::dtDim2DUnstructured:
		case Graph2dVerificationWindowResultSetting::dtDim3DUnstructured:
			for (int idx = m_indexMin; idx <= m_indexMax; ++idx) {
				window->controlWidget()->setIndexValue(idx);
				pixmap = window->snapshot();
				filename = QDir(folder).absoluteFilePath(m_prefix);
				filename.append(QString("_Time=%1_Index=%2.png").arg(formattedNumber(timeStep + 1, m_timeEnd)).arg(formattedNumber(idx + 1, m_indexMax)));
				ok = savePixmap(pixmap, filename);
				if (! ok) {
					showErrorMessage(filename);
					return;
				}
				qApp->processEvents();
				if (pdialog.wasCanceled()) {
					return;
				}
				++ imageIndex;
				pdialog.setValue(imageIndex);
			}
			break;
		}
		pdialog.setValue(imageIndex);
		timeStep += m_timeSkip;
	}
}
#endif

void Graph2dVerificationWindowDataModel::specialCsvExport()
{
	QMessageBox::information(this->mainWindow(), "XXX::XXX", "Testing");
	return;
}

bool Graph2dVerificationWindowDataModel::savePixmap(const QPixmap& pixmap, const QString& filename)
{
	// rename the temporary file to the target file.
	if (QFile::exists(filename)) {
		// remove first.
		if (! QFile::remove(filename)) {
			// unable to remove. fail.
			return false;
		}
	}
	return pixmap.save(filename);
}

void Graph2dVerificationWindowDataModel::showErrorMessage(const QString& filename)
{
	QMessageBox::critical(mainWindow(), tr("Error"), tr("Saving snapshot image to %1 failed.").arg(filename));
}

QString Graph2dVerificationWindowDataModel::formattedNumber(int number, int max)
{
	int limit = 10;
	int len = 1;
	while (max >= limit) {
		limit *= 10;
		++ len;
	}
	QString ret;
	QTextStream out(&ret);
	out.setFieldWidth(len);
	out.setPadChar('0');
	out.setFieldAlignment(QTextStream::AlignRight);
	out << number;

	return ret;
}

void Graph2dVerificationWindowDataModel::copyCalculationResult()
{
	Q_ASSERT(false);
}

void Graph2dVerificationWindowDataModel::drawSetting()
{
	Q_ASSERT(false);
}

void Graph2dVerificationWindowDataModel::markerSettiing()
{
	Q_ASSERT(false);
}

template <typename DataItem>
void Graph2dVerificationWindowDataModel::getXY(DataItem* dataItem, QVector<double>* x, QVector<double>* y) const
{
	*x = dataItem->xValues();
	*y = dataItem->yValues();
	if (m_setting.xAxisMode() != Graph2dVerificationWindowResultSetting::xaTime) {return;}
	int timeStepCount = projectData()->mainfile()->postSolutionInfo()->timeSteps()->timesteps().count();

	if (m_timeStart == 0 && m_timeEnd == timeStepCount - 1 && m_timeSkip == 1) {return;}
	QVector<double> clippedX, clippedY;

	for (int i = m_timeStart; i <= m_timeEnd; i += m_timeSkip) {
		clippedX.push_back(x->at(i));
		clippedY.push_back(y->at(i));
	}
	*x = clippedX;
	*y = clippedY;
}

#if SKIP
void Graph2dVerificationWindowDataModel::updateTitle()
{
	QString title = m_setting.title().trimmed();
	int currentStep = postSolutionInfo()->currentStep();
	const QList<double>& timesteps = postSolutionInfo()->timeSteps()->timesteps();
	double time;
	if (timesteps.count() == 0) {
		time = 0;
	} else {
		if (currentStep < timesteps.count()) {
			time = timesteps.at(currentStep);
		} else {
			time = 0;
		}
	}
	if (m_setting.addIndicesToTitle()) {
		QString suffix;
		Graph2dVerificationWindowResultSetting::DataTypeInfo* tinfo = m_setting.targetDataTypeInfo();
		Graph2dVerificationWindow* w = dynamic_cast<Graph2dVerificationWindow*>(mainWindow());
		Graph2dVerificationWindowControlWidget* c = w->controlWidget();
		switch (m_setting.xAxisMode()) {
		case Graph2dVerificationWindowResultSetting::xaTime:
			switch (tinfo->dataType) {
			case Graph2dVerificationWindowResultSetting::dtBaseIterative:
				suffix = "";
				break;
			case Graph2dVerificationWindowResultSetting::dtDim1DStructured:
				suffix = tr("I = %1").arg(c->iValue() + 1);
				break;
			case Graph2dVerificationWindowResultSetting::dtDim2DStructured:
				suffix = tr("I = %1, J = %2").arg(c->iValue() + 1).arg(c->jValue() + 1);
				break;
			case Graph2dVerificationWindowResultSetting::dtDim3DStructured:
				suffix = tr("I = %1, J = %2, K = %3").arg(c->iValue() + 1).arg(c->jValue() + 1).arg(c->kValue() + 1);
				break;
			case Graph2dVerificationWindowResultSetting::dtDim1DUnstructured:
			case Graph2dVerificationWindowResultSetting::dtDim2DUnstructured:
			case Graph2dVerificationWindowResultSetting::dtDim3DUnstructured:
				suffix = tr("Index = %1").arg(c->indexValue());
				break;
			}
			break;
		case Graph2dVerificationWindowResultSetting::xaI:
			switch (tinfo->dataType) {
			case Graph2dVerificationWindowResultSetting::dtDim1DStructured:
				suffix = tr("Time = %1 sec").arg(time);
				break;
			case Graph2dVerificationWindowResultSetting::dtDim2DStructured:
				suffix = tr("Time = %1 sec, J = %2").arg(time).arg(c->jValue() + 1);
				break;
			case Graph2dVerificationWindowResultSetting::dtDim3DStructured:
				suffix = tr("Time = %1 sec, J = %2, K = %3").arg(time).arg(c->jValue() + 1).arg(c->kValue() + 1);
				break;
			default:
				break;
			}
			break;
		case Graph2dVerificationWindowResultSetting::xaJ:
			switch (tinfo->dataType) {
			case Graph2dVerificationWindowResultSetting::dtDim2DStructured:
				suffix = tr("Time = %1 sec, I = %2").arg(time).arg(c->iValue() + 1);
				break;
			case Graph2dVerificationWindowResultSetting::dtDim3DStructured:
				suffix = tr("Time = %1 sec, I = %2, K = %3").arg(time).arg(c->iValue() + 1).arg(c->kValue() + 1);
				break;
			default:
				break;
			}
			break;
		case Graph2dVerificationWindowResultSetting::xaK:
			switch (tinfo->dataType) {
			case Graph2dVerificationWindowResultSetting::dtDim3DStructured:
				suffix = tr("Time = %1 sec, I = %2, J = %3").arg(time).arg(c->iValue() + 1).arg(c->jValue() + 1);
				break;
			default:
				break;
			}
			break;
		}
		title.append(" : ").append(suffix);
	}
	view()->setTitle(title);
}
#endif

bool Graph2dVerificationWindowDataModel::setupInitialSetting()
{
	PostSolutionInfo* sInfo = postSolutionInfo();
	if (! sInfo->isDataAvailable()) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("No calculation result exists."));
		return false;
	}
	// initially, setup physical value settings.
	bool loaded = m_setting.init(postSolutionInfo(), measuredData(), currentCgnsFileName());
	if (! loaded) {
		QMessageBox::critical(mainWindow(), tr("Error"), tr("Graph window setup fail. Calculation result is not loaded properly."));
		return false;
	}

	// check whether data to displayed on time window available.
	if (! m_setting.dataAvailable()) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("No calculation result exists."));
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

void Graph2dVerificationWindowDataModel::applyAxisSetting()
{
	return;
}

void Graph2dVerificationWindowDataModel::getXAxisValueRange(double* min, double* max)
{
	Graph2dWindowView* v = view();
	QwtScaleDiv sDiv = v->axisScaleDiv(QwtPlot::xBottom);
	*min = qMin(sDiv.lowerBound(), sDiv.upperBound());
	*max = qMax(sDiv.lowerBound(), sDiv.upperBound());
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
#if SKIP
	updateTitle();
#endif
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

void Graph2dVerificationWindowDataModel::showSettingDialog()
{
	Graph2dVerificationSettingDialog dialog(dynamic_cast<QWidget*>(mainWindow()->parent()));

	dialog.setSetting(m_setting);

	if (QDialog::Rejected == dialog.exec()) {return;}
	m_setting = dialog.setting();
	applySettings();
	view()->replot();
}

void Graph2dVerificationWindowDataModel::sliderChanged()
{
#if SKIP
	Graph2dVerificationWindowResultSetting::DataTypeInfo* tinfo = m_setting.targetDataTypeInfo();
	Graph2dVerificationWindow* w = dynamic_cast<Graph2dVerificationWindow*>(mainWindow());
	Graph2dVerificationWindowControlWidget* c = w->controlWidget();
	int index = 0;
	PostSolutionInfo* sol = postSolutionInfo();
	PostZoneDataContainer* cont = sol->zoneContainer(tinfo->dimension, tinfo->zoneName);
	if (cont == nullptr) {return;}
	switch (tinfo->dataType) {
	case Graph2dVerificationWindowResultSetting::dtDim1DStructured:
	case Graph2dVerificationWindowResultSetting::dtDim2DStructured:
	case Graph2dVerificationWindowResultSetting::dtDim3DStructured:
		index = cont->nodeIndex(c->iValue(), c->jValue(), c->kValue());
		break;
	default:
		break;
	}

	switch (tinfo->dataType) {
	case Graph2dVerificationWindowResultSetting::dtBaseIterative:
		break;
	case Graph2dVerificationWindowResultSetting::dtDim1DStructured:
		m_setting.setGridI(c->iValue());
		m_setting.setGridIndex(index);
		break;
	case Graph2dVerificationWindowResultSetting::dtDim2DStructured:
		m_setting.setGridI(c->iValue());
		m_setting.setGridJ(c->jValue());
		m_setting.setGridIndex(index);
		break;
	case Graph2dVerificationWindowResultSetting::dtDim3DStructured:
		m_setting.setGridI(c->iValue());
		m_setting.setGridJ(c->jValue());
		m_setting.setGridK(c->kValue());
		m_setting.setGridIndex(index);
		break;
	case Graph2dVerificationWindowResultSetting::dtDim1DUnstructured:
	case Graph2dVerificationWindowResultSetting::dtDim2DUnstructured:
	case Graph2dVerificationWindowResultSetting::dtDim3DUnstructured:
		m_setting.setGridIndex(c->indexValue());
		break;
	}
	updateData();
	updateTime();

	view()->replot();
#endif
}

void Graph2dVerificationWindowDataModel::applySettingsSlot()
{
	applySettings();
}

void Graph2dVerificationWindowDataModel::applySettings()
{
	// update axis setting.
	applyAxisSetting();

	updateData();
	updateTime();

	Graph2dVerificationWindow* w = dynamic_cast<Graph2dVerificationWindow*>(mainWindow());

#if SKIP
	// update title setting.
	updateTitle();
#endif

	w->controlWidget()->setSetting(m_setting);
	w->topWidget()->setSetting(m_setting);
}

void Graph2dVerificationWindowDataModel::updateData()
{
	int fn;
	CgnsFileOpener* opener = nullptr;
	fn = postSolutionInfo()->fileId();
	if (fn == 0) {
		// file not opened.
		QString cgnsFilename = currentCgnsFileName();
		try {
			opener = new CgnsFileOpener(iRIC::toStr(cgnsFilename), CG_MODE_READ);
			fn = opener->fileId();
		} catch (const std::runtime_error&) {
			return;
		}
	}

	updateData(fn);

	delete opener;

#if SKIP
	updateTitle();
#endif
}


void Graph2dVerificationWindowDataModel::updateData(int fn)
{
	static bool updating = false;
	if (updating == true) {
		return;
	}
	updating = true;
	Graph2dVerificationWindowRootDataItem* root = dynamic_cast<Graph2dVerificationWindowRootDataItem*>(m_rootDataItem);
	root->updateData(fn);
	updating = false;
}

//void Graph2dVerificationWindowDataModel::addKPMarkers()
//{
//	Q_ASSERT(false);
//}

void Graph2dVerificationWindowDataModel::doLoadFromProjectMainFile(const QDomNode& node)
{
	bool ok = m_setting.init(postSolutionInfo(), measuredData(), currentCgnsFileName());
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

//void Graph2dVerificationWindowDataModel::targetPolyLineDestroyed()
//{
//	Graph2dVerificationWindowProjectDataItem* item = dynamic_cast<Graph2dVerificationWindowProjectDataItem*>(parent());
//	QWidget* widget = dynamic_cast<QWidget*>(item->window()->parent());
//	widget->close();
//}

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
	vtkPointSet* ps = m_setting.activePostData()->data();
	vtkStructuredGrid* sgrid = vtkStructuredGrid::SafeDownCast(ps);

	if (sgrid == nullptr) {
		// unstructured grid.
		switch (type) {
		case 0:
			m_setting.m_graphType = Graph2dVerificationWindowResultSetting::gtMVvsCR;
			break;
		case 1:
			m_setting.m_graphType = Graph2dVerificationWindowResultSetting::gtMVvsError;
			break;
		}
	}
	else {
		// structured grid.
		switch (type) {
		case 0:
			m_setting.m_graphType = Graph2dVerificationWindowResultSetting::gtSWDvsValues;
			break;
		case 1:
			m_setting.m_graphType = Graph2dVerificationWindowResultSetting::gtSWDvsError;
			break;
		case 2:
			m_setting.m_graphType = Graph2dVerificationWindowResultSetting::gtMVvsCR;
			break;
		case 3:
			m_setting.m_graphType = Graph2dVerificationWindowResultSetting::gtMVvsError;
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
	vtkPointSet* ps = m_setting.activePostData()->data();

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

	m_pointsCurve = new QwtPlotCustomCurve();
	m_pointsCurve->setSymbol(symbol);
	m_pointsCurve->setStyle(QwtPlotCurve::NoCurve);
	m_pointsCurve->setLegendAttribute(QwtPlotCurve::LegendShowSymbol);
	m_pointsCurve->setLegendIconSize(QSize(7, 7));

	m_lineCurve = new QwtPlotCustomCurve();
	m_lineCurve->setLegendAttribute(QwtPlotCurve::LegendShowLine);

	view()->setAxisAutoScale(QwtPlot::yLeft);
	view()->setAxisAutoScale(QwtPlot::xBottom);
	switch (m_setting.m_graphType) {
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
}
