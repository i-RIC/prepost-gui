#include "../base/iricmainwindowinterface.h"
#include "../misc/cgnsfileopener.h"
#include "../project/projectdata.h"
#include "../solverdef/solverdefinition.h"
#include "../solverdef/solverdefinitiongridtype.h"
#include "../solverdef/solverdefinitiongridtype.h"
#include "exporter/postzonedatacsvexporter.h"
#include "exporter/postzonedatashapeexporter.h"
#include "exporter/postzonedatatpoexporter.h"
#include "exporter/postzonedatavtkexporter.h"
#include "postbaseselectingdialog.h"
#include "postcontainer/postcalculatedresult.h"
#include "postdataexportdialog.h"
//#include "postdummy3dzonedatacontainer.h"
#include "postiterationsteps.h"
#include "postsolutioninfo.h"
#include "posttimesteps.h"
#include "postzonedatacontainer.h"

#include <guibase/widget/itemselectingdialog.h>
#include <misc/lastiodirectory.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QCoreApplication>
#include <QDomElement>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QProgressDialog>
#include <QThread>
#include <QTime>
#include <QTimerEvent>
#include <QVector2D>
#include <QXmlStreamWriter>

#include <vtkStructuredGrid.h>
#include <memory>
#include <string>
#include <vector>

#include <cgnslib.h>
#include <iriclib.h>

