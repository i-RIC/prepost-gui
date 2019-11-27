#include "post2dwindownodevectorparticlegroupstructureddataitem.h"
#include "post2dwindowparticlestructuredsettingdialog.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindownodevectorparticlegroupstructureddataitem_setsettingcommand.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/project/projectdata.h>
#include <misc/xmlsupport.h>

#include <QDomElement>
#include <QSettings>
#include <QXmlStreamWriter>

#include <vtkProperty.h>
#include <vtkRenderer.h>

Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting::Setting() :
	CompositeContainer ({&range, &spaceMode, &spaceSamplingRate, &spaceDivision, &color, &size}),
	range {},
	spaceMode {"spaceMode", smNormal},
	spaceSamplingRate {"spaceSamplingRate", 2},
	spaceDivision {"spaceDivision", 2},
	color {"color"},
	size {"size", DEFAULT_SIZE}
{
	QSettings settings;
	color = settings.value("post2d/particlecolor", QColor(Qt::black)).value<QColor>();

	range.setPrefix("region");
}

Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting::Setting(const Setting& s) :
	Setting()
{
	CompositeContainer::copyValue(s);
}

Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting& Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting::operator=(const Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}

QDialog* Post2dWindowNodeVectorParticleGroupStructuredDataItem::propertyDialog(QWidget* p)
{
	Post2dWindowParticleStructuredSettingDialog* dialog = new Post2dWindowParticleStructuredSettingDialog(p);
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == 0 || cont->data() == 0) {
		delete dialog;
		return 0;
	}
	dialog->setProjectMainFile(projectData()->mainfile());
	dialog->setZoneData(cont);
	dialog->setActiveAvailable(cont->IBCExists());

	dialog->setSettings(m_setting, m_stSettings);

	return dialog;
}

void Post2dWindowNodeVectorParticleGroupStructuredDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	Post2dWindowParticleStructuredSettingDialog* dialog = dynamic_cast<Post2dWindowParticleStructuredSettingDialog*>(propDialog);
	pushRenderCommand(new SetSettingCommand(dialog->setting(), dialog->stSettings(), this), this, true);
}

void Post2dWindowNodeVectorParticleGroupStructuredDataItem::setDefaultValues()
{
	m_stSettings.clear();

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	vtkStructuredGrid* grid = dynamic_cast<vtkStructuredGrid*>(cont->data());
	int dim[3];
	grid->GetDimensions(dim);

	Setting s;
	s.range.iMin = 0;
	s.range.iMax = 0;
	s.range.jMin = 0;
	s.range.jMax = dim[1] - 1;

	m_stSettings.append(s);
}

void Post2dWindowNodeVectorParticleGroupStructuredDataItem::setupParticleSources()
{
	PostZoneDataContainer* zoneContainer = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	for (int i = 0; i < m_extractGrids.count(); ++i) {
		m_extractGrids[i]->Delete();
	}
	m_extractGrids.clear();
	for (int i = 0; i < m_subdivideGrids.count(); ++i) {
		m_subdivideGrids[i]->Delete();
	}
	m_subdivideGrids.clear();
	for (int i = 0; i < m_stSettings.count(); ++i) {
		const Setting& s = m_stSettings[i];
		vtkExtractGrid* ext = vtkExtractGrid::New();
		ext->SetInputData(zoneContainer->data());
		vtkSubdivideGrid* div = vtkSubdivideGrid::New();
		div->SetInputData(zoneContainer->data());

		auto& r = s.range;
		ext->SetVOI(r.iMin, r.iMax, r.jMin, r.jMax, 0, 0);
		div->SetVOI(r.iMin, r.iMax, r.jMin, r.jMax, 0, 0);
		ext->SetSampleRate(1, 1, 1);
		if (s.spaceMode == Setting::smSkip) {
			ext->SetSampleRate(s.spaceSamplingRate, s.spaceSamplingRate, 1);
		}
		div->SetDivideRate(s.spaceDivision, s.spaceDivision, 1);
		ext->Modified();
		div->Modified();

		m_extractGrids.append(ext);
		m_subdivideGrids.append(div);
	}
}

void Post2dWindowNodeVectorParticleGroupStructuredDataItem::setupActors()
{
	for (int i = 0; i < m_stSettings.count(); ++i) {
		const Setting& s = m_stSettings[i];
		vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
		vtkProperty* prop = actor->GetProperty();
		prop->SetLighting(false);
		prop->SetColor(s.color);
		prop->SetPointSize(s.size);
		actor->SetScale(1, m_zScale, 1);

		renderer()->AddActor(actor);
		actorCollection()->AddItem(actor);

		vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
		actor->SetMapper(mapper);

		m_particleActors.push_back(actor);
		m_particleMappers.push_back(mapper);
	}
}

vtkPointSet* Post2dWindowNodeVectorParticleGroupStructuredDataItem::newParticles(int i)
{
	vtkStructuredGrid* exGrid = nullptr;
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return nullptr;}
	const Setting& s = m_stSettings[i];
	switch (Setting::SpaceMode(s.spaceMode)) {
	case Setting::smNormal:
	case Setting::smSkip:
		m_extractGrids[i]->Update();
		exGrid = m_extractGrids[i]->GetOutput();
		break;
	case Setting::smSubdivide:
		m_subdivideGrids[i]->Update();
		exGrid = m_subdivideGrids[i]->GetOutput();
		break;
	}
	return exGrid;
}

void Post2dWindowNodeVectorParticleGroupStructuredDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	Post2dWindowNodeVectorParticleGroupDataItem::doLoadFromProjectMainFile(node);
	m_stSettings.clear();
	QDomNode particlesNode = iRIC::getChildNode(node, "Particles");
	if (! particlesNode.isNull()) {
		QDomNodeList particles = particlesNode.childNodes();
		for (int i = 0; i < particles.length(); ++i) {
			Setting s;
			s.load(particles.at(i));
			m_stSettings.append(s);
		}
	}
	updateActorSettings();
}

void Post2dWindowNodeVectorParticleGroupStructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	Post2dWindowNodeVectorParticleGroupDataItem::doSaveToProjectMainFile(writer);
	writer.writeStartElement("Particles");
	for (int i = 0; i < m_stSettings.count(); ++i) {
		const Setting& s = m_stSettings[i];
		writer.writeStartElement("Particle");
		s.save(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();
}
