#include "postsolutioninfo.h"
#include "postiterationsteps.h"
#include "posttimesteps.h"
#include "postzonedatacontainer.h"
#include "postdummy3dzonedatacontainer.h"
#include "../project/projectdata.h"
#include "../solverdef/solverdefinition.h"
#include "../solverdef/solverdefinitiongridtype.h"
#include <misc/stringtool.h>
#include <misc/mathsupport.h>
#include "postbaseselectingdialog.h"
#include "../solverdef/solverdefinitiongridtype.h"
#include "postdataexportdialog.h"
#include "../base/iricmainwindowinterface.h"
#include <guibase/itemselectingdialog.h>

#include <misc/lastiodirectory.h>

#include <cgnslib.h>
#include <iriclib.h>
#include <QXmlStreamWriter>
#include <QTimerEvent>
#include <QThread>
#include <QTime>
#include <QMessageBox>
#include <QProgressDialog>
#include <QCoreApplication>

#include <vtkStructuredGrid.h>

PostSolutionInfo::PostSolutionInfo(ProjectDataItem* parent)
	: ProjectDataItem(parent)
{
	m_currentStep = 0;
	m_iterationSteps = nullptr;
	m_timeSteps = nullptr;
	m_timerId = 0;
	m_fileId = 0;
	m_iterationType = SolverDefinition::NoIteration;

	m_exportFormat = efVTK;

	m_exportAllSteps = true;
	m_exportStartStep = 0;
	m_exportEndStep = 0;

	m_exportFullRange = true;
	m_exportIMin = 0;
	m_exportIMax = 0;
	m_exportJMin = 0;
	m_exportJMax = 0;
	m_exportKMin = 0;
	m_exportKMax = 0;
	m_exportFolder = "";
	m_exportPrefix = "Result_";
	m_particleExportPrefix = "Particle_";
	m_exportSkipRate = 1;
}

PostSolutionInfo::~PostSolutionInfo()
{
	if (m_fileId != 0){
		cg_close(m_fileId);
	}
}

void PostSolutionInfo::setIterationType(SolverDefinition::IterationType type)
{
	m_iterationType = type;
	if (m_iterationSteps != nullptr){
		delete m_iterationSteps;
		m_iterationSteps = nullptr;
	}
	if (m_timeSteps != nullptr){
		delete m_timeSteps;
		m_timeSteps = nullptr;
	}

	switch (m_iterationType)
	{
	case SolverDefinition::ConvergenceIteration:
		m_iterationSteps = new PostIterationSteps(this);
		connect(m_iterationSteps, SIGNAL(stepsUpdated(QList<int>)), this, SLOT(handleIterationStepsUpdate(QList<int>)));
		connect(m_iterationSteps, SIGNAL(stepsUpdated(int)), this, SIGNAL(cgnsStepsUpdated(int)));
		break;
	case SolverDefinition::TimeIteration:
		m_timeSteps = new PostTimeSteps(this);
		connect(m_timeSteps, SIGNAL(stepsUpdated(QList<double>)), this, SLOT(handleTimeStepsUpdate(QList<double>)));
		connect(m_timeSteps, SIGNAL(stepsUpdated(int)), this, SIGNAL(cgnsStepsUpdated(int)));
		break;
	case SolverDefinition::NoIteration:
	default:
		break;
	}
}

