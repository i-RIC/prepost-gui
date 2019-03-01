#include "datamodel/graph2dhybridwindowdrawsettingdialog.h"
#include "datamodel/graph2dhybridwindowimportdatadataitem.h"
#include "datamodel/graph2dhybridwindowimportdatagroupdataitem.h"
#include "datamodel/graph2dhybridwindowresultcopydataitem.h"
#include "datamodel/graph2dhybridwindowresultcopygroupdataitem.h"
#include "datamodel/graph2dhybridwindowresultdataitem.h"
#include "datamodel/graph2dhybridwindowresultgroupdataitem.h"
#include "datamodel/graph2dhybridwindowrootdataitem.h"
#include "geodata/polyline/geodatapolyline.h"
#include "graph2dhybridsettingdialog.h"
#include "graph2dhybridwindow.h"
#include "graph2dhybridwindowcontinuousexportdialog.h"
#include "graph2dhybridwindowcontrolwidget.h"
#include "graph2dhybridwindowdatamodel.h"
#include "graph2dhybridwindowdatasourcedialog.h"
#include "graph2dhybridwindowprojectdataitem.h"
#include "graph2dhybridwindowview.h"

#define _USE_MATH_DEFINES
#include <cmath>

#include <qwt_plot_marker.h>
#include <qwt_scale_engine.h>

#include <gridcreatingcondition/riversurvey/gridcreatingconditionriversurvey.h>
#include <guibase/objectbrowserview.h>
#include <guicore/base/animationcontrollerinterface.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/misc/cgnsfileopener.h>
#include <guicore/post/postzoneselectingdialog.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/posttimesteps.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
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

#include <vtkStructuredGrid.h>

#include <cgnslib.h>

#include <stdexcept>

Graph2dHybridWindowDataModel::Graph2dHybridWindowDataModel(Graph2dHybridWindow* w, ProjectDataItem* parent)
	: Graph2dWindowDataModel(w, parent)
{
	init();
}

Graph2dHybridWindowDataModel::~Graph2dHybridWindowDataModel()
{
	m_objectBrowserView->blockSignals(true);
	delete m_rootDataItem;
	m_objectBrowserView->blockSignals(false);
}

void Graph2dHybridWindowDataModel::init()
{
	m_view = new Graph2dHybridWindowView(dynamic_cast<QWidget*>(parent()));

	// setup the basic itemModel structure.
	Graph2dHybridWindowRootDataItem* root = new Graph2dHybridWindowRootDataItem(dynamic_cast<Graph2dHybridWindow*>(m_mainWindow), this);
	m_rootDataItem = root;
	root->setupStandardModel(m_itemModel);
	connect(m_itemModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(handleObjectBrowserChange(QStandardItem*)));

	PostSolutionInfo* post = postSolutionInfo();
	connect(post, SIGNAL(currentStepUpdated()), this, SLOT(updateTime()));
	connect(post, SIGNAL(cgnsStepsUpdated(int)), this, SLOT(updateData(int)));

	m_regionMode = Graph2dHybridWindowContinuousExportDialog::rmCurrentOnly;
	m_timeMode = Graph2dHybridWindowContinuousExportDialog::tmCurrentOnly;
	m_prefix = "Snapshot";
	m_csvPrefix = "ExportData";

	setupTimeMarker();
}

Graph2dHybridWindowView* Graph2dHybridWindowDataModel::view() const
{
	return dynamic_cast<Graph2dHybridWindowView*>(m_view);
}

PostSolutionInfo* Graph2dHybridWindowDataModel::postSolutionInfo()
{
	return projectData()->mainfile()->postSolutionInfo();
}

void Graph2dHybridWindowDataModel::axisSetting()
{
	showSettingDialog();
}

