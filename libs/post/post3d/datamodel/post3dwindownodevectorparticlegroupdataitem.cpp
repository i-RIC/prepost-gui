#include "../post3dwindowdatamodel.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindownodevectorparticledataitem.h"
#include "post3dwindownodevectorparticlegroupdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guibase/vtkCustomStreamTracer.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guibase/vtktool/vtkstreamtracerutil.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/posttimesteps.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/filesystemfunction.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>

#include <QDir>
#include <QDomNode>
#include <QFile>
#include <QSettings>
#include <QStandardItem>
#include <QUndoCommand>
#include <QXmlStreamWriter>

#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRungeKutta4.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkVertex.h>


Post3dWindowNodeVectorParticleGroupDataItem::Setting::Setting() :
	CompositeContainer ({&target, &generateMode, &timeMode, &timeSamplingRate, &timeDivision, &arbitraryTimes, &regionMode}),
	target {"solution"},
	generateMode {"generationMode", gmPeriodical},
	timeMode {"timeMode", tmNormal},
	timeSamplingRate {"timeSamplingRate", 2},
	timeDivision {"timeDivision", 2},
	arbitraryTimes {"arbitraryTimes"},
	regionMode {"regionMode", StructuredGridRegion::rmFull}
{}

Post3dWindowNodeVectorParticleGroupDataItem::Setting::Setting(const Setting& s) :
	Setting()
{
	CompositeContainer::copyValue(s);
}

Post3dWindowNodeVectorParticleGroupDataItem::Setting& Post3dWindowNodeVectorParticleGroupDataItem::Setting::operator=(const Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}

Post3dWindowNodeVectorParticleGroupDataItem::Post3dWindowNodeVectorParticleGroupDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem {tr("Particles (auto)"), QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_previousStep {-2},
	m_previousTime {0},
	m_nextStepToAddParticles {0},
	m_zScale {1}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	setDefaultValues();
	setupClipper();
	informGridUpdate();

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();
	for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(cont->data()->data()->GetPointData())) {
		auto item = new Post3dWindowNodeVectorParticleDataItem(name, gt->solutionCaption(name), this);
		m_childItems.push_back(item);
	}
}

Post3dWindowNodeVectorParticleGroupDataItem::~Post3dWindowNodeVectorParticleGroupDataItem()
{
	clearParticleActors();
	m_particleMappers.clear();
	clearParticleGrids();
}

void Post3dWindowNodeVectorParticleGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Particle Physical Value Change"));
	pushRenderCommand(cmd, this, true);
}

void Post3dWindowNodeVectorParticleGroupDataItem::setDefaultValues()
{
	m_setting.target= "";

	m_setting.timeMode = tmNormal;
	m_setting.timeSamplingRate = 2;
	m_setting.timeDivision = 2;
	m_setting.regionMode = StructuredGridRegion::rmFull;
}

void Post3dWindowNodeVectorParticleGroupDataItem::updateActorSettings()
{
	clearParticleActors();
	m_particleMappers.clear();
	clearParticleGrids();

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return;}
	if (m_setting.target == "") { return; }
	vtkPointSet* ps = cont->data()->data();
	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0) {return;}

	setupActors();

	applyZScale();
	setupParticleSources();
	resetParticles();
	updateVisibilityWithoutRendering();
}

void Post3dWindowNodeVectorParticleGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	setTarget(m_setting.target);
	updateActorSettings();
}

void Post3dWindowNodeVectorParticleGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

void Post3dWindowNodeVectorParticleGroupDataItem::setupClipper()
{
	m_IBCClipper = vtkSmartPointer<vtkClipPolyData>::New();
	m_IBCClipper->SetValue(PostZoneDataContainer::IBCLimit);
	m_IBCClipper->InsideOutOff();
	m_IBCClipper->SetInputArrayToProcess(0, 0, 0, 0, iRIC::toStr(PostZoneDataContainer::IBC).c_str());
}

void Post3dWindowNodeVectorParticleGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(3);
}

void Post3dWindowNodeVectorParticleGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	if (m_particleActors.size() == 0) {return;}
	if (m_particleActors.size() == 1) {
		m_particleActors[0]->SetPosition(0, 0, range.max());
		return;
	}
	for (int i = 0; i < m_particleActors.size(); ++i) {
		double depth = range.min() + static_cast<double>(i) / (m_particleActors.size() - 1) * (range.max() - range.min());
		m_particleActors[i]->SetPosition(0, 0, depth);
	}
}

void Post3dWindowNodeVectorParticleGroupDataItem::informGridUpdate()
{
	clearParticleActors();
	m_particleMappers.clear();

	if (m_standardItem->checkState() == Qt::Unchecked) {return;}
	if (m_setting.target == "") {return;}
	PostZoneDataContainer* zoneContainer = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (zoneContainer == nullptr) {return;}
	int currentStep = 0;
	if (zoneContainer != nullptr) {
		currentStep = zoneContainer->solutionInfo()->currentStep();
	}
	setupActors();
	applyZScale();
	if (zoneContainer == nullptr || zoneContainer->data() == nullptr) {
		resetParticles();
		goto TIMEHANDLING;
	}
	setupParticleSources();
	if (currentStep != 0 && (currentStep == m_previousStep + 1 || projectData()->mainWindow()->continuousSnapshotInProgress())) {
		// one increment add particles!
		addParticles();
	} else {
		// reset particles.
		resetParticles();
	}
	updateVisibilityWithoutRendering();

TIMEHANDLING:

	m_previousStep = currentStep;
	PostTimeSteps* tSteps = zoneContainer->solutionInfo()->timeSteps();
	if (m_previousStep < tSteps->timesteps().count()) {
		m_previousTime = tSteps->timesteps().at(m_previousStep);
	} else {
		m_previousTime = 0;
	}
}

void Post3dWindowNodeVectorParticleGroupDataItem::update()
{
	informGridUpdate();
}

void Post3dWindowNodeVectorParticleGroupDataItem::innerUpdateZScale(double zscale)
{
	m_zScale = zscale;
	applyZScale();
}

void Post3dWindowNodeVectorParticleGroupDataItem::applyZScale()
{
	for (auto actor : m_particleActors) {
		actor->SetScale(1, 1, m_zScale);
	}
}

void Post3dWindowNodeVectorParticleGroupDataItem::resetParticles()
{
	clearParticleGrids();
	bool add = true;
	if (m_setting.generateMode == gmPeriodical) {
		// continue
	} else {
		const auto& timeVals = m_setting.arbitraryTimes.value();
		PostZoneDataContainer* zoneContainer = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
		int s = zoneContainer->solutionInfo()->currentStep();
		bool found = std::binary_search(timeVals.begin(), timeVals.end(), s);
		if (! found) {
			add = false;
		}
	}
	for (int i = 0; i < m_particleActors.size(); ++i) {
		if (add) {
			vtkPointSet* pointsGrid = newParticles(i);
			vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
			points->SetDataTypeToDouble();
			if (pointsGrid != nullptr) {
				for (vtkIdType i = 0; i < pointsGrid->GetNumberOfPoints(); ++i) {
					double p[3];
					pointsGrid->GetPoint(i, p);
					points->InsertNextPoint(p);
				}
			}
			vtkPolyData* grid = setupPolyDataFromPoints(points);
			m_particleMappers[i]->SetInputData(grid);
			m_particleGrids.push_back(grid);
		} else {
			vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
			points->SetDataTypeToDouble();
			vtkPolyData* polyData = setupPolyDataFromPoints(points);
			m_particleGrids.push_back(polyData);
		}
	}
	PostZoneDataContainer* zoneContainer = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	unsigned int currentStep = zoneContainer->solutionInfo()->currentStep();
	if (m_setting.timeMode == tmSkip) {
		m_nextStepToAddParticles = currentStep + m_setting.timeSamplingRate;
	} else {
		m_nextStepToAddParticles = currentStep + 1;
	}
}