bool PostSolutionInfo::setCurrentStep(unsigned int step, int fn){
	static bool dialogShowing = false;
	m_currentStep = step;
	QTime time, wholetime;
	wholetime.start();

	int tmpfn;
	if (fn == 0){
		QString fname = currentCgnsFileName();
		if (m_fileId != 0){
			tmpfn = m_fileId;
		}else{
			time.start();
			int ier = 1;
			int loop = 0;
			int loopmax = 10;
			// loop until cg_open() succeed.
			while (ier != 0 && loop < loopmax){
				ier = cg_open(iRIC::toStr(fname).c_str(), CG_MODE_READ, &tmpfn);
				if (ier != 0){
					// error.
					cg_close(tmpfn);
					++ loop;
				}
			}
			qDebug("cg_open(): %d", time.elapsed());
			if (ier == 0){
				m_fileId = tmpfn;
			}else{
				tmpfn = 0;
			}
		}
	}else{
		tmpfn = fn;
	}
	if (tmpfn == 0){
		// opening failed.
		return false;
	}
	time.start();
	setupZoneDataContainers(tmpfn);
	checkBaseIterativeDataExist(tmpfn);
	qDebug("setupZoneDataContainer(): %d", time.elapsed());
	bool errorOccured = false;
	for (auto it = m_zoneContainers1D.begin(); it != m_zoneContainers1D.end(); ++it){
		errorOccured = errorOccured || (! (*it)->handleCurrentStepUpdate(tmpfn));
	}
	for (auto it = m_zoneContainers2D.begin(); it != m_zoneContainers2D.end(); ++it){
		time.start();
		errorOccured = errorOccured || (! (*it)->handleCurrentStepUpdate(tmpfn));
		qDebug("handleCurrentStepUpdate() for 2D: %d", time.elapsed());
	}
	for (auto it = m_zoneContainers3D.begin(); it != m_zoneContainers3D.end(); ++it){
		errorOccured = errorOccured || (! (*it)->handleCurrentStepUpdate(tmpfn));
		qDebug("handleCurrentStepUpdate() for 3D: %d", time.elapsed());
	}
	for (auto it2 = m_otherContainers.begin(); it2 != m_otherContainers.end(); ++it2){
		time.start();
		errorOccured = errorOccured || (! (*it2)->handleCurrentStepUpdate(tmpfn));
		qDebug("handleCurrentStepUpdate() for others: %d", time.elapsed());
	}
	qDebug("Loading result from CGNS file: %d", wholetime.elapsed());
	// inform that the current step is updated.
	emit currentStepUpdated();
	// inform that all post processors finished re-rendering.
	emit allPostProcessorsUpdated();

	if (errorOccured && (! dialogShowing)){
		dialogShowing = true;
		QMessageBox::critical(projectData()->mainWindow(), tr("Error"), tr("Error occured while loading calculation result."));
		dialogShowing = false;
	}
	return true;
}

void PostSolutionInfo::informStepsUpdated(){
	QString fname = currentCgnsFileName();
	int fn, ier;
	if (m_fileId != 0){
		cg_close(m_fileId);
		m_fileId = 0;
	}
	ier = cg_open(fname.toUtf8(), CG_MODE_READ, &fn);
	if (ier != 0){
		// failed.
//		const char* msg = cg_get_error();
		cg_close(fn);
		return;
	}
	m_fileId = fn;
	setupZoneDataContainers(m_fileId);
	checkBaseIterativeDataExist(m_fileId);
	emit updated();
	emit allPostProcessorsUpdated();
}

