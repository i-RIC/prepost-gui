#include "post3dwindownodevectorparticlegroupstructureddataitem.h"
#include "post3dwindowparticlestructuredsettingdialog.h"
#include "post3dwindowzonedataitem.h"
#include "private/post3dwindownodevectorparticlegroupstructureddataitem_setsettingcommand.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/project/projectdata.h>
#include <misc/iricundostack.h>
#include <misc/xmlsupport.h>

#include <QDomElement>
#include <QDomNode>
#include <QSettings>
#include <QXmlStreamWriter>

#include <vtkProperty.h>
#include <vtkRenderer.h>

Post3dWindowNodeVectorParticleGroupStructuredDataItem::Post3dWindowNodeVectorParticleGroupStructuredDataItem(Post3dWindowDataItem* parent) :
	Post3dWindowNodeVectorParticleGroupDataItem(parent)
{
	setDefaultValues();
}

QDialog* Post3dWindowNodeVectorParticleGroupStructuredDataItem::propertyDialog(QWidget* p)
{
	Post3dWindowParticleStructuredSettingDialog* dialog = new Post3dWindowParticleStructuredSettingDialog(p);
	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == 0 || cont->data() == 0) {
		delete dialog;
		return 0;
	}
	dialog->setProjectMainFile(projectData()->mainfile());
	dialog->setZoneData(cont);
	dialog->setActiveAvailable(cont->IBCExists());

	dialog->setSetting(m_setting);
	dialog->setSettings(m_settings);

	return dialog;
}

void Post3dWindowNodeVectorParticleGroupStructuredDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	Post3dWindowParticleStructuredSettingDialog* dialog = dynamic_cast<Post3dWindowParticleStructuredSettingDialog*>(propDialog);
	iRICUndoStack::instance().push(new SetSettingCommand(dialog->setting(), dialog->settings(), this));
}

void Post3dWindowNodeVectorParticleGroupStructuredDataItem::setDefaultValues()
{
	m_settings.clear();

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	vtkStructuredGrid* grid = dynamic_cast<vtkStructuredGrid*>(cont->data());
	int dim[3];
	grid->GetDimensions(dim);

	Post3dWindowStructuredParticleSetSetting s;
	s.range.iMin = 0;
	s.range.iMax = 0;
	s.range.jMin = 0;
	s.range.jMax = dim[1] - 1;
	s.range.kMin = 0;
	s.range.kMax = dim[2] - 1;
	s.spaceMode = Post3dWindowStructuredParticleSetSetting::smNormal;
	s.spaceSamplingRate = 2;
	s.spaceDivision = 2;

	QSettings setting;
	s.color = setting.value("post3d/particlecolor", QColor(Qt::black)).value<QColor>();

	s.size = 2;
	m_settings.append(s);
}

void Post3dWindowNodeVectorParticleGroupStructuredDataItem::setupParticleSources()
{
	PostZoneDataContainer* zoneContainer = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	for (int i = 0; i < m_extractGrids.count(); ++i) {
		m_extractGrids[i]->Delete();
	}
	m_extractGrids.clear();
	for (int i = 0; i < m_subdivideGrids.count(); ++i) {
		m_subdivideGrids[i]->Delete();
	}
	m_subdivideGrids.clear();
	for (int i = 0; i < m_settings.count(); ++i) {
		Post3dWindowStructuredParticleSetSetting& setting = m_settings[i];
		vtkExtractGrid* ext = vtkExtractGrid::New();
		ext->SetInputData(zoneContainer->data());
		vtkSubdivideGrid* div = vtkSubdivideGrid::New();
		div->SetInputData(zoneContainer->data());

		ext->SetVOI(setting.range.iMin, setting.range.iMax, setting.range.jMin, setting.range.jMax, setting.range.kMin, setting.range.kMax);
		div->SetVOI(setting.range.iMin, setting.range.iMax, setting.range.jMin, setting.range.jMax, setting.range.kMin, setting.range.kMax);
		ext->SetSampleRate(1, 1, 1);
		if (setting.spaceMode == Post3dWindowStructuredParticleSetSetting::smSkip) {
			ext->SetSampleRate(setting.spaceSamplingRate, setting.spaceSamplingRate, 1);
		}
		div->SetDivideRate(setting.spaceDivision, setting.spaceDivision, 1);
		ext->Modified();
		div->Modified();

		m_extractGrids.append(ext);
		m_subdivideGrids.append(div);
	}
}