namespace {

void writeZonesToProjectMainFile(int dim, const QList<PostZoneDataContainer*>& zones, QXmlStreamWriter& writer)
{
	if (zones.size() == 0) {return;}

	writer.writeStartElement("Base");
	writer.writeAttribute("dimension", QString::number(dim));
	for (auto c : zones) {
		writer.writeStartElement("Zone");
		c->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();
}

} // namespace

PostSolutionInfo::PostSolutionInfo(ProjectDataItem* parent) :
	ProjectDataItem {parent},
	m_iterationType {SolverDefinition::NoIteration},
	m_iterationSteps {nullptr},
	m_timeSteps {nullptr},
	m_currentStep {0},
	m_timerId {0},
	m_opener {nullptr},
	m_exportFormat {PostDataExportDialog::Format::VTKASCII},
	m_disableCalculatedResult {false},
	m_particleExportPrefix {"Particle_"},
	m_loadedElement {nullptr}
{}

PostSolutionInfo::~PostSolutionInfo()
{
	close();
	clearCalculatedResults(&m_calculatedResults1D);
	clearCalculatedResults(&m_calculatedResults2D);
	clearCalculatedResults(&m_calculatedResults3D);
	delete m_loadedElement;
}

SolverDefinition::IterationType PostSolutionInfo::iterationType() const
{
	return m_iterationType;
}

void PostSolutionInfo::setIterationType(SolverDefinition::IterationType type)
{
	m_iterationType = type;

	delete m_iterationSteps;
	m_iterationSteps = nullptr;

	delete m_timeSteps;
	m_timeSteps = nullptr;

	switch (m_iterationType) {
	case SolverDefinition::ConvergenceIteration:
		m_iterationSteps = new PostIterationSteps(this);
		connect(m_iterationSteps, SIGNAL(stepsUpdated(QList<int>)), this, SIGNAL(cgnsIterationStepsUpdated(QList<int>)));
		connect(m_iterationSteps, SIGNAL(stepsUpdated(QList<int>)), this, SLOT(informStepsUpdated()));
		connect(m_iterationSteps, SIGNAL(stepsUpdated(int)), this, SIGNAL(cgnsStepsUpdated(int)));
		break;
	case SolverDefinition::TimeIteration:
		m_timeSteps = new PostTimeSteps(this);
		connect(m_timeSteps, SIGNAL(stepsUpdated(QList<double>)), this, SIGNAL(cgnsTimeStepsUpdated(QList<double>)));
		connect(m_timeSteps, SIGNAL(stepsUpdated(QList<double>)), this, SLOT(informStepsUpdated()));
		connect(m_timeSteps, SIGNAL(stepsUpdated(int)), this, SIGNAL(cgnsStepsUpdated(int)));
		break;
	case SolverDefinition::NoIteration:
	default:
		break;
	}
}

PostIterationSteps* PostSolutionInfo::iterationSteps() const
{
	return m_iterationSteps;
}

PostTimeSteps* PostSolutionInfo::timeSteps() const
{
	return m_timeSteps;
}

int PostSolutionInfo::currentStep() const
{
	return m_currentStep;
}

bool PostSolutionInfo::setCurrentStep(unsigned int step, int fn)
{
	static bool dialogShowing = false;
	m_currentStep = step;
	QTime time, wholetime;
	wholetime.start();

	int tmpfn = 0;
	if (fn == 0) {
		bool ok = open();
		if (ok) {
			tmpfn = m_opener->fileId();
		}
	} else {
		tmpfn = fn;
	}
	if (tmpfn == 0) {
		// opening failed.
		return false;
	}
	time.start();
	setupZoneDataContainers(tmpfn);
	checkBaseIterativeDataExist(tmpfn);
	qDebug("setupZoneDataContainer(): %d", time.elapsed());

	loadCalculatedResult();

	bool errorOccured = false;
	for (auto it = m_zoneContainers1D.begin(); it != m_zoneContainers1D.end(); ++it) {
		errorOccured = errorOccured || (!(*it)->handleCurrentStepUpdate(tmpfn, m_disableCalculatedResult));
	}
	for (auto it = m_zoneContainers2D.begin(); it != m_zoneContainers2D.end(); ++it) {
		time.start();
		errorOccured = errorOccured || (!(*it)->handleCurrentStepUpdate(tmpfn, m_disableCalculatedResult));
		qDebug("handleCurrentStepUpdate() for 2D: %d", time.elapsed());
	}
	for (auto it = m_zoneContainers3D.begin(); it != m_zoneContainers3D.end(); ++it) {
		errorOccured = errorOccured || (!(*it)->handleCurrentStepUpdate(tmpfn, m_disableCalculatedResult));
		qDebug("handleCurrentStepUpdate() for 3D: %d", time.elapsed());
	}
	for (auto it2 = m_otherContainers.begin(); it2 != m_otherContainers.end(); ++it2) {
		time.start();
		errorOccured = errorOccured || (!(*it2)->handleCurrentStepUpdate(tmpfn));
		qDebug("handleCurrentStepUpdate() for others: %d", time.elapsed());
	}
	qDebug("Loading result from CGNS file: %d", wholetime.elapsed());
	// inform that the current step is updated.
	emit currentStepUpdated();
	// inform that all post processors finished re-rendering.
	emit allPostProcessorsUpdated();

	if (errorOccured && (! dialogShowing)) {
		dialogShowing = true;
		QMessageBox::critical(projectData()->mainWindow(), tr("Error"), tr("Error occured while loading calculation result."));
		dialogShowing = false;
	}
	return true;
}

void PostSolutionInfo::informStepsUpdated()
{
	bool ok = open();
	if (!ok) {return;}
	setupZoneDataContainers(m_opener->fileId());
	checkBaseIterativeDataExist(m_opener->fileId());
	emit updated();
	emit allPostProcessorsUpdated();
}

bool PostSolutionInfo::innerSetupZoneDataContainers(int fn, int dim, std::vector<std::string>* zoneNames, QList<PostZoneDataContainer*>* containers, QMap<std::string, PostZoneDataContainer*>* containerNameMap, QMap<std::string, std::vector<PostCalculatedResult*> > *results)
{
	int ier, nbases;
	ier = cg_nbases(fn, &nbases);
	if (ier != 0) {return false;}
	int baseid = 0;
	std::string baseName;
	for (int B = 1; B <= nbases; ++B) {
		char bname[32];
		int cell_dim;
		int phys_dim;
		ier = cg_base_read(fn, B, bname, &cell_dim, &phys_dim);
		if (ier != 0) {return false;}
		if (cell_dim == dim) {
			// target base found!
			baseid = B;
			baseName = bname;
		}
	}
	if (baseid == 0) {
		// no base for dimension dim.
		if (zoneNames->size() == 0) {return false;}
		zoneNames->clear();
		clearContainers(containers);
		containerNameMap->clear();
		return true;
	}
	int nzones;
	ier = cg_nzones(fn, baseid, &nzones);
	if (ier != 0) {return false;}
	std::vector<std::string> tmpZoneNames;
	for (int Z = 1; Z <= nzones; ++Z) {
		cgsize_t sizes[9];
		char zoneName[32];
		ier = cg_zone_read(fn, baseid, Z, zoneName, sizes);
		// check whether this zone has ZoneIterativeData that has some arrays.
		ier = cg_goto(fn, baseid, zoneName, 0, "ZoneIterativeData", 0, "end");
		if (ier != 0) {return false;}
		int narrays;
		ier = cg_narrays(&narrays);
		if (ier != 0) {return false;}
		if (narrays == 0) {continue;}
		tmpZoneNames.push_back(std::string(zoneName));
	}
	if (*zoneNames == tmpZoneNames) {
		// zone names are equal to those already read.
		for (int i = 0; i < containers->count(); ++i) {
			(*containers)[i]->loadIfEmpty(fn);
		}
		return false;
	}
	*zoneNames = tmpZoneNames;

	// clear the current zone containers first.
	for (auto c : *containers) {
		results->insert(c->zoneName(), c->detachCalculatedResult());
	}
	clearContainers(containers);
	containerNameMap->clear();
	QList<SolverDefinitionGridType*> gtypes = projectData()->solverDefinition()->gridTypes();
	for (std::string zoneName : *zoneNames) {
		bool found = false;
		if (zoneName == "iRICZone") {
			for (auto gtit = gtypes.begin(); gtit != gtypes.end(); ++gtit) {
				if ((*gtit)->isPrimary() && !(*gtit)->isOptional()) {
					PostZoneDataContainer* cont = new PostZoneDataContainer(baseName, zoneName, *gtit, this);
					cont->loadFromCgnsFile(fn);
					containers->push_back(cont);
					containerNameMap->insert(zoneName, cont);
					found = true;
					break;
				}
			}
		} else {
			for (auto gtit = gtypes.begin(); gtit != gtypes.end(); ++gtit) {
				if (zoneName.find((*gtit)->name()) != std::string::npos) {
					PostZoneDataContainer* cont = new PostZoneDataContainer(baseName, zoneName, *gtit, this);
					cont->loadFromCgnsFile(fn);
					containers->append(cont);
					containerNameMap->insert(zoneName, cont);
					found = true;
					break;
				}
			}
		}
		if (! found) {
			// no appropriate gridtype found. use the dummy grid type.
			PostZoneDataContainer* cont = new PostZoneDataContainer(baseName, zoneName, projectData()->solverDefinition()->dummyGridType(), this);
			cont->loadFromCgnsFile(fn);
			containers->append(cont);
			containerNameMap->insert(zoneName, cont);
		}
	}
	std::vector<std::string> namesToRemove;
	for (auto it = results->begin(); it != results->end(); ++it) {
		auto c = containerNameMap->value(it.key(), nullptr);
		if (c != nullptr) {
			c->attachCalculatedResult(it.value());
			namesToRemove.push_back(it.key());
		}
	}
	for (auto name : namesToRemove) {
		results->remove(name);
	}
	return true;
}

/*
bool PostSolutionInfo::innerSetupDummy3DZoneDataContainers(int fn, std::vector<std::string>* zoneNames, QList<PostZoneDataContainer*>* containers, QMap<std::string, PostZoneDataContainer*>* containerNameMap)
{
	int ier;
	int nbases;
	ier = cg_nbases(fn, &nbases);
	if (ier != 0) {return false;}
	int baseid = 0;
	std::string baseName;
	for (int B = 1; B <= nbases; ++B) {
		char bname[32];
		int cell_dim;
		int phys_dim;
		ier = cg_base_read(fn, B, bname, &cell_dim, &phys_dim);
		if (ier != 0) {return false;}
		if (cell_dim == 2) {
			// target base found!
			baseid = B;
			baseName = bname;
		}
	}
	if (baseid == 0) {
		// no base for dimension dim.
		if (zoneNames->size() == 0) {return false;}
		zoneNames->clear();
		clearContainers(containers);
		return true;
	}
	int nzones;
	ier = cg_nzones(fn, baseid, &nzones);
	if (ier != 0) {return false;}
	std::vector<std::string> tmpZoneNames;
	for (int Z = 1; Z <= nzones; ++Z) {
		cgsize_t sizes[9];
		char zoneName[32];
		cg_zone_read(fn, baseid, Z, zoneName, sizes);
		tmpZoneNames.push_back(std::string(zoneName));
	}
	if (*zoneNames == tmpZoneNames) {
		// zone names are equal to those already read.
		return false;
	}
	*zoneNames = tmpZoneNames;
	// clear the current zone containers first.
	clearContainers(containers);
	containerNameMap->clear();
	QList<SolverDefinitionGridType*> gtypes = projectData()->solverDefinition()->gridTypes();
	for (std::string zoneName : *zoneNames) {
		if (zoneName == "iRICZone") {
			for (auto gtit = gtypes.begin(); gtit != gtypes.end(); ++gtit) {
				if ((*gtit)->isPrimary() && !(*gtit)->isOptional()) {
					PostZoneDataContainer* cont = new PostDummy3DZoneDataContainer(baseName, zoneName, *gtit, this);
					containers->append(cont);
					containerNameMap->insert(zoneName, cont);
				}
			}
		} else {
			for (auto gtit = gtypes.begin(); gtit != gtypes.end(); ++gtit) {
				if (zoneName.find((*gtit)->name()) != std::string::npos) {
					PostZoneDataContainer* cont = new PostDummy3DZoneDataContainer(baseName, zoneName, *gtit, this);
					containers->append(cont);
					containerNameMap->insert(zoneName, cont);
				}
			}
		}
	}
	return true;
}
*/

void PostSolutionInfo::setupZoneDataContainers(int fn)
{
	bool ret;
	// setup 1D containers.
	ret = innerSetupZoneDataContainers(fn, 1, &m_zoneNames1D, &m_zoneContainers1D, &m_zoneContainerNameMap1D, &m_calculatedResults1D);
	if (ret) {emit zoneList1DUpdated();}
	// setup 2D containers;
	ret = innerSetupZoneDataContainers(fn, 2, &m_zoneNames2D, &m_zoneContainers2D, &m_zoneContainerNameMap2D, &m_calculatedResults2D);
	if (ret) {emit zoneList2DUpdated();}
	// setup 3D containers;
	ret = innerSetupZoneDataContainers(fn, 3, &m_zoneNames3D, &m_zoneContainers3D, &m_zoneContainerNameMap3D, &m_calculatedResults3D);
	// only for 3D demonstration.
//	ret = innerSetupDummy3DZoneDataContainers(fn, m_zoneNames3D, m_zoneContainers3D, m_zoneContainerNameMap3D);
	if (ret) {emit zoneList3DUpdated();}
}

void PostSolutionInfo::checkBaseIterativeDataExist(int fn)
{
	m_baseIterativeDataExists = false;
	int nbases, ier;
	ier = cg_nbases(fn, &nbases);
	if (ier != 0) {
		return;
	}

	for (int baseid = 1; baseid <= nbases; ++baseid) {
		int celldim, physdim;
		char basename[32];
		char bitername[32];
		int nsteps;
		cg_base_read(fn, baseid, basename, &celldim, &physdim);

		// setup baseIterative.
		ier = cg_biter_read(fn, baseid, bitername, &nsteps);
		if (ier != 0) {return;}

		cg_goto(fn, baseid, bitername, 0, "end");
		int narrays;
		cg_narrays(&narrays);
		for (int i = 1; i <= narrays; ++i) {
			char arrayname[32];
			DataType_t datatype;
			int datadim;
			cgsize_t dimVec[3];
			cg_array_info(i, arrayname, &datatype, &datadim, dimVec);
			QString aName(arrayname);
			if (aName != "TimeValues" && aName != "IterationValues") {
				m_baseIterativeDataExists = true;
				return;
			}
		}
	}
}

void PostSolutionInfo::loadCalculatedResult()
{
	if (m_loadedElement == nullptr) {return;}

	for (int i = 0; i < m_loadedElement->childNodes().size(); ++i) {
		auto baseNode = m_loadedElement->childNodes().at(i);
		int dim = iRIC::getIntAttribute(baseNode, "dimension");
		QMap<std::string, PostZoneDataContainer*>* conts = nullptr;
		if (dim == 1) {conts = &m_zoneContainerNameMap1D;}
		if (dim == 2) {conts = &m_zoneContainerNameMap2D;}
		if (dim == 3) {conts = &m_zoneContainerNameMap3D;}
		if (conts == nullptr) {continue;}

		for (int j = 0; j < baseNode.childNodes().size(); ++j) {
			auto zoneElem = baseNode.childNodes().at(j).toElement();
			auto zoneName = iRIC::toStr(zoneElem.attribute("name"));
			auto z = conts->value(zoneName, nullptr);
			if (z == nullptr) {continue;}
			z->loadFromProjectMainFile(zoneElem);
		}
	}

	delete m_loadedElement;
	m_loadedElement = nullptr;
}

bool PostSolutionInfo::hasResults()
{
	if (m_timeSteps != 0) {
		return m_timeSteps->dataExists();
	} else if (m_iterationSteps != nullptr) {
		return m_iterationSteps->dataExists();
	} else {
		return false;
	}
}

void PostSolutionInfo::checkCgnsStepsUpdate()
{
	static bool checking = false;
	if (checking) {
		return;
	}
	checking = true;
	close();

	QThread::msleep(1000);
	bool ok = open();
	if (! ok) {
		// error occured while opening.
		checking = false;
		QMessageBox::warning(projectData()->mainWindow(), tr("Warning"), tr("Loading calculation result for visualization failed. Please try again later, or wait until end of calculation."));
		return;
	}
	if (m_timeSteps != nullptr) {
		m_timeSteps->checkStepsUpdate(m_opener->fileId());
	}
	if (m_iterationSteps != nullptr) {
		m_iterationSteps->checkStepsUpdate(m_opener->fileId());
	}
	checking = false;
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

	m_loadedElement = new QDomElement();
	*m_loadedElement = elem;
}

void PostSolutionInfo::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	QString cstep;
	cstep.setNum(m_currentStep);
	writer.writeAttribute("currentStep", cstep);