bool PostSolutionInfo::innerSetupZoneDataContainers(int fn, int dim, QStringList& zonenames, QList<PostZoneDataContainer*>& containers, QMap<QString, PostZoneDataContainer*>& containerNameMap)
{
	int ier, nbases;
	ier = cg_nbases(fn, &nbases);
	if (ier != 0){return false;}
	int baseid = 0;
	QString baseName;
	for (int B = 1; B <= nbases; ++B){
		char bname[32];
		int cell_dim;
		int phys_dim;
		ier = cg_base_read(fn, B, bname, &cell_dim, &phys_dim);
		if (ier != 0){return false;}
		if (cell_dim == dim){
			// target base found!
			baseid = B;
			baseName = bname;
		}
	}
	if (baseid == 0){
		// no base for dimension dim.
		if (zonenames.count() == 0){return false;}
		zonenames.clear();
		for (auto it = containers.begin(); it != containers.end(); ++it){
			delete *it;
		}
		containers.clear();
		containerNameMap.clear();
		return true;
	}
	int nzones;
	ier = cg_nzones(fn, baseid, &nzones);
	if (ier != 0){return false;}
	QStringList tmpzonenames;
	for (int Z = 1; Z <= nzones; ++Z){
		cgsize_t sizes[9];
		char zoneName[32];
		ier = cg_zone_read(fn, baseid, Z, zoneName, sizes);
		// check whether this zone has ZoneIterativeData that has some arrays.
		ier = cg_goto(fn, baseid, zoneName, 0, "ZoneIterativeData", 0, "end");
		if (ier != 0){return false;}
		int narrays;
		ier = cg_narrays(&narrays);
		if (ier != 0){return false;}
		if (narrays == 0){continue;}
		tmpzonenames.append(QString(zoneName));
	}
	if (zonenames == tmpzonenames){
		// zone names are equal to those already read.
		for (int i = 0; i < containers.count(); ++i){
			containers[i]->loadIfEmpty(fn);
		}
		return false;
	}
	zonenames = tmpzonenames;
	// clear the current zone containers first.
	for (auto it = containers.begin(); it != containers.end(); ++it){
		delete *it;
	}
	containers.clear();
	containerNameMap.clear();
	QList<SolverDefinitionGridType*> gtypes = projectData()->solverDefinition()->gridTypes();
	for (auto slit = zonenames.begin(); slit != zonenames.end(); ++slit){
		QString zoneName = *slit;
		bool found = false;
		if (zoneName == "iRICZone"){
			for (auto gtit = gtypes.begin(); gtit != gtypes.end(); ++gtit){
				if ((*gtit)->isPrimary() && ! (*gtit)->isOptional()){
					PostZoneDataContainer* cont = new PostZoneDataContainer(baseName, zoneName, *gtit, this);
					cont->loadFromCgnsFile(fn);
					containers.append(cont);
					containerNameMap.insert(zoneName, cont);
					found = true;
				}
			}
		} else {
			for (auto gtit = gtypes.begin(); gtit != gtypes.end(); ++gtit){
				if (zoneName.contains((*gtit)->name())){
					PostZoneDataContainer* cont = new PostZoneDataContainer(baseName, zoneName, *gtit, this);
					cont->loadFromCgnsFile(fn);
					containers.append(cont);
					containerNameMap.insert(zoneName, cont);
					found = true;
				}
			}
		}
		if (! found){
			// no appropriate gridtype found. use the dummy grid type.
			PostZoneDataContainer* cont = new PostZoneDataContainer(baseName, zoneName, projectData()->solverDefinition()->dummyGridType(), this);
			cont->loadFromCgnsFile(fn);
			containers.append(cont);
			containerNameMap.insert(zoneName, cont);
		}
	}
	return true;
}

bool PostSolutionInfo::innerSetupDummy3DZoneDataContainers(int fn, QStringList& zonenames, QList<PostZoneDataContainer*>& containers, QMap<QString, PostZoneDataContainer*>& containerNameMap)
{
	int ier;
	int nbases;
	ier = cg_nbases(fn, &nbases);
	if (ier != 0){return false;}
	int baseid = 0;
	QString baseName;
	for (int B = 1; B <= nbases; ++B){
		char bname[32];
		int cell_dim;
		int phys_dim;
		ier = cg_base_read(fn, B, bname, &cell_dim, &phys_dim);
		if (ier != 0){return false;}
		if (cell_dim == 2){
			// target base found!
			baseid = B;
			baseName = bname;
		}
	}
	if (baseid == 0){
		// no base for dimension dim.
		if (zonenames.count() == 0){return false;}
		zonenames.clear();
		for (auto it = containers.begin(); it != containers.end(); ++it){
			delete *it;
		}
		containers.clear();
		return true;
	}
	int nzones;
	ier = cg_nzones(fn, baseid, &nzones);
	if (ier != 0){return false;}
	QStringList tmpzonenames;
	for (int Z = 1; Z <= nzones; ++Z){
		cgsize_t sizes[9];
		char zoneName[32];
		cg_zone_read(fn, baseid, Z, zoneName, sizes);
		tmpzonenames.append(QString(zoneName));
	}
	if (zonenames == tmpzonenames){
		// zone names are equal to those already read.
		return false;
	}
	zonenames = tmpzonenames;
	// clear the current zone containers first.
	for (auto it = containers.begin(); it != containers.end(); ++it){
		delete *it;
	}
	containers.clear();
	containerNameMap.clear();
	QList<SolverDefinitionGridType*> gtypes = projectData()->solverDefinition()->gridTypes();
	for (auto slit = zonenames.begin(); slit != zonenames.end(); ++slit){
		QString zoneName = *slit;
		if (zoneName == "iRICZone"){
			for (auto gtit = gtypes.begin(); gtit != gtypes.end(); ++gtit){
				if ((*gtit)->isPrimary() && ! (*gtit)->isOptional()){
					PostZoneDataContainer* cont = new PostDummy3DZoneDataContainer(baseName, zoneName, *gtit, this);
					containers.append(cont);
					containerNameMap.insert(zoneName, cont);
				}
			}
		}else{
			for (auto gtit = gtypes.begin(); gtit != gtypes.end(); ++gtit){
				if (zoneName.contains((*gtit)->name())){
					PostZoneDataContainer* cont = new PostDummy3DZoneDataContainer(baseName, zoneName, *gtit, this);
					containers.append(cont);
					containerNameMap.insert(zoneName, cont);
				}
			}
		}
	}
	return true;
}

