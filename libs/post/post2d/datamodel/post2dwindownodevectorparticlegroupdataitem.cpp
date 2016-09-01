#include "../post2dwindowdatamodel.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindownodevectorparticledataitem.h"
#include "post2dwindownodevectorparticlegroupdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guibase/vtkdatasetattributestool.h>
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

Post2dWindowNodeVectorParticleGroupDataItem::Setting::Setting() :
	CompositeContainer ({&target, &timeMode, &timeSamplingRate, &timeDivision, &particleSize, &regionMode}),
	target {"solution"},
	timeMode {"timeMode", tmNormal},
	timeSamplingRate {"timeSamplingRate", 2},
	timeDivision {"timeDivision", 2},
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
	for (int i = 0; i < m_particleActors.size(); ++i) {
		renderer()->RemoveActor(m_particleActors[i]);
	}
}

void Post2dWindowNodeVectorParticleGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Particle Physical Value Change"));
	pushRenderCommand(cmd, this, true);
}

void Post2dWindowNodeVectorParticleGroupDataItem::updateActorSettings()
{
	for (auto actor : m_particleActors) {
		renderer()->RemoveActor(actor);
	}
	m_actorCollection->RemoveAllItems();
	m_particleActors.clear();
	m_particleMappers.clear();
	m_particleGrids.clear();

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return;}
	if (m_setting.target == "") {return;}
	vtkPointSet* ps = cont->data();
	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0) {return;}

	setupActors();

	setupStreamTracer();
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


void Post2dWindowNodeVectorParticleGroupDataItem::setupStreamTracer()
{
	m_streamTracer = vtkSmartPointer<vtkStreamPoints>::New();
	m_streamPoints = vtkSmartPointer<vtkCustomStreamPoints>::New();

	m_streamTracer->SetInputData(getRegion());
	m_streamTracer->SetIntegrationDirectionToForward();

	m_streamPoints->SetInputData(getRegion());
	m_streamPoints->SetIntegrationDirectionToForward();
}

void Post2dWindowNodeVectorParticleGroupDataItem::informGridUpdate()
{
	for (auto actor : m_particleActors) {
		renderer()->RemoveActor(actor);
	}
	m_actorCollection->RemoveAllItems();
	m_particleActors.clear();
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
	setupStreamTracer();
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

void Post2dWindowNodeVectorParticleGroupDataItem::resetParticles()
{
	m_particleGrids.clear();
	for (int i = 0; i < m_particleActors.size(); ++i) {
		vtkPolyData* grid = vtkPolyData::New();
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
		grid->SetPoints(points);
		grid->Allocate(points->GetNumberOfPoints());
		vtkSmartPointer<vtkVertex> vertex = vtkSmartPointer<vtkVertex>::New();
		for (vtkIdType j = 0; j < points->GetNumberOfPoints(); ++j) {
			vertex->GetPointIds()->SetId(0, j);
			grid->InsertNextCell(vertex->GetCellType(), vertex->GetPointIds());
		}
		grid->BuildLinks();
		grid->Modified();
		m_particleMappers[i]->SetInputData(grid);
		m_particleGrids.push_back(grid);
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

	for (int i = 0; i < m_particleActors.size(); ++i) {
		// Find the new positions of points already exists.
		m_streamPoints->SetSourceData(m_particleGrids[i]);
		m_streamPoints->SetMaximumPropagationTime(timeDiv * 1.1);
		m_streamPoints->SetTimeIncrement(timeDiv);
		m_streamPoints->Update();

		vtkPolyData* p = m_streamPoints->GetOutput();
		vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
		points->SetDataTypeToDouble();
		for (vtkIdType j = 0; j < p->GetNumberOfPoints(); ++j) {
			double v[3];
			p->GetPoint(j, v);
			points->InsertNextPoint(v);
		}
		// add new particles.
		if (currentStep == m_nextStepToAddParticles) {
			vtkPointSet* pointsGrid = newParticles(i);
			if (m_setting.timeMode == tmSubdivide) {
				for (int j = 0; j < m_setting.timeDivision - 1; ++j) {
					m_streamTracer->SetSourceData(pointsGrid);
					m_streamTracer->SetMaximumPropagationTime(timeDiv * (1 - 0.5 / m_setting.timeDivision));
					m_streamTracer->SetTimeIncrement(timeDiv / m_setting.timeDivision);
					m_streamTracer->Update();
					vtkPolyData* p = m_streamTracer->GetOutput();
					for (vtkIdType k = 0; k < p->GetNumberOfPoints(); ++k) {
						double v[3];
						p->GetPoint(k, v);
						points->InsertNextPoint(v);
					}
				}
			} else {
				for (vtkIdType j = 0; j < pointsGrid->GetNumberOfPoints(); ++j) {
					double v[3];
					pointsGrid->GetPoint(j, v);
					points->InsertNextPoint(v);
				}
			}
		}
		points->Modified();

		vtkPolyData* newPoints = vtkPolyData::New();
		newPoints->SetPoints(points);
		vtkIdType numPoints = points->GetNumberOfPoints();
		vtkSmartPointer<vtkCellArray> ca = vtkSmartPointer<vtkCellArray>::New();
		for (vtkIdType j = 0; j < numPoints; ++j) {
			ca->InsertNextCell(1, &j);
		}
		newPoints->SetVerts(ca);
		newPoints->Modified();
		m_particleMappers[i]->SetInputData(newPoints);
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
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->initNodeAttributeBrowser();
}

void Post2dWindowNodeVectorParticleGroupDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->clearNodeAttributeBrowser();
}

void Post2dWindowNodeVectorParticleGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->updateNodeAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeVectorParticleGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->fixNodeAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeVectorParticleGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->showNodeAttributeBrowserAction();
	menu->addAction(abAction);
}