void Post3dWindowNodeVectorParticleGroupStructuredDataItem::setupActors()
{
	for (int i = 0; i < m_settings.count(); ++i) {
		Post3dWindowStructuredParticleSetSetting& setting = m_settings[i];
		vtkActor* actor = vtkActor::New();
		vtkProperty* prop = actor->GetProperty();
		prop->SetLighting(false);
		prop->SetColor(setting.color.redF(), setting.color.greenF(), setting.color.blueF());
		prop->SetPointSize(setting.size);

		renderer()->AddActor(actor);
		actorCollection()->AddItem(actor);

		vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
		actor->SetMapper(mapper);

		m_particleActors.push_back(actor);
		m_particleMappers.push_back(mapper);
	}
}

vtkPointSet* Post3dWindowNodeVectorParticleGroupStructuredDataItem::newParticles(int i)
{
	vtkStructuredGrid* exGrid = nullptr;
	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return nullptr;}
	Post3dWindowStructuredParticleSetSetting& setting = m_settings[i];
	switch (setting.spaceMode) {
	case Post3dWindowStructuredParticleSetSetting::smNormal:
	case Post3dWindowStructuredParticleSetSetting::smSkip:
		m_extractGrids[i]->Update();
		exGrid = m_extractGrids[i]->GetOutput();
		break;
	case Post3dWindowStructuredParticleSetSetting::smSubdivide:
		m_subdivideGrids[i]->Update();
		exGrid = m_subdivideGrids[i]->GetOutput();
		break;
	}
	return exGrid;
}

void Post3dWindowNodeVectorParticleGroupStructuredDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	Post3dWindowNodeVectorParticleGroupDataItem::doLoadFromProjectMainFile(node);

	m_settings.clear();
	QDomNode psNode = iRIC::getChildNode(node, "ParticleSettings");
	if (! psNode.isNull()) {
		QDomNodeList particlesettings = psNode.childNodes();
		for (int i = 0; i < particlesettings.length(); ++i) {
			QDomElement elem = particlesettings.at(i).toElement();
			Post3dWindowStructuredParticleSetSetting s;
			s.range.iMin = elem.attribute("regionIMin").toInt();
			s.range.iMax = elem.attribute("regionIMax").toInt();
			s.range.jMin = elem.attribute("regionJMin").toInt();
			s.range.jMax = elem.attribute("regionJMax").toInt();
			s.range.kMin = elem.attribute("regionKMin").toInt();
			s.range.kMax = elem.attribute("regionKMax").toInt();
			s.spaceMode = static_cast<Post3dWindowStructuredParticleSetSetting::SpaceMode>(elem.attribute("spaceMode").toInt());
			s.spaceSamplingRate = elem.attribute("spaceSamplingRate").toInt();
			s.spaceDivision = elem.attribute("spaceDivision").toInt();
			s.color = loadColorAttribute("color", elem, s.color);
			s.size = elem.attribute("size").toInt();
			m_settings.append(s);
		}
	}

	updateActorSettings();
}

void Post3dWindowNodeVectorParticleGroupStructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	Post3dWindowNodeVectorParticleGroupDataItem::doSaveToProjectMainFile(writer);

	writer.writeStartElement("ParticleSettings");
	for (int i = 0; i < m_settings.count(); ++i) {
		Post3dWindowStructuredParticleSetSetting& setting = m_settings[i];
		writer.writeStartElement("Particle");
		writer.writeAttribute("regionIMin", QString::number(setting.range.iMin));
		writer.writeAttribute("regionIMax", QString::number(setting.range.iMax));
		writer.writeAttribute("regionJMin", QString::number(setting.range.jMin));
		writer.writeAttribute("regionJMax", QString::number(setting.range.jMax));
		writer.writeAttribute("regionKMin", QString::number(setting.range.jMin));
		writer.writeAttribute("regionKMax", QString::number(setting.range.jMax));
		writer.writeAttribute("spaceMode", QString::number(static_cast<int>(setting.spaceMode)));
		writer.writeAttribute("spaceSamplingRate", QString::number(setting.spaceSamplingRate));
		writer.writeAttribute("spaceDivision", QString::number(setting.spaceDivision));
		writeColorAttribute("color", setting.color, writer);
		writer.writeAttribute("size", QString::number(setting.size));
		writer.writeEndElement();
	}
	writer.writeEndElement();
}
