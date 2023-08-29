#include "../post2dwindowdatamodel.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindownodevectorparticledataitem.h"
#include "post2dwindownodevectorparticlegroupdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guibase/vtkCustomStreamTracer.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guibase/vtktool/vtkstreamtracerutil.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/posttimesteps.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>

#include <QMenu>
#include <QMouseEvent>
#include <QStandardItem>

#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

#include <algorithm>

Post2dWindowNodeVectorParticleGroupDataItem::Post2dWindowNodeVectorParticleGroupDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem {tr("Particles (auto)"), QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_previousStep {-2},
	m_previousTime {0},
	m_nextStepToAddParticles {0},
	m_zScale {1}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	informGridUpdate();

	auto cont = zoneDataItem()->dataContainer();
	auto gt = cont->gridType();
	for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(cont->data()->data()->GetPointData())) {
		auto caption = gt->vectorOutputCaption(name);
		auto item = new Post2dWindowNodeVectorParticleDataItem(name, caption, this);
		m_childItems.push_back(item);
	}
}

Post2dWindowNodeVectorParticleGroupDataItem::~Post2dWindowNodeVectorParticleGroupDataItem()
{
	clearParticles();
	clearParticleActors();
}

void Post2dWindowNodeVectorParticleGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Particle Physical Value Change"));
	pushRenderCommand(cmd, this, true);
}

void Post2dWindowNodeVectorParticleGroupDataItem::updateActorSetting()
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(iRIC::toStr(m_setting.target), m_childItems, true);

	clearParticleActors();
	clearParticles();

	auto cont = zoneDataItem()->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return;}
	if (m_setting.target == "") {return;}
	vtkPointSet* ps = cont->data()->data();
	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0) {return;}

	setupActors();

	resetParticles();

	updateVisibilityWithoutRendering();
	assignActorZValues(m_zDepthRange);
}

void Post2dWindowNodeVectorParticleGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	setTarget(m_setting.target);
}

void Post2dWindowNodeVectorParticleGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
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
	for (int i = 0; i < static_cast<int> (m_particleActors.size()); ++i) {
		double depth = range.min() + static_cast<double>(i) / (m_particleActors.size() - 1) * (range.max() - range.min());
		m_particleActors[i]->SetPosition(0, 0, depth);
	}
}

