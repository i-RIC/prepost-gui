#include "post2dwindownodevectorparticlegroupstructureddataitem.h"
#include "post2dwindowparticlestructuredsettingdialog.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/iricundostack.h>
#include <misc/xmlsupport.h>

#include <QDomElement>
#include <QSettings>

#include <vtkProperty.h>
#include <vtkRenderer.h>

QDialog* Post2dWindowNodeVectorParticleGroupStructuredDataItem::propertyDialog(QWidget* p)
{
	Post2dWindowParticleStructuredSettingDialog* dialog = new Post2dWindowParticleStructuredSettingDialog(p);
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == 0 || cont->data() == 0){
		delete dialog;
		return 0;
	}
	dialog->setZoneData(cont);
	dialog->setActiveAvailable(cont->IBCExists());

	dialog->setSolution(m_currentSolution);
	dialog->setTimeMode(m_timeMode);
	dialog->setTimeSamplingRate(m_timeSamplingRate);
	dialog->setTimeDivision(m_timeDivision);
	dialog->setSettings(m_settings);
	dialog->setRegionMode(m_regionMode);

	return dialog;
}

class Post2dWindowParticleStructuredSetProperty : public QUndoCommand
{
public:
	Post2dWindowParticleStructuredSetProperty(const QString& sol, Post2dWindowNodeVectorParticleGroupDataItem::TimeMode tm, int tsr, int tdiv, const QList<Post2dWindowStructuredParticleSetSetting>& settings, StructuredGridRegion::RegionMode rm, Post2dWindowNodeVectorParticleGroupStructuredDataItem* item)
		: QUndoCommand(QObject::tr("Update Particle Setting"))
	{
		m_newEnabled = true;
		m_newSolution = sol;
		m_newTimeMode = tm;
		m_newTimeSamplingRate = tsr;
		m_newTimeDivision = tdiv;
		m_newSettings = settings;
		m_newRegionMode = rm;

		m_oldEnabled = item->isEnabled();
		m_oldSolution = item->m_currentSolution;
		m_oldTimeMode = item->m_timeMode;
		m_oldTimeSamplingRate = item->m_timeSamplingRate;
		m_oldTimeDivision = item->m_timeDivision;
		m_oldSettings = item->m_settings;
		m_oldRegionMode = item->m_regionMode;

		m_item = item;
	}
	void undo()
	{
		m_item->setIsCommandExecuting(true);
		m_item->setEnabled(m_oldEnabled);
		m_item->setCurrentSolution(m_oldSolution);
		m_item->m_timeMode = m_oldTimeMode;
		m_item->m_timeSamplingRate = m_oldTimeSamplingRate;
		m_item->m_timeDivision = m_oldTimeDivision;
		m_item->m_settings = m_oldSettings;
		m_item->m_regionMode = m_oldRegionMode;

		m_item->updateActorSettings();
		m_item->setIsCommandExecuting(false);
	}
	void redo()
	{
		m_item->setIsCommandExecuting(true);
		m_item->setEnabled(m_newEnabled);
		m_item->setCurrentSolution(m_newSolution);
		m_item->m_timeMode = m_newTimeMode;
		m_item->m_timeSamplingRate = m_newTimeSamplingRate;
		m_item->m_timeDivision = m_newTimeDivision;
		m_item->m_settings = m_newSettings;
		m_item->m_regionMode = m_newRegionMode;

		m_item->updateActorSettings();
		m_item->setIsCommandExecuting(false);
	}

private:
	bool m_oldEnabled;
	QString m_oldSolution;
	Post2dWindowNodeVectorParticleGroupDataItem::TimeMode m_oldTimeMode;
	int m_oldTimeSamplingRate;
	int m_oldTimeDivision;
	QList<Post2dWindowStructuredParticleSetSetting> m_oldSettings;
	StructuredGridRegion::RegionMode m_oldRegionMode;

	bool m_newEnabled;
	QString m_newSolution;
	Post2dWindowNodeVectorParticleGroupDataItem::TimeMode m_newTimeMode;
	int m_newTimeSamplingRate;
	int m_newTimeDivision;
	QList<Post2dWindowStructuredParticleSetSetting> m_newSettings;
	StructuredGridRegion::RegionMode m_newRegionMode;

	Post2dWindowNodeVectorParticleGroupStructuredDataItem* m_item;
};

void Post2dWindowNodeVectorParticleGroupStructuredDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	Post2dWindowParticleStructuredSettingDialog* dialog = dynamic_cast<Post2dWindowParticleStructuredSettingDialog*>(propDialog);
	iRICUndoStack::instance().push(new Post2dWindowParticleStructuredSetProperty(dialog->solution(), dialog->timeMode(), dialog->timeSamplingRate(), dialog->timeDivision(), dialog->settings(), dialog->regionMode(), this));
}

void Post2dWindowNodeVectorParticleGroupStructuredDataItem::setDefaultValues()
{
	m_settings.clear();

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	vtkStructuredGrid* grid = dynamic_cast<vtkStructuredGrid*>(cont->data());
	int dim[3];
	grid->GetDimensions(dim);

	Post2dWindowStructuredParticleSetSetting s;
	s.range.iMin = 0;
	s.range.iMax = 0;
	s.range.jMin = 0;
	s.range.jMax = dim[1] - 1;
	s.spaceMode = Post2dWindowStructuredParticleSetSetting::smNormal;
	s.spaceSamplingRate = 2;
	s.spaceDivision = 2;

	QSettings setting;
	s.color = setting.value("post2d/particlecolor", QColor(Qt::black)).value<QColor>();

	s.size = 2;
	m_settings.append(s);
}