void PostSolutionInfo::setupZoneDataContainers(int fn)
{
	bool ret;
	// setup 1D containers.
	ret = innerSetupZoneDataContainers(fn, 1, m_zoneNames1D, m_zoneContainers1D, m_zoneContainerNameMap1D);
	if (ret){emit zoneList1DUpdated();}
	// setup 2D containers;
	ret = innerSetupZoneDataContainers(fn, 2, m_zoneNames2D, m_zoneContainers2D, m_zoneContainerNameMap2D);
	if (ret){emit zoneList2DUpdated();}
	// setup 3D containers;
	ret = innerSetupZoneDataContainers(fn, 3, m_zoneNames3D, m_zoneContainers3D, m_zoneContainerNameMap3D);
	// only for 3D demonstration.
//	ret = innerSetupDummy3DZoneDataContainers(fn, m_zoneNames3D, m_zoneContainers3D, m_zoneContainerNameMap3D);
	if (ret){emit zoneList3DUpdated();}
}

void PostSolutionInfo::checkBaseIterativeDataExist(int fn)
{
	m_baseIterativeDataExists = false;
	int nbases, ier;
	ier = cg_nbases(fn, &nbases);
	if (ier != 0){
		return;
	}

	for (int baseid = 1; baseid <= nbases; ++baseid){
		int celldim, physdim;
		char basename[32];
		char bitername[32];
		int nsteps;
		cg_base_read(fn, baseid, basename, &celldim, &physdim);

		// setup baseIterative.
		ier = cg_biter_read(fn, baseid, bitername, &nsteps);
		if (ier != 0){return;}

		cg_goto(fn, baseid, bitername, 0, "end");
		int narrays;
		cg_narrays(&narrays);
		for (int i = 1; i <= narrays; ++i){
			char arrayname[32];
			DataType_t datatype;
			int datadim;
			cgsize_t dimVec[3];
			cg_array_info(i, arrayname, &datatype, &datadim, dimVec);
			QString aName(arrayname);
			if (aName != "TimeValues" && aName != "IterationValues"){
				m_baseIterativeDataExists = true;
				return;
			}
		}
	}
}

bool PostSolutionInfo::hasResults()
{
	if (m_timeSteps != 0){
		return m_timeSteps->dataExists();
	}else if (m_iterationSteps != nullptr){
		return m_iterationSteps->dataExists();
	}else{
		return false;
	}
}

void PostSolutionInfo::checkCgnsStepsUpdate()
{
	static bool checking = false;
	if (checking){
		return;
	}
	checking = true;
	close();
	QString fname = currentCgnsFileName();
	int fn, ier;
	ier = cg_open(iRIC::toStr(fname).c_str(), CG_MODE_READ, &fn);
	if (ier != 0){
		// error occured while opening.
		cg_close(fn);
		checking = false;
		QMessageBox::critical(projectData()->mainWindow(), tr("Error"), tr("Error occured while loading calculation result."));
		return;
	}
	m_fileId = fn;
	if (m_timeSteps != nullptr) {
		m_timeSteps->checkStepsUpdate(m_fileId);
	}
	if (m_iterationSteps != nullptr) {
		m_iterationSteps->checkStepsUpdate(m_fileId);
	}
	checking = false;
}

void PostSolutionInfo::handleIterationStepsUpdate(const QList<int>& steps)
{
	QList<QString> strSteps;
	for (auto it = steps.begin(); it != steps.end(); ++it){
		strSteps.push_back(QString("i = %1").arg(*it));
	}
	// inform the containers that the steps are updated.
	informStepsUpdated();
	// inform to the animation controller that the steps are updated.
	emit cgnsStepsUpdated(strSteps);
}

void PostSolutionInfo::handleTimeStepsUpdate(const QList<double>& steps)
{
	QList<QString> strSteps;
	for (auto it = steps.begin(); it != steps.end(); ++it){
		strSteps.push_back(QString("t = %1 s").arg(iRIC::timeSecondValueStr(*it)));
	}
	// inform the containers that the steps are updated.
	informStepsUpdated();
	// inform to the animation controller that the steps are updated.
	emit cgnsStepsUpdated(strSteps);
}