	writeZonesToProjectMainFile(1, zoneContainers1D(), writer);
	writeZonesToProjectMainFile(2, zoneContainers2D(), writer);
	writeZonesToProjectMainFile(3, zoneContainers3D(), writer);
}

void PostSolutionInfo::loadFromCgnsFile(const int fn)
{
	m_currentStep = 0;
	if (m_timeSteps != nullptr) {
		m_timeSteps->blockSignals(true);
		m_timeSteps->loadFromCgnsFile(fn);
		m_timeSteps->blockSignals(false);
	}
	if (m_iterationSteps != nullptr) {
		m_iterationSteps->blockSignals(true);
		m_iterationSteps->loadFromCgnsFile(fn);
		m_iterationSteps->blockSignals(false);
	}
	setCurrentStep(currentStep(), fn);
}

void PostSolutionInfo::closeCgnsFile()
{
	// clear the current zone containers first.
	clearContainers(&m_zoneContainers1D);
	m_zoneContainerNameMap1D.clear();
	clearContainers(&m_zoneContainers2D);
	m_zoneContainerNameMap2D.clear();
	clearContainers(&m_zoneContainers3D);
	m_zoneContainerNameMap3D.clear();
	m_zoneNames1D.clear();
	m_zoneNames2D.clear();
	m_zoneNames3D.clear();
	emit zoneList1DUpdated();
	emit zoneList2DUpdated();
	emit zoneList3DUpdated();
}