void Post3dWindowNodeVectorParticleGroupDataItem::addParticles()
{
	PostZoneDataContainer* zoneContainer = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	vtkPointSet* ps = zoneContainer->data()->data();
	ps->GetPointData()->SetActiveVectors(iRIC::toStr(m_setting.target).c_str());

	int currentStep = zoneContainer->solutionInfo()->currentStep();

	PostTimeSteps* tSteps = zoneContainer->solutionInfo()->timeSteps();
	QList<double> timeSteps = tSteps->timesteps();
	double timeDiv = timeSteps[currentStep] - m_previousTime;

	vtkSmartPointer<vtkCustomStreamTracer> tracer = vtkSmartPointer<vtkCustomStreamTracer>::New();
	vtkStreamTracerUtil::setupForParticleTracking(tracer);
	tracer->SetInputData(getRegion());

	for (int i = 0; i < m_particleActors.size(); ++i) {
		// Find the new positions of points already exists.

		vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
		points->SetDataTypeToDouble();

		tracer->SetMaximumIntegrationTime(timeDiv);
		tracer->SetSourceData(m_particleGrids[i]);
		tracer->Update();
		vtkStreamTracerUtil::addParticlePointsAtTime(points, tracer, timeDiv);

		// add new particles.
		if (m_setting.generateMode == gmPeriodical) {
			// periodical
			if (currentStep == m_nextStepToAddParticles) {
				vtkPointSet* pointsGrid = newParticles(i);
				if (m_setting.timeMode == tmSubdivide) {
					for (int j = 0; j < m_setting.timeDivision - 1; ++j) {
						double subTime = j * timeDiv / m_setting.timeDivision;
						tracer->SetMaximumIntegrationTime(subTime);
						tracer->SetSourceData(pointsGrid);
						tracer->Update();
						vtkStreamTracerUtil::addParticlePointsAtTime(points, tracer, subTime);
					}
				} else {
					for (vtkIdType j = 0; j < pointsGrid->GetNumberOfPoints(); ++j) {
						double v[3];
						pointsGrid->GetPoint(j, v);
						points->InsertNextPoint(v);
					}
				}
			}
		} else {
			// arbitrary
			const auto& timeVals = m_setting.arbitraryTimes.value();
			int s = currentStep;
			bool found = std::binary_search(timeVals.begin(), timeVals.end(), s);
			if (found) {
				vtkPointSet* newPoints = newParticles(i);
				for (vtkIdType j = 0; j < newPoints->GetNumberOfPoints(); ++j) {
					double v[3];
					newPoints->GetPoint(j, v);
					points->InsertNextPoint(v);
				}
			}
		}
		points->Modified();

		vtkPolyData* newPoints = setupPolyDataFromPoints(points);
		m_particleMappers[i]->SetInputData(newPoints);
		m_particleGrids[i]->Delete();
		m_particleGrids[i] = newPoints;
	}
	if (m_setting.timeMode == tmSkip) {
		if (currentStep == m_nextStepToAddParticles) {
			m_nextStepToAddParticles = currentStep + m_setting.timeSamplingRate;
		}
	} else {
		m_nextStepToAddParticles = currentStep + 1;
	}
}

vtkPolyData* Post3dWindowNodeVectorParticleGroupDataItem::setupPolyDataFromPoints(vtkPoints* points)
{
	vtkPolyData* ret = vtkPolyData::New();
	ret->SetPoints(points);

	vtkIdType numPoints = points->GetNumberOfPoints();
	vtkSmartPointer<vtkCellArray> ca = vtkSmartPointer<vtkCellArray>::New();
	for (vtkIdType i = 0; i < numPoints; ++i) {
			ca->InsertNextCell(1, &i);
	}
	ret->SetVerts(ca);
	ret->Modified();

	return ret;
}

