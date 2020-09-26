#include "../post2dwindowdatamodel.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindownodevectorparticledataitem.h"
#include "post2dwindownodevectorparticlegroupdataitem.h"
#include "post2dwindowzonedataitem.h"

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

#include <QDomElement>
#include <QMenu>
#include <QMouseEvent>
#include <QSettings>
#include <QStandardItem>
#include <QUndoCommand>
#include <QXmlStreamWriter>

#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataWriter.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRungeKutta4.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkVertex.h>

#include <algorithm>

Post2dWindowNodeVectorParticleGroupDataItem::Setting::Setting() :
	CompositeContainer ({&target, &generateMode, &timeMode, &timeSamplingRate, &timeDivision, &arbitraryTimes, &particleSize, &regionMode}),
	target {"solution"},
	generateMode {"generationMode", gmPeriodical},
	timeMode {"timeMode", tmNormal},
	timeSamplingRate {"timeSamplingRate", 2},
	timeDivision {"timeDivision", 2},
	arbitraryTimes {"arbitraryTimes"},
	particleSize {"particleSize", DEFAULT_SIZE},
	regionMode {"regionMode", StructuredGridRegion::rmFull}
{}

Post2dWindowNodeVectorParticleGroupDataItem::Setting::Setting(const Setting& s) :
	Setting()
{
	CompositeContainer::copyValue(s);
}

Post2dWindowNodeVectorParticleGroupDataItem::Setting& Post2dWindowNodeVectorParticleGroupDataItem::Setting::operator=(const Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}

Post2dWindowNodeVectorParticleGroupDataItem::Post2dWindowNodeVectorParticleGroupDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem {tr("Particles (auto)"), QIcon(":/libs/guibase/images/iconFolder.png"), p},
	m_previousStep {-2},
	m_previousTime {0},
	m_nextStepToAddParticles {0},
	m_zScale {1}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	setupClipper();
	informGridUpdate();

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();
	for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(cont->data()->GetPointData())) {
		auto item = new Post2dWindowNodeVectorParticleDataItem(name, gt->solutionCaption(name), this);
		m_childItems.push_back(item);
	}
}

Post2dWindowNodeVectorParticleGroupDataItem::~Post2dWindowNodeVectorParticleGroupDataItem()
{
	clearParticleGrids();
	clearParticleActors();
}

void Post2dWindowNodeVectorParticleGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Particle Physical Value Change"));
	pushRenderCommand(cmd, this, true);
}

void Post2dWindowNodeVectorParticleGroupDataItem::updateActorSettings()
{
	clearParticleActors();
	m_particleMappers.clear();
	clearParticleGrids();

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return;}
	if (m_setting.target == "") {return;}
	vtkPointSet* ps = cont->data();
	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0) {return;}

	setupActors();

	setupParticleSources();
	resetParticles();
	updateVisibilityWithoutRendering();
	assignActorZValues(m_zDepthRange);
}

void Post2dWindowNodeVectorParticleGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	setTarget(m_setting.target);
	updateActorSettings();
}

void Post2dWindowNodeVectorParticleGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

void Post2dWindowNodeVectorParticleGroupDataItem::setupClipper()
{
	m_IBCClipper = vtkSmartPointer<vtkClipPolyData>::New();
	m_IBCClipper->SetValue(PostZoneDataContainer::IBCLimit);
	m_IBCClipper->InsideOutOff();
	m_IBCClipper->SetInputArrayToProcess(0, 0, 0, 0, iRIC::toStr(PostZoneDataContainer::IBC).c_str());
}

void Post2dWindowNodeVectorParticleGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(3);
}

void Post2dWindowNodeVectorParticleGroupDataItem::assignActorZValues(const ZDepthRange& range)
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

void Post2dWindowNodeVectorParticleGroupDataItem::informGridUpdate()
{
	clearParticleActors();
	m_particleMappers.clear();

	if (m_standardItem->checkState() == Qt::Unchecked) {return;}
	if (m_setting.target == "") {return;}
	PostZoneDataContainer* zoneContainer = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (zoneContainer == nullptr) {return;}
	unsigned int currentStep = 0;
	if (zoneContainer != nullptr) {
		currentStep = zoneContainer->solutionInfo()->currentStep();
	}
	setupActors();
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
	assignActorZValues(m_zDepthRange);

TIMEHANDLING:

	m_previousStep = currentStep;
	PostTimeSteps* tSteps = zoneContainer->solutionInfo()->timeSteps();
	if (m_previousStep < tSteps->timesteps().count()) {
		m_previousTime = tSteps->timesteps().at(m_previousStep);
	} else {
		m_previousTime = 0;
	}
}

void Post2dWindowNodeVectorParticleGroupDataItem::update()
{
	informGridUpdate();
}

std::string Post2dWindowNodeVectorParticleGroupDataItem::target() const
{
	return m_setting.target;
}

void Post2dWindowNodeVectorParticleGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	m_setting.target = target.c_str();
	updateActorSettings();
}

bool Post2dWindowNodeVectorParticleGroupDataItem::isOutput() const
{
	if (! isAncientChecked()) {return false;}
	if (standardItem()->checkState() != Qt::Checked) {return false;}
	if (m_setting.target == "") {return false;}

	auto cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr) {return false;}
	if (cont->data() == nullptr) {return false;}

	return true;
}