void Graph2dHybridWindowDataModel::specialSnapshot()
{
	int dims[4];

	PostSolutionInfo* sol = postSolutionInfo();
	Graph2dHybridWindowResultSetting::DataTypeInfo* tinfo = m_setting.targetDataTypeInfo();
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
	Graph2dHybridWindowContinuousExportDialog dialog(mainWindow());
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
	case Graph2dHybridWindowResultSetting::dtBaseIterative:
		dataCount = 1;
		break;
	case Graph2dHybridWindowResultSetting::dtDim1DStructured:
		if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaTime) {
			dataCount = (m_iMax - m_iMin + 1);
		}
		break;
	case Graph2dHybridWindowResultSetting::dtDim2DStructured:
		if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaTime) {
			dataCount = (m_iMax - m_iMin + 1) * (m_jMax - m_jMin + 1);
		} else if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaI) {
			dataCount = (m_jMax - m_jMin + 1);
		} else if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaJ) {
			dataCount = (m_iMax - m_iMin + 1);
		}
		break;
	case Graph2dHybridWindowResultSetting::dtDim3DStructured:
		if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaTime) {
			dataCount = (m_iMax - m_iMin + 1) * (m_jMax - m_jMin + 1) * (m_kMax - m_kMin + 1);
		} else if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaI) {
			dataCount = (m_jMax - m_jMin + 1) * (m_kMax - m_kMin + 1);
		} else if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaJ) {
			dataCount = (m_iMax - m_iMin + 1) * (m_kMax - m_kMin + 1);
		} else if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaK) {
			dataCount = (m_iMax - m_iMin + 1) * (m_jMax - m_jMin + 1);
		}
		break;
	case Graph2dHybridWindowResultSetting::dtDim1DUnstructured:
	case Graph2dHybridWindowResultSetting::dtDim2DUnstructured:
	case Graph2dHybridWindowResultSetting::dtDim3DUnstructured:
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
		Graph2dHybridWindow* window = dynamic_cast<Graph2dHybridWindow*>(mainWindow());
		QPixmap pixmap;
		QString filename;
		bool ok;
		switch (tinfo->dataType) {
		case Graph2dHybridWindowResultSetting::dtBaseIterative:
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
		case Graph2dHybridWindowResultSetting::dtDim1DStructured:
			if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaTime) {
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
		case Graph2dHybridWindowResultSetting::dtDim2DStructured:
			if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaTime) {
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
			} else if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaI) {
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
			} else if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaJ) {
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
		case Graph2dHybridWindowResultSetting::dtDim3DStructured:
			if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaTime) {
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
			} else if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaI) {
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
			} else if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaJ) {
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
			} else if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaK) {
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
		case Graph2dHybridWindowResultSetting::dtDim1DUnstructured:
		case Graph2dHybridWindowResultSetting::dtDim2DUnstructured:
		case Graph2dHybridWindowResultSetting::dtDim3DUnstructured:
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

void Graph2dHybridWindowDataModel::specialCsvExport()
{
	int dims[4];

	PostSolutionInfo* sol = postSolutionInfo();
	Graph2dHybridWindowResultSetting::DataTypeInfo* tinfo = m_setting.targetDataTypeInfo();
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
	Graph2dHybridWindowContinuousExportDialog dialog(mainWindow());
	dialog.setSetting(m_setting, dims);
	dialog.setTimesteps(projectData()->mainfile()->postSolutionInfo()->timeSteps()->timesteps());
	dialog.setCurrentStep(projectData()->mainfile()->postSolutionInfo()->currentStep());

	dialog.setFolder(LastIODirectory::get());
	dialog.setPrefix(m_csvPrefix);
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
	dialog.setWindowTitle(tr("CSV Export Setting"));

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	QString folder = dialog.folder();
	LastIODirectory::set(folder);
	m_csvPrefix = dialog.prefix();
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
	case Graph2dHybridWindowResultSetting::dtBaseIterative:
		dataCount = 1;
		break;
	case Graph2dHybridWindowResultSetting::dtDim1DStructured:
		if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaTime) {
			dataCount = (m_iMax - m_iMin + 1);
		}
		break;
	case Graph2dHybridWindowResultSetting::dtDim2DStructured:
		if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaTime) {
			dataCount = (m_iMax - m_iMin + 1) * (m_jMax - m_jMin + 1);
		} else if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaI) {
			dataCount = (m_jMax - m_jMin + 1);
		} else if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaJ) {
			dataCount = (m_iMax - m_iMin + 1);
		}
		break;
	case Graph2dHybridWindowResultSetting::dtDim3DStructured:
		if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaTime) {
			dataCount = (m_iMax - m_iMin + 1) * (m_jMax - m_jMin + 1) * (m_kMax - m_kMin + 1);
		} else if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaI) {
			dataCount = (m_jMax - m_jMin + 1) * (m_kMax - m_kMin + 1);
		} else if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaJ) {
			dataCount = (m_iMax - m_iMin + 1) * (m_kMax - m_kMin + 1);
		} else if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaK) {
			dataCount = (m_iMax - m_iMin + 1) * (m_jMax - m_jMin + 1);
		}
		break;
	case Graph2dHybridWindowResultSetting::dtDim1DUnstructured:
	case Graph2dHybridWindowResultSetting::dtDim2DUnstructured:
	case Graph2dHybridWindowResultSetting::dtDim3DUnstructured:
		dataCount = (m_indexMax - m_indexMin + 1);
		break;
	}
	QProgressDialog pdialog(mainWindow());
	pdialog.setRange(0, timeStepCount * dataCount);
	pdialog.setWindowTitle(tr("CSV Export"));
	pdialog.setLabelText(tr("Saving csv files..."));
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
		Graph2dHybridWindow* window = dynamic_cast<Graph2dHybridWindow*>(mainWindow());
		QString filename;
		bool ok;
		switch (tinfo->dataType) {
		case Graph2dHybridWindowResultSetting::dtBaseIterative:
			filename = QDir(folder).absoluteFilePath(m_csvPrefix);
			filename.append(QString(".csv"));
			ok = exportCsv(filename);
			if (! ok) {
				showErrorMessage(filename);
				return;
			}
			++ imageIndex;
			break;
		case Graph2dHybridWindowResultSetting::dtDim1DStructured:
			if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaTime) {
				for (int i = m_iMin; i <= m_iMax; ++i) {
					window->controlWidget()->setIValue(i);
					filename = QDir(folder).absoluteFilePath(m_csvPrefix);
					filename.append(QString("_I=%1.csv").arg(formattedNumber(i + 1, m_iMax)));
					ok = exportCsv(filename);
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
		case Graph2dHybridWindowResultSetting::dtDim2DStructured:
			if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaTime) {
				for (int i = m_iMin; i <= m_iMax; ++i) {
					window->controlWidget()->setIValue(i);
					for (int j = m_jMin; j <= m_jMax; ++j) {
						window->controlWidget()->setJValue(j);
						filename = QDir(folder).absoluteFilePath(m_csvPrefix);
						filename.append(QString("_I=%1_J=%2.csv").arg(formattedNumber(i + 1, m_iMax)).arg(formattedNumber(j + 1, m_jMax)));
						ok = exportCsv(filename);
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
			} else if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaI) {
				for (int j = m_jMin; j <= m_jMax; ++j) {
					window->controlWidget()->setJValue(j);
					filename = QDir(folder).absoluteFilePath(m_csvPrefix);
					filename.append(QString("_Time=%1_J=%2.csv").arg(formattedNumber(timeStep + 1, m_timeEnd)).arg(formattedNumber(j + 1, m_jMax)));
					ok = exportCsv(filename);
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
			} else if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaJ) {
				for (int i = m_iMin; i <= m_iMax; ++i) {
					window->controlWidget()->setIValue(i);
					filename = QDir(folder).absoluteFilePath(m_csvPrefix);
					filename.append(QString("_Time=%1_I=%2.csv").arg(formattedNumber(timeStep + 1, m_timeEnd)).arg(formattedNumber(i + 1, m_iMax)));
					ok = exportCsv(filename);
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
		case Graph2dHybridWindowResultSetting::dtDim3DStructured:
			if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaTime) {
				for (int i = m_iMin; i <= m_iMax; ++i) {
					window->controlWidget()->setIValue(i);
					for (int j = m_jMin; j <= m_jMax; ++j) {
						window->controlWidget()->setJValue(j);
						for (int k = m_kMin; k <= m_kMax; ++k) {
							window->controlWidget()->setKValue(k);
							filename = QDir(folder).absoluteFilePath(m_csvPrefix);
							filename.append(QString("_I=%1_J=%2_K=%3.csv").arg(formattedNumber(i + 1, m_iMax)).arg(formattedNumber(j + 1, m_jMax)).arg(formattedNumber(k + 1, m_kMax)));
							ok = exportCsv(filename);
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
			} else if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaI) {
				for (int j = m_jMin; j <= m_jMax; ++j) {
					window->controlWidget()->setJValue(j);
					for (int k = m_kMin; k <= m_kMax; ++k) {
						window->controlWidget()->setKValue(k);
						filename = QDir(folder).absoluteFilePath(m_csvPrefix);
						filename.append(QString("_Time=%1_J=%2_K=%3.csv").arg(formattedNumber(timeStep + 1, m_timeEnd)).arg(formattedNumber(j + 1, m_jMax)).arg(formattedNumber(k + 1, m_kMax)));
						ok = exportCsv(filename);
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
			} else if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaJ) {
				for (int i = m_iMin; i <= m_iMax; ++i) {
					window->controlWidget()->setIValue(i);
					for (int k = m_kMin; k <= m_kMax; ++k) {
						window->controlWidget()->setKValue(k);
						filename = QDir(folder).absoluteFilePath(m_csvPrefix);
						filename.append(QString("_Time=%1_I=%2_K=%3.csv").arg(formattedNumber(timeStep + 1, m_timeEnd)).arg(formattedNumber(i + 1, m_iMax)).arg(formattedNumber(k + 1, m_kMax)));
						ok = exportCsv(filename);
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
			} else if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaK) {
				for (int i = m_iMin; i <= m_iMax; ++i) {
					window->controlWidget()->setIValue(i);
					for (int j = m_jMin; j <= m_jMax; ++j) {
						window->controlWidget()->setJValue(j);
						filename = QDir(folder).absoluteFilePath(m_csvPrefix);
						filename.append(QString("_Time=%1_I=%2_J=%3.csv").arg(formattedNumber(timeStep + 1, m_timeEnd)).arg(formattedNumber(i + 1, m_iMax)).arg(formattedNumber(j + 1, m_jMax)));
						ok = exportCsv(filename);
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
		case Graph2dHybridWindowResultSetting::dtDim1DUnstructured:
		case Graph2dHybridWindowResultSetting::dtDim2DUnstructured:
		case Graph2dHybridWindowResultSetting::dtDim3DUnstructured:
			for (int idx = m_indexMin; idx <= m_indexMax; ++idx) {
				window->controlWidget()->setIndexValue(idx);
				filename = QDir(folder).absoluteFilePath(m_csvPrefix);
				filename.append(QString("_Time=%1_Index=%2.csv").arg(formattedNumber(timeStep + 1, m_timeEnd)).arg(formattedNumber(idx + 1, m_indexMax)));
				ok = exportCsv(filename);
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

bool Graph2dHybridWindowDataModel::savePixmap(const QPixmap& pixmap, const QString& filename)
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

void Graph2dHybridWindowDataModel::showErrorMessage(const QString& filename)
{
	QMessageBox::critical(mainWindow(), tr("Error"), tr("Saving snapshot image to %1 failed.").arg(filename));
}

QString Graph2dHybridWindowDataModel::formattedNumber(int number, int max)
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

void Graph2dHybridWindowDataModel::copyCalculationResult()
{
	Graph2dHybridWindowRootDataItem* root = dynamic_cast<Graph2dHybridWindowRootDataItem*>(m_rootDataItem);
	Graph2dHybridWindowResultCopyGroupDataItem* cgroup = root->resultCopyGroupItem();
	Graph2dHybridWindowResultGroupDataItem* rgroup = root->resultGroupItem();
	rgroup->copy(cgroup);
	cgroup->updateData();
	root->renderView();
}

void Graph2dHybridWindowDataModel::drawSetting()
{
	Graph2dHybridWindowDrawSettingDialog dialog(mainWindow());
	Graph2dHybridWindowRootDataItem* root = dynamic_cast<Graph2dHybridWindowRootDataItem*>(m_rootDataItem);
	Graph2dHybridWindowResultGroupDataItem* rgroup = root->resultGroupItem();
	QList <Graph2dWindowDataItem*> resultList = rgroup->childItems();
	QList<Graph2dHybridWindowResultSetting::Setting> resultSettings;
	for (int i = 0; i < resultList.count(); ++i) {
		Graph2dHybridWindowResultDataItem* item = dynamic_cast<Graph2dHybridWindowResultDataItem*>(resultList.at(i));
		resultSettings.append(item->setting());
	}
	dialog.setResultSettings(resultSettings);

	Graph2dHybridWindowResultCopyGroupDataItem* cgroup = root->resultCopyGroupItem();
	QList <Graph2dWindowDataItem*> resultCopyList = cgroup->childItems();
	QList<Graph2dHybridWindowResultSetting::Setting> copySettings;
	QList<QString> copyIds;
	for (int i = 0; i < resultCopyList.count(); ++i) {
		Graph2dHybridWindowResultCopyDataItem* cdi = dynamic_cast<Graph2dHybridWindowResultCopyDataItem*>(resultCopyList.at(i));
		copySettings.append(cdi->setting());
		copyIds.append(cdi->id());
	}
	dialog.setCopySettings(copySettings, copyIds);

	Graph2dHybridWindowImportDataGroupDataItem* igroup = root->importDataGroupItem();
	QList <Graph2dWindowDataItem*> importDataList = igroup->childItems();
	QList<Graph2dHybridWindowResultSetting::Setting> importDataSettings;
	QList<QString> importDataIds;
	for (int i = 0; i < importDataList.count(); ++i) {
		Graph2dHybridWindowImportDataDataItem* ddi = dynamic_cast<Graph2dHybridWindowImportDataDataItem*>(importDataList.at(i));
		importDataSettings.append(ddi->setting());
		importDataIds.append(ddi->id());
	}
	dialog.setImportDataSettings(importDataSettings, importDataIds);

	int ret = dialog.exec();

	if (ret == QDialog::Rejected) {return;}

	resultSettings = dialog.resultSettings();
	for (int i = 0; i < resultList.count(); ++i) {
		Graph2dHybridWindowResultDataItem* item = dynamic_cast<Graph2dHybridWindowResultDataItem*>(resultList.at(i));
		item->setSetting(resultSettings.at(i));
	}
	m_setting.targetDatas() = resultSettings;

	copySettings = dialog.copySettings();
	for (int i = 0; i < resultCopyList.count(); ++i) {
		Graph2dHybridWindowResultCopyDataItem* item = dynamic_cast<Graph2dHybridWindowResultCopyDataItem*>(resultCopyList.at(i));
		item->setSetting(copySettings.at(i));
	}

	QList<bool> copyDeleted = dialog.copyDeleted();
	for (int i = copyDeleted.count() - 1; i >= 0; --i) {
		bool deleted = copyDeleted[i];
		if (deleted) {
			delete resultCopyList.at(i);
		}
	}

	importDataSettings = dialog.importDataSettings();
	for (int i = 0; i < importDataList.count(); ++i) {
		Graph2dHybridWindowImportDataDataItem* item = dynamic_cast<Graph2dHybridWindowImportDataDataItem*>(importDataList.at(i));
		item->setSetting(importDataSettings.at(i));
	}

	QList<bool> importDataDeleted = dialog.importDataDeleted();
	for (int i = importDataDeleted.count() - 1; i >= 0; --i) {
		bool deleted = importDataDeleted[i];
		if (deleted) {
			delete importDataList.at(i);
		}
	}
	updateData();
	applyAxisSetting();
	root->renderView();
}

void Graph2dHybridWindowDataModel::markerSettiing()
{
	Graph2dHybridWindowRootDataItem* root = dynamic_cast<Graph2dHybridWindowRootDataItem*>(m_rootDataItem);
	root->markerGroupItem()->showPropertyDialog();
}

void Graph2dHybridWindowDataModel::exportCsv()
{
	iRICMainWindowInterface* mainW = projectData()->mainWindow();
	QString iodir = LastIODirectory::get();
	if (mainW->isSolverRunning()) {
		mainW->warnSolverRunning();
		return;
	}
	QString fname = QFileDialog::getSaveFileName(
										mainW, tr("Export CSV file"), iodir, tr("CSV file (*.csv)"));
	if (fname == "") {return;}

	bool ok = exportCsv(fname);
	if (! ok) {
		QMessageBox::critical(mainW, tr("Error"), tr("Saving %1 failed.").arg(QDir::toNativeSeparators(fname)));
		return;
	}
	mainW->statusBar()->showMessage(tr("CSV file successfully exported."), iRICMainWindowInterface::STATUSBAR_DISPLAYTIME);
	QFileInfo finfo(fname);
	LastIODirectory::set(finfo.absolutePath());
}

template <typename DataItem>
void Graph2dHybridWindowDataModel::getXY(DataItem* dataItem, QVector<double>* x, QVector<double>* y) const
{
	*x = dataItem->xValues();
	*y = dataItem->yValues();
	if (m_setting.xAxisMode() != Graph2dHybridWindowResultSetting::xaTime) {return;}
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

bool Graph2dHybridWindowDataModel::exportCsv(const QString& filename) const
{
	int maxCount = 0;
	QList<QString> titles;
	QList<QVector<double> > values;
	QVector<double> prevX;

	Graph2dHybridWindowRootDataItem* root = dynamic_cast<Graph2dHybridWindowRootDataItem*>(m_rootDataItem);
	Graph2dHybridWindowResultGroupDataItem* rgroup = root->resultGroupItem();
	QList <Graph2dWindowDataItem*> resultList = rgroup->childItems();
	for (int i = 0; i < resultList.count(); ++i) {
		Graph2dHybridWindowResultDataItem* item = dynamic_cast<Graph2dHybridWindowResultDataItem*>(resultList.at(i));
		QVector<double> tmpX, tmpY;
		getXY(item, &tmpX, &tmpY);
		if (prevX != tmpX) {
			titles.append("X");
			values.append(tmpX);
			prevX = tmpX;
		}
		titles.append(item->title());
		values.append(tmpY);
		if (maxCount < tmpY.count()) {maxCount = tmpY.count();}
	}
	Graph2dHybridWindowResultCopyGroupDataItem* cgroup = root->resultCopyGroupItem();
	QList <Graph2dWindowDataItem*> resultCopyList = cgroup->childItems();
	for (int i = 0; i < resultCopyList.count(); ++i) {
		Graph2dHybridWindowResultCopyDataItem* cdi = dynamic_cast<Graph2dHybridWindowResultCopyDataItem*>(resultCopyList.at(i));
		QVector<double> tmpX, tmpY;
		getXY(cdi, &tmpX, &tmpY);
		if (prevX != tmpX) {
			titles.append("X");
			values.append(tmpX);
		}
		titles.append(cdi->title());
		values.append(tmpY);
		if (maxCount < tmpY.count()) {maxCount = tmpY.count();}
	}

	Graph2dHybridWindowImportDataGroupDataItem* igroup = root->importDataGroupItem();
	QList <Graph2dWindowDataItem*> importDataList = igroup->childItems();
	for (int i = 0; i < importDataList.count(); ++i) {
		Graph2dHybridWindowImportDataDataItem* ddi = dynamic_cast<Graph2dHybridWindowImportDataDataItem*>(importDataList.at(i));
		QVector<double> tmpX = ddi->xValues();
		QVector<double> tmpY = ddi->yValues();
		if (prevX != tmpX) {
			titles.append("X");
			values.append(tmpX);
		}
		titles.append(ddi->title());
		values.append(tmpY);
		if (maxCount < tmpY.count()) {maxCount = tmpY.count();}
	}

	QFile csvFile(filename);
	bool ok = csvFile.open(QIODevice::WriteOnly);
	if (! ok) {return false;}
	QTextStream stream(&csvFile);

	// output header
	for (int i = 0; i < titles.count(); ++i) {
		if (i != 0) { stream << ",";}
		stream << "\"" << titles.at(i) << "\"";

	}
	stream << "\r\n";
	for (int i = 0; i < maxCount; ++i) {
		QList<QString> tmpvals;
		for (int j = 0; j < values.count(); ++j) {
			const QVector<double>& vals = values.at(j);
			if (i < vals.count()) {
				double v = vals.at(i);
				tmpvals.append(QString::number(v));
			} else {
				tmpvals.append("");
			}
		}
		for (int j = 0; j < tmpvals.count(); ++j) {
			if (j != 0) { stream << ",";}
			stream << tmpvals.at(j);
		}
		stream << "\r\n";
	}
	csvFile.close();
	return true;
}

void Graph2dHybridWindowDataModel::setupTimeMarker()
{
	m_timeMarker = new QwtPlotMarker();
	m_timeMarker->setAxes(QwtPlot::xBottom, QwtPlot::yLeft);
	m_timeMarker->setLineStyle(QwtPlotMarker::VLine);

	QPen pen;
	pen.setStyle(Qt::SolidLine);
	pen.setColor(Qt::red);
	m_timeMarker->setLinePen(pen);
}

void Graph2dHybridWindowDataModel::updateTitle()
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
		Graph2dHybridWindowResultSetting::DataTypeInfo* tinfo = m_setting.targetDataTypeInfo();
		Graph2dHybridWindow* w = dynamic_cast<Graph2dHybridWindow*>(mainWindow());
		Graph2dHybridWindowControlWidget* c = w->controlWidget();
		switch (m_setting.xAxisMode()) {
		case Graph2dHybridWindowResultSetting::xaTime:
			switch (tinfo->dataType) {
			case Graph2dHybridWindowResultSetting::dtBaseIterative:
				suffix = "";
				break;
			case Graph2dHybridWindowResultSetting::dtDim1DStructured:
				suffix = tr("I = %1").arg(c->iValue() + 1);
				break;
			case Graph2dHybridWindowResultSetting::dtDim2DStructured:
				suffix = tr("I = %1, J = %2").arg(c->iValue() + 1).arg(c->jValue() + 1);
				break;
			case Graph2dHybridWindowResultSetting::dtDim3DStructured:
				suffix = tr("I = %1, J = %2, K = %3").arg(c->iValue() + 1).arg(c->jValue() + 1).arg(c->kValue() + 1);
				break;
			case Graph2dHybridWindowResultSetting::dtDim1DUnstructured:
			case Graph2dHybridWindowResultSetting::dtDim2DUnstructured:
			case Graph2dHybridWindowResultSetting::dtDim3DUnstructured:
				suffix = tr("Index = %1").arg(c->indexValue());
				break;
			}
			break;
		case Graph2dHybridWindowResultSetting::xaI:
			switch (tinfo->dataType) {
			case Graph2dHybridWindowResultSetting::dtDim1DStructured:
				suffix = tr("Time = %1 sec").arg(time);
				break;
			case Graph2dHybridWindowResultSetting::dtDim2DStructured:
				suffix = tr("Time = %1 sec, J = %2").arg(time).arg(c->jValue() + 1);
				break;
			case Graph2dHybridWindowResultSetting::dtDim3DStructured:
				suffix = tr("Time = %1 sec, J = %2, K = %3").arg(time).arg(c->jValue() + 1).arg(c->kValue() + 1);
				break;
			default:
				break;
			}
			break;
		case Graph2dHybridWindowResultSetting::xaJ:
			switch (tinfo->dataType) {
			case Graph2dHybridWindowResultSetting::dtDim2DStructured:
				suffix = tr("Time = %1 sec, I = %2").arg(time).arg(c->iValue() + 1);
				break;
			case Graph2dHybridWindowResultSetting::dtDim3DStructured:
				suffix = tr("Time = %1 sec, I = %2, K = %3").arg(time).arg(c->iValue() + 1).arg(c->kValue() + 1);
				break;
			default:
				break;
			}
			break;
		case Graph2dHybridWindowResultSetting::xaK:
			switch (tinfo->dataType) {
			case Graph2dHybridWindowResultSetting::dtDim3DStructured:
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

bool Graph2dHybridWindowDataModel::setupInitialSetting()
{
	PostSolutionInfo* sInfo = postSolutionInfo();
	if (! sInfo->isDataAvailable()) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("No calculation result exists."));
		return false;
	}
	// initially, setup physical value settings.
	bool loaded = m_setting.init(postSolutionInfo(), currentCgnsFileName());
	if (! loaded) {
		QMessageBox::critical(mainWindow(), tr("Error"), tr("Graph window setup fail. Calculation result is not loaded properly."));
		return false;
	}

	// check whether data to displayed on time window available.
	if (! m_setting.dataAvailable()) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("No calculation result exists."));
		return false;
	}

	Graph2dHybridWindowDataSourceDialog* dialog = new Graph2dHybridWindowDataSourceDialog(mainWindow());
	Graph2dHybridWindowRootDataItem* rItem = dynamic_cast<Graph2dHybridWindowRootDataItem*>(m_rootDataItem);
	Graph2dHybridWindowImportDataGroupDataItem* gItem = rItem->importDataGroupItem();

	dialog->setMainWindow(projectData()->mainWindow());
	dialog->setSetting(m_setting);
	dialog->setImportData(gItem);
	int ret = dialog->exec();
	if (ret == QDialog::Rejected) {
		return false;
	}
	m_setting = dialog->setting();
	rItem->resultGroupItem()->updateChildren(m_setting);

	applySettings();
	view()->replot();
	delete dialog;

	return true;
}

void Graph2dHybridWindowDataModel::applyAxisSetting()
{
	Graph2dWindowView* v = view();
	if (m_setting.xAxisLog()) {
		v->setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine());
	} else {
		v->setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine());
	}
	if (m_setting.xAxisReverse() && m_setting.xAxisMode() != Graph2dHybridWindowResultSetting::xaTime) {
		v->axisScaleEngine(QwtPlot::xBottom)->setAttribute(QwtScaleEngine::Inverted, true);
	} else {
		v->axisScaleEngine(QwtPlot::xBottom)->setAttribute(QwtScaleEngine::Inverted, false);
	}
	if (m_setting.xAxisAutoRange()) {
		v->setAxisAutoScale(QwtPlot::xBottom);
	} else {
		if (m_setting.xAxisReverse()) {
			v->setAxisScale(QwtPlot::xBottom, m_setting.xAxisValueMax(), m_setting.xAxisValueMin());
		} else {
			v->setAxisScale(QwtPlot::xBottom, m_setting.xAxisValueMin(), m_setting.xAxisValueMax());
		}
	}
	v->setAxisTitle(QwtPlot::xBottom, m_setting.xAxisLabel());

	Graph2dHybridWindowRootDataItem* root = dynamic_cast<Graph2dHybridWindowRootDataItem*>(m_rootDataItem);
	if (root->axisNeeded(Graph2dHybridWindowResultSetting::asLeft)) {
		v->enableAxis(QwtPlot::yLeft);
		v->setAxisTitle(QwtPlot::yLeft, m_setting.yAxisLeftTitle());
		if (m_setting.yAxisLeftLog()) {
			v->setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine());
		} else {
			v->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine());
		}
		// control range.
		if (m_setting.yAxisLeftAutoRange()) {
			v->setAxisAutoScale(QwtPlot::yLeft);
		} else {
			if (m_setting.yAxisLeftReverse()) {
				v->setAxisScale(QwtPlot::yLeft, m_setting.yAxisLeftMax(), m_setting.yAxisLeftMin());
			} else {
				v->setAxisScale(QwtPlot::yLeft, m_setting.yAxisLeftMin(), m_setting.yAxisLeftMax());
			}
		}
		v->axisScaleEngine(QwtPlot::yLeft)->setAttribute(QwtScaleEngine::Inverted, m_setting.yAxisLeftReverse());
	} else {
		v->enableAxis(QwtPlot::yLeft, false);
	}
	if (root->axisNeeded(Graph2dHybridWindowResultSetting::asRight)) {
		v->enableAxis(QwtPlot::yRight);
		v->setAxisTitle(QwtPlot::yRight, m_setting.yAxisRightTitle());
		if (m_setting.yAxisRightLog()) {
			v->setAxisScaleEngine(QwtPlot::yRight, new QwtLogScaleEngine());
		} else {
			v->setAxisScaleEngine(QwtPlot::yRight, new QwtLinearScaleEngine());
		}
		// control range.
		if (m_setting.yAxisRightAutoRange()) {
			v->setAxisAutoScale(QwtPlot::yRight);
		} else {
			if (m_setting.yAxisRightReverse()) {
				v->setAxisScale(QwtPlot::yRight, m_setting.yAxisRightMax(), m_setting.yAxisRightMin());
			} else {
				v->setAxisScale(QwtPlot::yRight, m_setting.yAxisRightMin(), m_setting.yAxisRightMax());
			}
		}
		v->axisScaleEngine(QwtPlot::yRight)->setAttribute(QwtScaleEngine::Inverted, m_setting.yAxisRightReverse());
	} else {
		v->enableAxis(QwtPlot::yRight, false);
	}
}

void Graph2dHybridWindowDataModel::getXAxisValueRange(double* min, double* max)
{
	Graph2dWindowView* v = view();
	QwtScaleDiv sDiv = v->axisScaleDiv(QwtPlot::xBottom);
	*min = qMin(sDiv.lowerBound(), sDiv.upperBound());
	*max = qMax(sDiv.lowerBound(), sDiv.upperBound());
}

void Graph2dHybridWindowDataModel::getYAxisValueRange(Graph2dWindowDataModel::AxisSide as, double* min, double* max)
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

void Graph2dHybridWindowDataModel::updateTime()
{
	if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaTime) {
		int currentStep = postSolutionInfo()->currentStep();
		const QList<double>& timesteps = postSolutionInfo()->timeSteps()->timesteps();
		if (timesteps.count() == 0) {
			m_timeMarker->setXValue(0);
		} else {
			if (currentStep < timesteps.count()) {
				if (m_setting.timeValueType() == Graph2dHybridWindowResultSetting::tvtTime) {
					m_timeMarker->setXValue(timesteps.at(currentStep));
				} else {
					m_timeMarker->setXValue(currentStep + 1);
				}
			} else {
				m_timeMarker->setXValue(0);
			}
		}
	} else {
		updateData();
	}
	updateTitle();
	view()->replot();
}

void Graph2dHybridWindowDataModel::dataSourceSetting()
{
	Graph2dHybridWindowDataSourceDialog* dialog = new Graph2dHybridWindowDataSourceDialog(mainWindow());
	Graph2dHybridWindowRootDataItem* rItem = dynamic_cast<Graph2dHybridWindowRootDataItem*>(m_rootDataItem);
	Graph2dHybridWindowImportDataGroupDataItem* gItem = rItem->importDataGroupItem();

	dialog->setMainWindow(projectData()->mainWindow());
	dialog->setSetting(m_setting);
	dialog->setImportData(gItem);
	int ret = dialog->exec();

	if (ret == QDialog::Rejected) {return;}

	const Graph2dHybridWindowResultSetting& newSetting = dialog->setting();
	if (m_setting.xAxisMode() != newSetting.xAxisMode()) {
		// X axis is changed. clear all copys.
		rItem->resultCopyGroupItem()->clear();
	}
	m_setting = newSetting;

	rItem->resultGroupItem()->updateChildren(m_setting);

	applySettings();
	view()->replot();

	delete dialog;
}

void Graph2dHybridWindowDataModel::showSettingDialog()
{
	Graph2dHybridSettingDialog dialog(mainWindow());
	if (m_setting.yAxisLeftAutoRange()) {
		double min, max;
		getYAxisValueRange(Graph2dWindowDataModel::asLeft, &min, &max);
		m_setting.setYAxisLeftMin(min);
		m_setting.setYAxisLeftMax(max);
	}
	if (m_setting.yAxisRightAutoRange()) {
		double min, max;
		getYAxisValueRange(Graph2dWindowDataModel::asRight, &min, &max);
		m_setting.setYAxisRightMin(min);
		m_setting.setYAxisRightMax(max);
	}
	int dims[4];
	PostSolutionInfo* sol = postSolutionInfo();
	Graph2dHybridWindowResultSetting::DataTypeInfo* tinfo = m_setting.targetDataTypeInfo();
	PostZoneDataContainer* cont = sol->zoneContainer(tinfo->dimension, tinfo->zoneName);
	if (cont != nullptr) {
		vtkStructuredGrid* sGrid = dynamic_cast<vtkStructuredGrid*>(cont->data());
		if (sGrid != nullptr) {
			// structured
			sGrid->GetDimensions(dims);
			if (tinfo->gridLocation == CellCenter) {
				sGrid->GetCellDims(dims);
			}
		}
	}
	if (m_setting.xAxisAutoRange()) {
		double min, max;
		getXAxisValueRange(&min, &max);
		m_setting.setXAxisValueMin(min);
		m_setting.setXAxisValueMax(max);
	}
	dialog.setSetting(m_setting);

	if (QDialog::Rejected == dialog.exec()) {return;}
	m_setting = dialog.setting();
	applySettings();
	view()->replot();
}

void Graph2dHybridWindowDataModel::sliderChanged()
{
	Graph2dHybridWindowResultSetting::DataTypeInfo* tinfo = m_setting.targetDataTypeInfo();
	Graph2dHybridWindow* w = dynamic_cast<Graph2dHybridWindow*>(mainWindow());
	Graph2dHybridWindowControlWidget* c = w->controlWidget();
	int index = 0;
	PostSolutionInfo* sol = postSolutionInfo();
	PostZoneDataContainer* cont = sol->zoneContainer(tinfo->dimension, tinfo->zoneName);
	if (cont == nullptr) {return;}
	switch (tinfo->dataType) {
	case Graph2dHybridWindowResultSetting::dtDim1DStructured:
	case Graph2dHybridWindowResultSetting::dtDim2DStructured:
	case Graph2dHybridWindowResultSetting::dtDim3DStructured:
		index = cont->nodeIndex(c->iValue(), c->jValue(), c->kValue());
		break;
	default:
		break;
	}

	switch (tinfo->dataType) {
	case Graph2dHybridWindowResultSetting::dtBaseIterative:
		break;
	case Graph2dHybridWindowResultSetting::dtDim1DStructured:
		m_setting.setGridI(c->iValue());
		m_setting.setGridIndex(index);
		break;
	case Graph2dHybridWindowResultSetting::dtDim2DStructured:
		m_setting.setGridI(c->iValue());
		m_setting.setGridJ(c->jValue());
		m_setting.setGridIndex(index);
		break;
	case Graph2dHybridWindowResultSetting::dtDim3DStructured:
		m_setting.setGridI(c->iValue());
		m_setting.setGridJ(c->jValue());
		m_setting.setGridK(c->kValue());
		m_setting.setGridIndex(index);
		break;
	case Graph2dHybridWindowResultSetting::dtDim1DUnstructured:
	case Graph2dHybridWindowResultSetting::dtDim2DUnstructured:
	case Graph2dHybridWindowResultSetting::dtDim3DUnstructured:
		m_setting.setGridIndex(c->indexValue());
		break;
	}
	updateData();
	updateTime();

	view()->replot();
}

void Graph2dHybridWindowDataModel::applySettingsSlot()
{
	applySettings();
}

void Graph2dHybridWindowDataModel::applySettings()
{
	// update axis setting.
	applyAxisSetting();

	updateData();
	updateTime();

	int dims[4];

	PostSolutionInfo* sol = postSolutionInfo();
	Graph2dHybridWindowResultSetting::DataTypeInfo* tinfo = m_setting.targetDataTypeInfo();
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
	Graph2dHybridWindow* w = dynamic_cast<Graph2dHybridWindow*>(mainWindow());
	Graph2dWindowView* v = view();

	// update title setting.
	updateTitle();

	w->controlWidget()->setSetting(m_setting, dims);
	if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaTime) {
		m_timeMarker->attach(v);
	} else {
		m_timeMarker->detach();
	}
}

void Graph2dHybridWindowDataModel::updateData()
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

	updateTitle();
}


void Graph2dHybridWindowDataModel::updateData(int fn)
{
	static bool updating = false;
	if (updating == true) {
		return;
	}
	updating = true;
	Graph2dHybridWindowRootDataItem* root = dynamic_cast<Graph2dHybridWindowRootDataItem*>(m_rootDataItem);
	root->updateData(fn);
	updating = false;
}

void Graph2dHybridWindowDataModel::addKPMarkers()
{
	QList<Graph2dWindowMarkerSetting::Graph2dWindowMarkerSettingItem> items;
	Graph2dHybridWindowResultSetting::DataTypeInfo* info = 0;
	PreProcessorGridAndGridCreatingConditionDataItemInterface* citem = 0;
	PreProcessorGridCreatingConditionDataItemInterface* condItem = 0;
	GridCreatingCondition* cond = 0;
	GridCreatingConditionRiverSurvey* condrs = 0;

	Graph2dHybridWindowRootDataItem* root = dynamic_cast<Graph2dHybridWindowRootDataItem*>(m_rootDataItem);
	Graph2dHybridWindowResultDataItem* ditem = dynamic_cast<Graph2dHybridWindowResultDataItem*>(root->resultGroupItem()->childItems().at(0));
	QVector<double> xvalues = ditem->xValues();
	int iValue = 0;
	GeoDataRiverPathPoint* start = 0;
	GeoDataRiverPathPoint* end = 0;
	GeoDataRiverPathPoint* pp = 0;

	if (m_setting.xAxisMode() != Graph2dHybridWindowResultSetting::xaI) {goto CONDITIONERROR;}
	info = m_setting.targetDataTypeInfo();
	if (info->gridType == nullptr) {goto CONDITIONERROR;}
	citem = projectData()->mainWindow()->preProcessorWindow()->dataModel()->getGridAndGridCreatingConditionDataItem(info->gridType->name(), info->zoneName);
	if (citem == nullptr) {goto CONDITIONERROR;}
	condItem = citem->creatingConditionDataItem();
	cond = condItem->condition();
	if (cond == nullptr) {goto CONDITIONERROR;}
	condrs = dynamic_cast<GridCreatingConditionRiverSurvey*>(cond);
	if (condrs == nullptr) {goto CONDITIONERROR;}
	start = condrs->lastStartPoint();
	end = condrs->lastEndPoint();
	pp = start;

	while (pp != end) {
		double val = xvalues.at(iValue);
		QString name = "KP %1";

		Graph2dWindowMarkerSetting::Graph2dWindowMarkerSettingItem item;
		item.setAxis(QwtPlot::xBottom);
		item.setShowLine(false);
		item.setAlignCenter(true);
		item.setColor(Qt::black);
		item.setValue(val);
		item.setLabelMode(Graph2dWindowMarkerSetting::lmCustom);
		item.setLabel(name.arg(pp->name()));
		item.setCustomLabel(name.arg(pp->name()));
		item.setOrientation(Graph2dWindowMarkerSetting::oVertical);
		items.append(item);

		iValue += (pp->CenterLineCtrlPoints.count() + 1);
		pp = pp->nextPoint();
	}
	if (pp != nullptr) {
		double val = xvalues.at(iValue);
		QString name = "KP %1";

		Graph2dWindowMarkerSetting::Graph2dWindowMarkerSettingItem item;

		item.setAxis(QwtPlot::xBottom);
		item.setShowLine(false);
		item.setAlignCenter(true);
		item.setColor(Qt::black);
		item.setValue(val);
		item.setLabelMode(Graph2dWindowMarkerSetting::lmCustom);
		item.setLabel(name.arg(pp->name()));
		item.setCustomLabel(name.arg(pp->name()));
		item.setOrientation(Graph2dWindowMarkerSetting::oVertical);

		items.append(item);
	}
	root->markerGroupItem()->addMarkers(items);
	m_view->replot();
	return;

CONDITIONERROR:
	QMessageBox::warning(mainWindow(), tr("Warning"),
											 tr("This function can be used when the following conditions are satisfied: <ul>"
													"<li>Graph for two-dimensional structured grid result is drawn.</li>"
													"<li>X-axis is I-direction in the grid.</li>"
													"<li>The grid is created using the algorithm \"Create grid from from river survey data\"</li>"
													"</ul>"));
	return;

}

void Graph2dHybridWindowDataModel::doLoadFromProjectMainFile(const QDomNode& node)
{
	bool ok = m_setting.init(postSolutionInfo(), currentCgnsFileName());
	if (! ok) {
		throw ErrorMessage("No solution found.");
	}
	QDomNode sNode = iRIC::getChildNode(node, "Setting");
	if (! sNode.isNull()) {
		m_setting.loadFromProjectMainFile(sNode);
	}
	Graph2dWindowDataModel::doLoadFromProjectMainFile(node);

	Graph2dHybridWindowRootDataItem* rItem = dynamic_cast<Graph2dHybridWindowRootDataItem*>(m_rootDataItem);
	rItem->resultGroupItem()->updateChildren(m_setting);

	applySettings();
	updateData();
	view()->replot();
}

void Graph2dHybridWindowDataModel::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	Graph2dWindowDataModel::doSaveToProjectMainFile(writer);
	writer.writeStartElement("Setting");
	m_setting.saveToProjectMainFile(writer);
	writer.writeEndElement();
}

void Graph2dHybridWindowDataModel::targetPolyLineDestroyed()
{
	Graph2dHybridWindowProjectDataItem* item = dynamic_cast<Graph2dHybridWindowProjectDataItem*>(parent());
	QWidget* widget = dynamic_cast<QWidget*>(item->window()->parent());
	widget->close();
}