void Post3dWindowNodeVectorParticleGroupDataItem::clearParticleActors()
{
	auto r = renderer();
	for (auto actor : m_particleActors) {
		r->RemoveActor(actor);
	}
	m_particleActors.clear();
	m_actorCollection->RemoveAllItems();
}

void Post3dWindowNodeVectorParticleGroupDataItem::clearParticleGrids()
{
		for (auto grid : m_particleGrids) {
				grid->Delete();
		}
		m_particleGrids.clear();
}

bool Post3dWindowNodeVectorParticleGroupDataItem::exportParticles(const QString& filePrefix, int fileIndex, double time)
{
	for (int i = 0; i < m_particleGrids.size(); ++i) {
		QString tempPath = QDir::tempPath();
		QString tmpFile = iRIC::getTempFileName(tempPath);

		vtkUnstructuredGridWriter* writer = vtkUnstructuredGridWriter::New();
		QString header("iRIC particle output t = %1");
		writer->SetHeader(iRIC::toStr(header.arg(time)).c_str());
		writer->SetInputData(m_particleGrids[i]);
		writer->SetFileTypeToASCII();
		writer->SetFileName(iRIC::toStr(tmpFile).c_str());

		// export data.
		writer->Update();
		writer->Delete();

		QString filename = filePrefix;
		if (m_particleGrids.size() == 1) {
			filename.append(QString("%1.vtk").arg(fileIndex));
		} else {
			filename.append(QString("Group%1_%2.vtk").arg(i + 1).arg(fileIndex));
		}
		// rename the temporary file to the target file.
		if (QFile::exists(filename)) {
			// remove first.
			if (! QFile::remove(filename)) {
				// unable to remove. fail.
				QFile::remove(tmpFile);
				return false;
			}
		}
		bool ok = QFile::rename(tmpFile, filename);
		if (! ok) {
			// rename failed.
			QFile::remove(tmpFile);
			return false;
		}
	}
	return true;
}

std::string Post3dWindowNodeVectorParticleGroupDataItem::target() const
{
	return m_setting.target;
}

void Post3dWindowNodeVectorParticleGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);

	m_setting.target.setValue(target.c_str());
	updateActorSettings();
}

bool Post3dWindowNodeVectorParticleGroupDataItem::isOutput() const
{
	if (! isAncientChecked()) {return false;}
	if (standardItem()->checkState() != Qt::Checked) {return false;}
	if (m_setting.target == "") {return false;}

	auto cont = dynamic_cast<Post3dWindowZoneDataItem*> (parent())->dataContainer();
	if (cont == nullptr) {return false;}
	if (cont->data() == nullptr) {return false;}

	return true;
}

vtkPointSet* Post3dWindowNodeVectorParticleGroupDataItem::getRegion()
{
	vtkPointSet* ps = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer()->data()->data();
	if (m_setting.regionMode == StructuredGridRegion::rmFull) {
		return ps;
	} else if (m_setting.regionMode == StructuredGridRegion::rmActive) {
		vtkSmartPointer<vtkStructuredGridGeometryFilter> geoFilter = vtkSmartPointer<vtkStructuredGridGeometryFilter>::New();
		geoFilter->SetInputData(ps);
		geoFilter->Update();
		ps->GetPointData()->SetActiveScalars(iRIC::toStr(PostZoneDataContainer::IBC).c_str());
		m_IBCClipper->SetInputConnection(geoFilter->GetOutputPort());
		m_IBCClipper->Update();
		m_regionClippedPolyData = vtkSmartPointer<vtkPolyData>::New();
		m_regionClippedPolyData->DeepCopy(m_IBCClipper->GetOutput());
		m_regionClippedPolyData->GetPointData()->SetActiveScalars("");
		return m_regionClippedPolyData;
	}
	return nullptr;
}