void Post2dWindowNodeVectorParticleGroupStructuredDataItem::setupParticleSources()
{
	PostZoneDataContainer* zoneContainer = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	for (int i = 0; i < m_extractGrids.count(); ++i){
		m_extractGrids[i]->Delete();
	}
	m_extractGrids.clear();
	for (int i = 0; i < m_subdivideGrids.count(); ++i){
		m_subdivideGrids[i]->Delete();
	}
	m_subdivideGrids.clear();
	for (int i = 0; i < m_settings.count(); ++i){
		Post2dWindowStructuredParticleSetSetting& setting = m_settings[i];
		vtkExtractGrid* ext = vtkExtractGrid::New();
		ext->SetInputData(zoneContainer->data());
		vtkSubdivideGrid* div = vtkSubdivideGrid::New();
		div->SetInputData(zoneContainer->data());

		ext->SetVOI(setting.range.iMin, setting.range.iMax, setting.range.jMin, setting.range.jMax, 0, 0);
		div->SetVOI(setting.range.iMin, setting.range.iMax, setting.range.jMin, setting.range.jMax, 0, 0);
		ext->SetSampleRate(1, 1, 1);
		if (setting.spaceMode == Post2dWindowStructuredParticleSetSetting::smSkip){
			ext->SetSampleRate(setting.spaceSamplingRate, setting.spaceSamplingRate, 1);
		}
		div->SetDivideRate(setting.spaceDivision, setting.spaceDivision, 1);
		ext->Modified();
		div->Modified();

		m_extractGrids.append(ext);
		m_subdivideGrids.append(div);
	}
}

void Post2dWindowNodeVectorParticleGroupStructuredDataItem::setupActors()
{
	for (int i = 0; i < m_settings.count(); ++i){
		Post2dWindowStructuredParticleSetSetting& setting = m_settings[i];
		vtkActor* actor = vtkActor::New();
		vtkProperty* prop = actor->GetProperty();
		prop->SetLighting(false);
		prop->SetColor(setting.color.redF(), setting.color.greenF(), setting.color.blueF());
		prop->SetPointSize(setting.size);

		renderer()->AddActor(actor);
		actorCollection()->AddItem(actor);

		vtkDataSetMapper* mapper = vtkDataSetMapper::New();
		actor->SetMapper(mapper);

		m_particleActors.append(actor);
		m_particleMappers.append(mapper);
	}
}


vtkPointSet* Post2dWindowNodeVectorParticleGroupStructuredDataItem::newParticles(int i)
{
	vtkStructuredGrid* exGrid = 0;
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == 0 || cont->data() == 0){return 0;}
	Post2dWindowStructuredParticleSetSetting& setting = m_settings[i];
	switch (setting.spaceMode){
	case Post2dWindowStructuredParticleSetSetting::smNormal:
	case Post2dWindowStructuredParticleSetSetting::smSkip:
		m_extractGrids[i]->Update();
		exGrid = m_extractGrids[i]->GetOutput();
		break;
	case Post2dWindowStructuredParticleSetSetting::smSubdivide:
		m_subdivideGrids[i]->Update();
		exGrid = m_subdivideGrids[i]->GetOutput();
		break;
	}
	return exGrid;
}

void Post2dWindowNodeVectorParticleGroupStructuredDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	Post2dWindowNodeVectorParticleGroupDataItem::doLoadFromProjectMainFile(node);
	m_settings.clear();
	QDomNode particlesNode = iRIC::getChildNode(node, "Particles");
	if (! particlesNode.isNull()){
		QDomNodeList particles = particlesNode.childNodes();
		for (int i = 0; i < particles.length(); ++i){
			QDomElement elem = particles.at(i).toElement();
			Post2dWindowStructuredParticleSetSetting s;
			s.range.iMin = elem.attribute("regionIMin").toInt();
			s.range.iMax = elem.attribute("regionIMax").toInt();
			s.range.jMin = elem.attribute("regionJMin").toInt();
			s.range.jMax = elem.attribute("regionJMax").toInt();
			s.spaceMode = static_cast<Post2dWindowStructuredParticleSetSetting::SpaceMode>(elem.attribute("spaceMode").toInt());
			s.spaceSamplingRate = elem.attribute("spaceSamplingRate").toInt();
			s.spaceDivision = elem.attribute("spaceDivision").toInt();
			s.color = loadColorAttribute("color", elem, s.color);
			s.size = elem.attribute("size").toInt();
			m_settings.append(s);
		}
	}
	updateActorSettings();
}

void Post2dWindowNodeVectorParticleGroupStructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	Post2dWindowNodeVectorParticleGroupDataItem::doSaveToProjectMainFile(writer);
	writer.writeStartElement("Particles");
	for (int i = 0; i < m_settings.count(); ++i){
		Post2dWindowStructuredParticleSetSetting& setting = m_settings[i];
		writer.writeStartElement("Particle");
		writer.writeAttribute("regionIMin", QString::number(setting.range.iMin));
		writer.writeAttribute("regionIMax", QString::number(setting.range.iMax));
		writer.writeAttribute("regionJMin", QString::number(setting.range.jMin));
		writer.writeAttribute("regionJMax", QString::number(setting.range.jMax));
		writer.writeAttribute("spaceMode", QString::number(static_cast<int>(setting.spaceMode)));
		writer.writeAttribute("spaceSamplingRate", QString::number(setting.spaceSamplingRate));
		writer.writeAttribute("spaceDivision", QString::number(setting.spaceDivision));
		writeColorAttribute("color", setting.color, writer);
		writer.writeAttribute("size", QString::number(setting.size));
		writer.writeEndElement();
	}
	writer.writeEndElement();
}