const QList<PostZoneDataContainer*>& PostSolutionInfo::zoneContainers1D() const
{
	return m_zoneContainers1D;
}

const QList<PostZoneDataContainer*>& PostSolutionInfo::zoneContainers2D() const
{
	return m_zoneContainers2D;
}

const QList<PostZoneDataContainer*>& PostSolutionInfo::zoneContainers3D() const
{
	return m_zoneContainers3D;
}

PostZoneDataContainer* PostSolutionInfo::zoneContainer1D(const std::string& zoneName) const
{
	return m_zoneContainerNameMap1D.value(zoneName, 0);
}

PostZoneDataContainer* PostSolutionInfo::zoneContainer2D(const std::string& zoneName) const
{
	return m_zoneContainerNameMap2D.value(zoneName, 0);
}

PostZoneDataContainer* PostSolutionInfo::zoneContainer3D(const std::string& zoneName) const
{
	return m_zoneContainerNameMap3D.value(zoneName, 0);
}

void PostSolutionInfo::informSolverStart()
{
//	m_timerId = startTimer(TIMERINTERVAL);
//	informStepsUpdated();
}

void PostSolutionInfo::informSolverFinish()
{
//	killTimer(m_timerId);
}

void PostSolutionInfo::timerEvent(QTimerEvent* /*e*/)
{
	/*
		if (e->timerId() == m_timerId){
			checkCgnsStepsUpdate();
		}
	*/
}