void PostSolutionInfo::informCgnsSteps()
{
	if (m_timeSteps != nullptr) {
		m_timeSteps->informSteps();
	}
	if (m_iterationSteps != nullptr) {
		m_iterationSteps->informSteps();
	}
}

void PostSolutionInfo::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	m_currentStep = elem.attribute("currentStep").toInt();
}

void PostSolutionInfo::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	QString cstep;
	cstep.setNum(m_currentStep);
	writer.writeAttribute("currentStep", cstep);
}

void PostSolutionInfo::loadFromCgnsFile(const int fn)
{
	m_currentStep = 0;
	if (m_timeSteps != nullptr){
		m_timeSteps->blockSignals(true);
		m_timeSteps->loadFromCgnsFile(fn);
		m_timeSteps->blockSignals(false);
	}
	if (m_iterationSteps != nullptr){
		m_iterationSteps->blockSignals(true);
		m_iterationSteps->loadFromCgnsFile(fn);
		m_iterationSteps->blockSignals(false);
	}
	setCurrentStep(currentStep(), fn);
}

void PostSolutionInfo::closeCgnsFile()
{
	// clear the current zone containers first.
	for (auto it = m_zoneContainers1D.begin(); it != m_zoneContainers1D.end(); ++it){
		delete *it;
	}
	for (auto it = m_zoneContainers2D.begin(); it != m_zoneContainers2D.end(); ++it){
		delete *it;
	}
	for (auto it = m_zoneContainers3D.begin(); it != m_zoneContainers3D.end(); ++it){
		delete *it;
	}
	m_zoneContainers1D.clear();
	m_zoneContainers2D.clear();
	m_zoneContainers3D.clear();
	m_zoneContainerNameMap1D.clear();
	m_zoneContainerNameMap2D.clear();
	m_zoneContainerNameMap3D.clear();
	m_zoneNames1D.clear();
	m_zoneNames2D.clear();
	m_zoneNames3D.clear();
	emit zoneList1DUpdated();
	emit zoneList2DUpdated();
	emit zoneList3DUpdated();
}

void PostSolutionInfo::informSolverStart()
{
//	m_timerId = startTimer(TIMERINTERVAL);
	informStepsUpdated();
}

void PostSolutionInfo::informSolverFinish()
{
//	killTimer(m_timerId);
}

void PostSolutionInfo::timerEvent(QTimerEvent * /*e*/)
{
/*
	if (e->timerId() == m_timerId){
		checkCgnsStepsUpdate();
	}
*/
}

bool PostSolutionInfo::isDataAvailable()
{
	return (isDataAvailableBase() || isDataAvailable1D() || isDataAvailable2D() || isDataAvailable3D());
}

bool PostSolutionInfo::isDataAvailableBase()
{
	return stepsExist() && m_baseIterativeDataExists;
}

bool PostSolutionInfo::isDataAvailable1D()
{
	return stepsExist() && (m_zoneContainers1D.count() > 0);
}

bool PostSolutionInfo::isDataAvailable2D()
{
	return stepsExist() && (m_zoneContainers2D.count() > 0);
}

bool PostSolutionInfo::isDataAvailable3D()
{
	return stepsExist() && (m_zoneContainers3D.count() > 0);
}

bool PostSolutionInfo::stepsExist()
{
	bool ret = false;
	if (m_timeSteps != nullptr) {
		ret = m_timeSteps->dataExists();
	}
	if (m_iterationSteps != nullptr) {
		ret = m_iterationSteps->dataExists();
	}
	return ret;
}

double PostSolutionInfo::currentTimeStep()
{
	if (m_timeSteps == nullptr){return 0;}
	if (m_currentStep >= m_timeSteps->timesteps().count()){return 0;}
	return m_timeSteps->timesteps().at(m_currentStep);
}

const QList<PostZoneDataContainer*>& PostSolutionInfo::zoneContainers(Dimension dim)
{
	if (dim == dim1D){return zoneContainers1D();}
	else if (dim == dim2D){return zoneContainers2D();}
	else {return zoneContainers3D();}
}

PostZoneDataContainer* PostSolutionInfo::zoneContainer(Dimension dim, const QString& zoneName)
{
	if (dim == dim1D){return zoneContainer1D(zoneName);}
	else if (dim == dim2D){return zoneContainer2D(zoneName);}
	else {return zoneContainer3D(zoneName);}
}