void Post2dWindowNodeVectorParticleGroupDataItem::resetParticles()
{
	clearParticleGrids();
	bool add = true;
	if (m_setting.generateMode == gmPeriodical) {
		// continue
	} else {
		const auto& timeVals = m_setting.arbitraryTimes.value();
		PostZoneDataContainer* zoneContainer = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
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
			vtkPolyData* polyData = setupPolyDataFromPoints(points);
			m_particleMappers[i]->SetInputData(polyData);
			m_particleGrids.push_back(polyData);
		} else {
			vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
			points->SetDataTypeToDouble();
			vtkPolyData* polyData = setupPolyDataFromPoints(points);
			m_particleGrids.push_back(polyData);
		}
	}
	PostZoneDataContainer* zoneContainer = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	unsigned int currentStep = zoneContainer->solutionInfo()->currentStep();
	if (m_setting.timeMode == tmSkip) {
		m_nextStepToAddParticles = currentStep + m_setting.timeSamplingRate;
	} else {
		m_nextStepToAddParticles = currentStep + 1;
	}
}

void Post2dWindowNodeVectorParticleGroupDataItem::addParticles()
{
	PostZoneDataContainer* zoneContainer = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	vtkPointSet* ps = zoneContainer->data();
	ps->GetPointData()->SetActiveVectors(iRIC::toStr(m_setting.target).c_str());

	unsigned int currentStep = zoneContainer->solutionInfo()->currentStep();

	PostTimeSteps* tSteps = zoneContainer->solutionInfo()->timeSteps();
	QList<double> timeSteps = tSteps->timesteps();
	double timeDiv = timeSteps[currentStep] - m_previousTime;

	vtkSmartPointer<vtkCustomStreamTracer> tracer = vtkSmartPointer<vtkCustomStreamTracer>::New();
	vtkStreamTracerUtil::setupForParticleTracking(tracer);
	tracer->SetInputData(getRegion());

	for (int i = 0; i < m_particleActors.size(); ++i) {
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
				vtkPointSet* newPoints = newParticles(i);
				if (m_setting.timeMode == tmSubdivide) {
					for (int j = 0; j < m_setting.timeDivision - 1; ++j) {
						double subTime = j * timeDiv / m_setting.timeDivision;
						tracer->SetMaximumIntegrationTime(subTime);
						tracer->SetSourceData(newPoints);
						tracer->Update();
						vtkStreamTracerUtil::addParticlePointsAtTime(points, tracer, subTime);
					}
				} else {
					for (vtkIdType j = 0; j < newPoints->GetNumberOfPoints(); ++j) {
						double v[3];
						newPoints->GetPoint(j, v);
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

		vtkPolyData* polyData = setupPolyDataFromPoints(points);
		m_particleMappers[i]->SetInputData(polyData);

		m_particleGrids[i]->Delete();
		m_particleGrids[i] = polyData;
	}
	if (m_setting.timeMode == tmSkip) {
		if (currentStep == m_nextStepToAddParticles) {
			m_nextStepToAddParticles = currentStep + m_setting.timeSamplingRate;
		}
	} else {
		m_nextStepToAddParticles = currentStep + 1;
	}
}

void Post2dWindowNodeVectorParticleGroupDataItem::innerUpdateZScale(double scale)
{
	m_zScale = scale;
	for (vtkActor* actor : m_particleActors) {
		actor->SetScale(1, scale, 1);
	}
}

bool Post2dWindowNodeVectorParticleGroupDataItem::exportParticles(const QString& filePrefix, int fileIndex, double time)
{
	for (int i = 0; i < m_particleGrids.size(); ++i) {
		QString tmpFile = projectData()->tmpFileName();

		vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
		QString header("iRIC particle output t = %1");
		writer->SetHeader(iRIC::toStr(header.arg(time)).c_str());
		writer->SetInputData(m_particleGrids[i]);
		writer->SetFileTypeToASCII();
		writer->SetFileName(iRIC::toStr(tmpFile).c_str());

		// export data.
		writer->Update();

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

vtkPointSet* Post2dWindowNodeVectorParticleGroupDataItem::getRegion()
{
	vtkPointSet* ps = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer()->data();
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

void Post2dWindowNodeVectorParticleGroupDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->initNodeResultAttributeBrowser();
}

void Post2dWindowNodeVectorParticleGroupDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->clearNodeResultAttributeBrowser();
}

void Post2dWindowNodeVectorParticleGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->updateNodeResultAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeVectorParticleGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->fixNodeResultAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeVectorParticleGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->showAttributeBrowserActionForNodeResult();
	menu->addAction(abAction);
}

vtkPolyData* Post2dWindowNodeVectorParticleGroupDataItem::setupPolyDataFromPoints(vtkPoints* points)
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

void Post2dWindowNodeVectorParticleGroupDataItem::clearParticleActors()
{
	auto r = renderer();
	for (auto actor : m_particleActors) {
		r->RemoveActor(actor);
	}
	m_particleActors.clear();
	m_actorCollection->RemoveAllItems();
}

void Post2dWindowNodeVectorParticleGroupDataItem::clearParticleGrids()
{
	for (auto grid : m_particleGrids) {
		grid->Delete();
	}
	m_particleGrids.clear();
}