bool PostSolutionInfo::isDataAvailable() const
{
	return (isDataAvailableBase() || isDataAvailable1D() || isDataAvailable2D() || isDataAvailable3D());
}

bool PostSolutionInfo::isDataAvailableBase() const
{
	return stepsExist() && m_baseIterativeDataExists;
}

bool PostSolutionInfo::isDataAvailable1D() const
{
	return stepsExist() && (m_zoneContainers1D.count() > 0);
}

bool PostSolutionInfo::isDataAvailable2D() const
{
	return stepsExist() && (m_zoneContainers2D.count() > 0);
}

bool PostSolutionInfo::isDataAvailable3D() const
{
	return stepsExist() && (m_zoneContainers3D.count() > 0);
}

bool PostSolutionInfo::stepsExist() const
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
	if (m_timeSteps == nullptr) {return 0;}
	if (m_currentStep >= m_timeSteps->timesteps().count()) {return 0;}
	return m_timeSteps->timesteps().at(m_currentStep);
}

const QList<PostZoneDataContainer*>& PostSolutionInfo::zoneContainers(Dimension dim) const
{
	if (dim == dim1D) {return zoneContainers1D();}
	else if (dim == dim2D) {return zoneContainers2D();}
	else {return zoneContainers3D();}
}

PostZoneDataContainer* PostSolutionInfo::zoneContainer(Dimension dim, const std::string& zoneName) const
{
	if (dim == dim1D) {return zoneContainer1D(zoneName);}
	else if (dim == dim2D) {return zoneContainer2D(zoneName);}
	else {return zoneContainer3D(zoneName);}
}