int PostSolutionInfo::toIntDimension(Dimension dim)
{
	switch (dim){
	case PostSolutionInfo::dim1D:
		return 1;
		break;
	case PostSolutionInfo::dim2D:
		return 2;
		break;
	case PostSolutionInfo::dim3D:
		return 3;
		break;
	default:
		return 3;
	}
}

PostSolutionInfo::Dimension PostSolutionInfo::fromIntDimension(int dim)
{
	switch (dim){
	case 1:
		return PostSolutionInfo::dim1D;
		break;
	case 2:
		return PostSolutionInfo::dim2D;
		break;
	case 3:
		return PostSolutionInfo::dim3D;
		break;
	}
	return PostSolutionInfo::dim3D;
}

void PostSolutionInfo::close()
{
	if (m_fileId != 0){
		cg_close(m_fileId);
		m_fileId = 0;
	}
}

void PostSolutionInfo::exportCalculationResult()
{
	// check whether it has result.
//	if (! hasResults()){return;}

	// If it has multiple dimension results, ask user to select one of them.
	int availableDims = 0;
	int availabieDimCount = 0;
	if (isDataAvailable1D()){
		availableDims = availableDims + PostBaseSelectingDialog::Dimension1;
		++ availabieDimCount;
	}
	if (isDataAvailable2D()){
		availableDims = availableDims + PostBaseSelectingDialog::Dimension2;
		++ availabieDimCount;
	}
	if (isDataAvailable3D()){
		availableDims = availableDims + PostBaseSelectingDialog::Dimension3;
		++ availabieDimCount;
	}
	Dimension dim;
	switch (availabieDimCount){
	case 0:
		QMessageBox::warning(iricMainWindow(), tr("Warning"), tr("No calculation result exists."), QMessageBox::Ok);
		return;
		break;
	case 1:
		dim = PostBaseSelectingDialog::getPostDimension(static_cast<PostBaseSelectingDialog::BaseDimension>(availableDims));
		break;
	default:
		PostBaseSelectingDialog dialog(iricMainWindow());
		dialog.setAvailableDimensions(availableDims);
		int ret = dialog.exec();
		if (ret == QDialog::Rejected){
			// cancel clicked.
			return;
		}
		dim = dialog.selectedPostDimension();
	}
	// select zone.
	QList<PostZoneDataContainer*> containers = zoneContainers(dim);
	QList<PostZoneDataContainer*> tmpContainers;
	for (int i = 0; i < containers.count(); ++i){
		PostZoneDataContainer* cont = containers[i];
		SolverDefinitionGridType::GridType gt = cont->gridType()->defaultGridType();
		switch (gt){
		case SolverDefinitionGridType::gtUnknownGrid:
			// NG
			break;
		default:
			// OK
			tmpContainers.append(cont);
		}
	}
	QString zoneName;
	if (tmpContainers.count() == 0){
		// No valid grid.
		QMessageBox::warning(iricMainWindow(), tr("Error"), tr("Calculation result does not contain grid data."));
		return;
	} else if (tmpContainers.count() == 1){
		zoneName = containers.at(0)->zoneName();
	} if (tmpContainers.count() > 1){
		ItemSelectingDialog dialog;
		QList<QString> zonelist;
		for (auto it = tmpContainers.begin(); it != tmpContainers.end(); ++it){
			zonelist.append((*it)->zoneName());
		}
		dialog.setItems(zonelist);
		int ret = dialog.exec();
		if (ret == QDialog::Rejected){
			return;
		}
		zoneName = zonelist.at(dialog.selectIndex());
	}
	PostZoneDataContainer* zoneC = zoneContainer(dim, zoneName);
	// show setting dialog
	PostDataExportDialog expDialog(iricMainWindow());

	if (m_exportFormat == efVTK){
		expDialog.setFormat(PostDataExportDialog::fmVTK);
	} else if (m_exportFormat == efCSV){
		expDialog.setFormat(PostDataExportDialog::fmCSV);
	}

	expDialog.setTimeValues(m_timeSteps->timesteps());
	expDialog.setAllTimeSteps(m_exportAllSteps);
	expDialog.setStartTimeStep(m_exportStartStep);
	expDialog.setEndTimeStep(m_exportEndStep);
	vtkStructuredGrid* sGrid = vtkStructuredGrid::SafeDownCast(zoneC->data());
	if (sGrid != nullptr){
		// structured grid
		int dim[3];
		sGrid->GetDimensions(dim);
		expDialog.setIJKRange(dim[0], dim[1], dim[2]);
		expDialog.setFullRange(m_exportFullRange);
		expDialog.setIMin(m_exportIMin);
		expDialog.setIMax(m_exportIMax);
		expDialog.setJMin(m_exportJMin);
		expDialog.setJMax(m_exportJMax);
		expDialog.setKMin(m_exportKMin);
		expDialog.setKMax(m_exportKMax);
	} else {
		// unstructured grid
		expDialog.hideDataRange();
	}
	if (m_exportFolder == ""){
		m_exportFolder = LastIODirectory::get();
	}
	expDialog.setOutputFolder(m_exportFolder);
	expDialog.setPrefix(m_exportPrefix);
	expDialog.setSkipRate(m_exportSkipRate);

	if (expDialog.exec() != QDialog::Accepted){return;}
	if (expDialog.format() == PostDataExportDialog::fmVTK){
		m_exportFormat = efVTK;
	} else if (expDialog.format() == PostDataExportDialog::fmCSV){
		m_exportFormat = efCSV;
	}
	m_exportAllSteps = expDialog.allTimeSteps();
	m_exportStartStep = expDialog.startTimeStep();
	m_exportEndStep = expDialog.endTimeStep();
	if (zoneC->gridType()->defaultGridType() == SolverDefinitionGridType::gtStructured2DGrid){
		m_exportFullRange = expDialog.fullRange();
		m_exportIMin = expDialog.iMin();
		m_exportIMax = expDialog.iMax();
		m_exportJMin = expDialog.jMin();
		m_exportJMax = expDialog.jMax();
		m_exportKMin = expDialog.kMin();
		m_exportKMax = expDialog.kMax();
	}
	m_exportFolder = expDialog.outputFolder();
	m_exportPrefix = expDialog.prefix();
	m_exportSkipRate = expDialog.skipRate();

	// start exporting.
	QProgressDialog dialog(iricMainWindow());
	dialog.setRange(m_exportStartStep, m_exportEndStep);
	dialog.setWindowTitle(tr("Export Calculation Result"));
	if (m_exportFormat == efVTK){
		dialog.setLabelText(tr("Saving calculation result as VTK files..."));
	} else {
		dialog.setLabelText(tr("Saving calculation result as CSV files..."));
	}
	dialog.setFixedSize(300, 100);
	dialog.setModal(true);
	dialog.show();

	iricMainWindow()->setContinuousSnapshotInProgress(true);
	int stepBackup = currentStep();

	int step = m_exportStartStep;
	int fileIndex = 1;
	QDir outputFolder(m_exportFolder);
	while (step <= m_exportEndStep){
		dialog.setValue(step);
		qApp->processEvents();
		if (dialog.wasCanceled()){
			setCurrentStep(stepBackup);
			iricMainWindow()->setContinuousSnapshotInProgress(false);
			return;
		}
		setCurrentStep(step);
		QString fileName = m_exportPrefix;
		bool ok;
		double time = currentTimeStep();
		if (m_exportFormat == efVTK){
			fileName.append(QString("%1.vtk").arg(fileIndex));
			ok = zoneC->saveToVTKFile(outputFolder.absoluteFilePath(fileName), time, m_exportIMin, m_exportIMax, m_exportJMin, m_exportJMax, m_exportKMin, m_exportKMax);
		} else {
			fileName.append(QString("%1.csv").arg(fileIndex));
			ok = zoneC->saveToCSVFile(outputFolder.absoluteFilePath(fileName), time, m_exportIMin, m_exportIMax, m_exportJMin, m_exportJMax, m_exportKMin, m_exportKMax);
		}
		if (! ok){
			setCurrentStep(stepBackup);
			QMessageBox::critical(iricMainWindow(), tr("Error"), tr("Error occured while saving %1").arg(fileName));
			iricMainWindow()->setContinuousSnapshotInProgress(false);
			return;
		}
		step += m_exportSkipRate;
		++ fileIndex;
	}
	iricMainWindow()->setContinuousSnapshotInProgress(false);
	setCurrentStep(stepBackup);
}