void Post2dWindowNodeVectorParticleGroupDataItem::informGridUpdate()
{
	clearParticleActors();

	if (m_standardItem->checkState() == Qt::Unchecked) {return;}
	if (m_setting.target == "") {return;}

	auto zoneContainer = zoneDataItem()->dataContainer();
	if (zoneContainer == nullptr) {return;}

	int currentStep = 0;
	if (zoneContainer != nullptr) {
		currentStep = zoneContainer->solutionInfo()->currentStep();
	}
	setupActors();
	if (zoneContainer == nullptr || zoneContainer->data() == nullptr) {
		resetParticles();
		goto TIMEHANDLING;
	}

	if (currentStep != 0 && (currentStep == m_previousStep + 1 || projectData()->mainWindow()->continuousSnapshotInProgress())) {
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
	updateActorSetting();
}

bool Post2dWindowNodeVectorParticleGroupDataItem::isOutput() const
{
	if (! isAncientChecked()) {return false;}
	if (standardItem()->checkState() != Qt::Checked) {return false;}
	if (m_setting.target == "") {return false;}

	auto cont = zoneDataItem()->dataContainer();
	if (cont == nullptr) {return false;}
	if (cont->data() == nullptr) {return false;}

	return true;
}

void Post2dWindowNodeVectorParticleGroupDataItem::resetParticles()
{
	clearParticles();
	bool add = true;
	if (m_setting.generateMode == ParticleSettingContainer::GenerateMode::Periodical) {
		// continue
	} else {
		const auto& timeVals = m_setting.arbitraryTimes.value();
		auto zoneContainer = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
		int s = zoneContainer->solutionInfo()->currentStep();
		bool found = std::binary_search(timeVals.begin(), timeVals.end(), s);
		if (! found) {
			add = false;
		}
	}
	for (int i = 0; i < static_cast<int> (m_particleActors.size()); ++i) {
		auto points = vtkSmartPointer<vtkPoints>::New();
		points->SetDataTypeToDouble();
		if (add) {
			auto particles = newParticles(i);
			if (particles != nullptr) {
				for (vtkIdType i = 0; i < particles->GetNumberOfPoints(); ++i) {
					double p[3];
					particles->GetPoint(i, p);
					points->InsertNextPoint(p);
				}
			}
		}
		vtkPolyData* polyData = setupPolyDataFromPoints(points);
		m_particles.push_back(polyData);

		auto mapper = vtkPolyDataMapper::SafeDownCast(m_particleActors[i]->GetMapper());
		mapper->SetInputData(polyData);
	}

	auto zoneContainer = zoneDataItem()->dataContainer();
	unsigned int currentStep = zoneContainer->solutionInfo()->currentStep();
	if (m_setting.timeMode == ParticleSettingContainer::TimeMode::Skip) {
		m_nextStepToAddParticles = currentStep + m_setting.timeSamplingRate;
	} else {
		m_nextStepToAddParticles = currentStep + 1;
	}
}

void Post2dWindowNodeVectorParticleGroupDataItem::addParticles()
{
	auto zoneContainer = zoneDataItem()->dataContainer();
	vtkPointSet* ps = zoneContainer->data()->data();
	ps->GetPointData()->SetActiveVectors(iRIC::toStr(m_setting.target).c_str());

	int currentStep = zoneContainer->solutionInfo()->currentStep();

	PostTimeSteps* tSteps = zoneContainer->solutionInfo()->timeSteps();
	QList<double> timeSteps = tSteps->timesteps();
	double timeDiv = timeSteps[currentStep] - m_previousTime;

	auto tracer = vtkSmartPointer<vtkCustomStreamTracer>::New();
	vtkStreamTracerUtil::setupForParticleTracking(tracer);

	auto region = getRegion();
	tracer->SetInputData(region);
	region->Delete();

	for (int i = 0; i < static_cast<int>(m_particleActors.size()); ++i) {
		auto points = vtkSmartPointer<vtkPoints>::New();
		points->SetDataTypeToDouble();

		tracer->SetMaximumIntegrationTime(timeDiv);
		tracer->SetSourceData(m_particles[i]);
		tracer->Update();
		vtkStreamTracerUtil::addParticlePointsAtTime(points, tracer, timeDiv);

		// add new particles.
		if (m_setting.generateMode == ParticleSettingContainer::GenerateMode::Periodical) {
			// periodical
			if (currentStep == m_nextStepToAddParticles) {
				auto newPoints = newParticles(i);
				points->Resize(points->GetNumberOfPoints() + newPoints->GetNumberOfPoints());
				for (vtkIdType j = 0; j < newPoints->GetNumberOfPoints(); ++j) {
					double v[3];
					newPoints->GetPoint(j, v);
					points->InsertNextPoint(v);
				}

				if (m_setting.timeMode == ParticleSettingContainer::TimeMode::Subdivide) {
					for (int j = 0; j < m_setting.timeDivision - 1; ++j) {
						double subTime = (j + 1) * timeDiv / m_setting.timeDivision;
						tracer->SetMaximumIntegrationTime(subTime);
						tracer->SetSourceData(newPoints);
						tracer->Update();
						vtkStreamTracerUtil::addParticlePointsAtTime(points, tracer, subTime);
					}
				}
				newPoints->Delete();
			}
		} else {
			// arbitrary
			const auto& timeVals = m_setting.arbitraryTimes.value();
			int s = currentStep;
			bool found = std::binary_search(timeVals.begin(), timeVals.end(), s);
			if (found) {
				auto newPoints = newParticles(i);
				points->Resize(points->GetNumberOfPoints() + newPoints->GetNumberOfPoints());
				for (vtkIdType j = 0; j < newPoints->GetNumberOfPoints(); ++j) {
					double v[3];
					newPoints->GetPoint(j, v);
					points->InsertNextPoint(v);
				}
				newPoints->Delete();
			}
		}
		points->Modified();

		auto polyData = setupPolyDataFromPoints(points);
		m_particles[i]->Delete();
		m_particles[i] = polyData;
		auto mapper = vtkPolyDataMapper::SafeDownCast(m_particleActors[i]->GetMapper());
		mapper->SetInputData(polyData);
	}
	if (m_setting.timeMode == ParticleSettingContainer::TimeMode::Skip) {
		if (currentStep == m_nextStepToAddParticles) {
			m_nextStepToAddParticles = currentStep + m_setting.timeSamplingRate;
		}
	} else {
		m_nextStepToAddParticles = currentStep + 1;
	}
}

void Post2dWindowNodeVectorParticleGroupDataItem::setupActors()
{
	for (int i = 0; i < numberOfActors(); ++i) {
		auto actor = setupActor(i);
		actor->GetProperty()->SetLighting(false);
		actor->SetScale(1, m_zScale, 1);

		auto mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		actor->SetMapper(mapper);
		mapper->Delete();

		renderer()->AddActor(actor);
		actorCollection()->AddItem(actor);
		m_particleActors.push_back(actor);

		actor->Delete();
	}
}

Post2dWindowZoneDataItem* Post2dWindowNodeVectorParticleGroupDataItem::zoneDataItem() const
{
	return dynamic_cast<Post2dWindowZoneDataItem*> (parent());
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
	for (int i = 0; i < m_particles.size(); ++i) {
		QString tmpFile = projectData()->tmpFileName();

		vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
		QString header("iRIC particle output t = %1");
		writer->SetHeader(iRIC::toStr(header.arg(time)).c_str());
		writer->SetInputData(m_particles[i]);
		writer->SetFileTypeToASCII();
		writer->SetFileName(iRIC::toStr(tmpFile).c_str());

		// export data.
		writer->Update();

		QString filename = filePrefix;
		if (m_particles.size() == 1) {
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

void Post2dWindowNodeVectorParticleGroupDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	zoneDataItem()->initNodeResultAttributeBrowser();
}

void Post2dWindowNodeVectorParticleGroupDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	zoneDataItem()->clearNodeResultAttributeBrowser();
}

void Post2dWindowNodeVectorParticleGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	zoneDataItem()->updateNodeResultAttributeBrowser(event->pos(), v);
}

void Post2dWindowNodeVectorParticleGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	zoneDataItem()->fixNodeResultAttributeBrowser(event->pos(), v);
}

void Post2dWindowNodeVectorParticleGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = zoneDataItem()->showAttributeBrowserActionForNodeResult();
	menu->addAction(abAction);
}

vtkPolyData* Post2dWindowNodeVectorParticleGroupDataItem::setupPolyDataFromPoints(vtkPoints* points)
{
	auto ret = vtkPolyData::New();
	ret->SetPoints(points);

	vtkIdType numPoints = points->GetNumberOfPoints();
	auto ca = vtkSmartPointer<vtkCellArray>::New();
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
	m_actorCollection->RemoveAllItems();

	m_particleActors.clear();
}

void Post2dWindowNodeVectorParticleGroupDataItem::clearParticles()
{
	for (auto data : m_particles) {
		data->Delete();
	}
	m_particles.clear();
}