PostZoneDataContainer* PostSolutionInfo::firstZoneContainer() const
{
	auto conts1d = zoneContainers1D();
	if (conts1d.length() > 0) {return conts1d.first();}
	auto conts2d = zoneContainers2D();
	if (conts2d.length() > 0) {return conts2d.first();}
	auto conts3d = zoneContainers3D();
	if (conts3d.length() > 0) {return conts3d.first();}
	return nullptr;
}

int PostSolutionInfo::toIntDimension(Dimension dim)
{
	switch (dim) {
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
	switch (dim) {
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
	delete m_opener;
	m_opener = nullptr;
}

const PostExportSetting& PostSolutionInfo::exportSetting() const
{
	return m_exportSetting;
}

const QString& PostSolutionInfo::particleExportPrefix() const
{
	return m_particleExportPrefix;
}

void PostSolutionInfo::setExportSetting(const PostExportSetting& setting)
{
	m_exportSetting = setting;
}

void PostSolutionInfo::setParticleExportPrefix(const QString& prefix)
{
	m_particleExportPrefix = prefix;
}

int PostSolutionInfo::fileId() const
{
	if (m_opener == nullptr) {return 0;}

	return m_opener->fileId();
}

void PostSolutionInfo::setCalculatedResultDisabled(bool disabled)
{
	m_disableCalculatedResult = disabled;
}

void PostSolutionInfo::exportCalculationResult()
{
	// check whether it has result.
//	if (! hasResults()){return;}

	// If it has multiple dimension results, ask user to select one of them.
	int availableDims = 0;
	int availabieDimCount = 0;
	if (isDataAvailable1D()) {
		availableDims = availableDims + PostBaseSelectingDialog::Dimension1;
		++ availabieDimCount;
	}
	if (isDataAvailable2D()) {
		availableDims = availableDims + PostBaseSelectingDialog::Dimension2;
		++ availabieDimCount;
	}
	if (isDataAvailable3D()) {
		availableDims = availableDims + PostBaseSelectingDialog::Dimension3;
		++ availabieDimCount;
	}
	Dimension dim;
	switch (availabieDimCount) {
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
		if (ret == QDialog::Rejected) {
			// cancel clicked.
			return;
		}
		dim = dialog.selectedPostDimension();
	}
	// select zone.
	QList<PostZoneDataContainer*> containers = zoneContainers(dim);
	std::vector<PostZoneDataContainer*> tmpContainers;
	for (int i = 0; i < containers.count(); ++i) {
		PostZoneDataContainer* cont = containers[i];
		tmpContainers.push_back(cont);
	}
	std::string zoneName;
	if (tmpContainers.size() == 0) {
		// No valid grid.
		QMessageBox::warning(iricMainWindow(), tr("Error"), tr("Calculation result does not contain grid data."));
		return;
	} else if (tmpContainers.size() == 1) {
		zoneName = containers.at(0)->zoneName();
	} if (tmpContainers.size() > 1) {
		ItemSelectingDialog dialog;
		std::vector<QString> zonelist;
		for (auto it = tmpContainers.begin(); it != tmpContainers.end(); ++it) {
			zonelist.push_back((*it)->zoneName().c_str());
		}
		dialog.setItems(zonelist);
		int ret = dialog.exec();
		if (ret == QDialog::Rejected) {
			return;
		}
		zoneName = iRIC::toStr(zonelist.at(dialog.selectedIndex()));
	}
	PostZoneDataContainer* zoneC = zoneContainer(dim, zoneName);
	// show setting dialog
	PostDataExportDialog expDialog(iricMainWindow());

	expDialog.setFormat(m_exportFormat);
	expDialog.setTimeValues(m_timeSteps->timesteps());

	vtkStructuredGrid* sGrid = vtkStructuredGrid::SafeDownCast(zoneC->data());
	if (sGrid != nullptr) {
		// structured grid
		int dim[3];
		sGrid->GetDimensions(dim);
		expDialog.setIJKRange(dim[0], dim[1], dim[2]);
	}
	if (m_exportSetting.folder == "") {
		m_exportSetting.folder = LastIODirectory::get();
	}
	expDialog.setExportSetting(m_exportSetting);

	if (sGrid == nullptr) {
		// unstructured grid
		expDialog.hideDataRange();
	}

	if (expDialog.exec() != QDialog::Accepted) {return;}

	m_exportFormat = expDialog.format();
	m_exportSetting = expDialog.exportSetting();

	// start exporting.
	QProgressDialog dialog(iricMainWindow());
	dialog.setRange(m_exportSetting.startStep , m_exportSetting.endStep);
	dialog.setWindowTitle(tr("Export Calculation Result"));
	switch (m_exportFormat) {
	case PostDataExportDialog::Format::VTKASCII:
	case PostDataExportDialog::Format::VTKBinary:
		dialog.setLabelText(tr("Saving calculation result as VTK files..."));
		break;
	case PostDataExportDialog::Format::CSV:
		dialog.setLabelText(tr("Saving calculation result as CSV files..."));
		break;
	case PostDataExportDialog::Format::ESRIShape:
		dialog.setLabelText(tr("Saving calculation result as ESRI Shape files..."));
		break;
	}
	dialog.setFixedSize(300, 100);
	dialog.setModal(true);
	dialog.show();

	iricMainWindow()->setContinuousSnapshotInProgress(true);
	int stepBackup = currentStep();
	int step = m_exportSetting.startStep;
	int fileIndex = 1;
	QDir outputFolder(m_exportSetting.folder);

	std::unique_ptr<PostZoneDataExporter> exporter;
	if (m_exportFormat == PostDataExportDialog::Format::VTKASCII) {
		exporter = std::unique_ptr<PostZoneDataExporter> {new PostZoneDataVtkExporter{ projectData()->workDirectory(), PostZoneDataVtkExporter::Mode::ASCII }};
	} else if (m_exportFormat == PostDataExportDialog::Format::VTKBinary) {
		exporter = std::unique_ptr<PostZoneDataExporter> {new PostZoneDataVtkExporter{ projectData()->workDirectory(), PostZoneDataVtkExporter::Mode::BINARY }};
	} else if (m_exportFormat == PostDataExportDialog::Format::CSV) {
		exporter = std::unique_ptr<PostZoneDataExporter> {new PostZoneDataCsvExporter {}};
	} else if (m_exportFormat == PostDataExportDialog::Format::TPO) {
		exporter = std::unique_ptr<PostZoneDataExporter> {new PostZoneDataTpoExporter {}};
	} else if (m_exportFormat == PostDataExportDialog::Format::ESRIShape) {
		exporter = std::unique_ptr<PostZoneDataExporter> {new PostZoneDataShapeExporter {projectData()->workDirectory()}};
	}
	while (step <= m_exportSetting.endStep) {
		dialog.setValue(step);
		qApp->processEvents();
		if (dialog.wasCanceled()) {
			setCurrentStep(stepBackup);
			iricMainWindow()->setContinuousSnapshotInProgress(false);
			return;
		}
		setCurrentStep(step);
		double time = currentTimeStep();
		auto& s = m_exportSetting;
		QString fileName = outputFolder.absoluteFilePath(exporter->filename(s.prefix, fileIndex));
		bool ok = exporter->exportToFile(zoneC, fileName, time, s.iMin, s.iMax, s.jMin, s.jMax, s.kMin, s.kMax, projectData(), offset());
		if (! ok) {
			setCurrentStep(stepBackup);
			QMessageBox::critical(iricMainWindow(), tr("Error"), tr("Error occured while saving %1").arg(fileName));
			iricMainWindow()->setContinuousSnapshotInProgress(false);
			return;
		}
		step += m_exportSetting.skipRate;
		++ fileIndex;
	}
	iricMainWindow()->setContinuousSnapshotInProgress(false);
	setCurrentStep(stepBackup);
}

void PostSolutionInfo::exportCalculationResult(const std::string& folder, const std::string& prefix, const std::vector<int> steps, PostDataExportDialog::Format format)
{
	PostZoneDataContainer* cont = firstZoneContainer();
	int iMin, iMax, jMin, jMax, kMin, kMax;

	vtkStructuredGrid* sGrid = vtkStructuredGrid::SafeDownCast(cont->data());
	if (sGrid != nullptr) {
		// structured grid
		int dim[3];
		sGrid->GetDimensions(dim);
		iMin = 0; iMax = dim[0] - 1;
		jMin = 0; jMax = dim[1] - 1;
		kMin = 0; kMax = dim[2] - 1;
	}

	iricMainWindow()->setContinuousSnapshotInProgress(true);
	int stepBackup = currentStep();

	QDir outputFolder(folder.c_str());
	std::unique_ptr<PostZoneDataExporter> exporter;

	if (format == PostDataExportDialog::Format::VTKASCII) {
		exporter = std::unique_ptr<PostZoneDataExporter> {new PostZoneDataVtkExporter{ projectData()->workDirectory(), PostZoneDataVtkExporter::Mode::ASCII }};
	} else if (format == PostDataExportDialog::Format::VTKBinary) {
		exporter = std::unique_ptr<PostZoneDataExporter> {new PostZoneDataVtkExporter{ projectData()->workDirectory(), PostZoneDataVtkExporter::Mode::BINARY }};
	} else if (format == PostDataExportDialog::Format::CSV) {
		exporter = std::unique_ptr<PostZoneDataExporter> {new PostZoneDataCsvExporter {}};
	} else if (format == PostDataExportDialog::Format::ESRIShape) {
		exporter = std::unique_ptr<PostZoneDataExporter> {new PostZoneDataShapeExporter {projectData()->workDirectory()}};
	}

	int fileIndex = 1;
	for (int step : steps) {
		setCurrentStep(step);
		double time = currentTimeStep();
		QString fileName = outputFolder.absoluteFilePath(exporter->filename(prefix.c_str(), fileIndex));
		bool ok = exporter->exportToFile(cont, fileName, time, iMin, iMax, jMin, jMax, kMin, kMax, projectData(), offset());
		if (! ok) {
			setCurrentStep(stepBackup);
			iricMainWindow()->setContinuousSnapshotInProgress(false);
			return;
		}
		++ fileIndex;
	}
	iricMainWindow()->setContinuousSnapshotInProgress(false);
	setCurrentStep(stepBackup);
}

bool PostSolutionInfo::open()
{
	if (m_opener != nullptr) {
		return true;
	}

	try {
		m_opener = new CgnsFileOpener(iRIC::toStr(currentCgnsFileName()), CG_MODE_READ);
	} catch (const std::runtime_error&) {
		return false;
	}

	return true;
}

void PostSolutionInfo::clearContainers(QList<PostZoneDataContainer*>* conts)
{
	for (auto c : *conts) {
		delete c;
	}
	conts->clear();
}

void PostSolutionInfo::applyOffset(double x_diff, double y_diff)
{
	for (auto c1 : m_zoneContainers1D) {
		c1->applyOffset(x_diff, y_diff);
	}
	for (auto c2 : m_zoneContainers2D) {
		c2->applyOffset(x_diff, y_diff);
	}
	for (auto c3 : m_zoneContainers3D) {
		c3->applyOffset(x_diff, y_diff);
	}
}

void PostSolutionInfo::clearCalculatedResults(QMap<std::string, std::vector<PostCalculatedResult*> >* results)
{
	for (auto list : *results) {
		for (auto r : list) {
			delete r;
		}
	}
}
