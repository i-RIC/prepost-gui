#include "post2dwindownodevectorstreamlinegroupstructureddataitem.h"
#include "post2dwindowstreamlinestructuredsettingdialog.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomElement>
#include <QDomNode>
#include <QSettings>
#include <QUndoCommand>

#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkStructuredGrid.h>

Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Post2dWindowNodeVectorStreamlineGroupStructuredDataItem(Post2dWindowDataItem* parent)
	: Post2dWindowNodeVectorStreamlineGroupDataItem(parent)
{
	setupDefaultValues();
}

Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::~Post2dWindowNodeVectorStreamlineGroupStructuredDataItem()
{

}

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::setupDefaultValues()
{
	m_settings.clear();

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	vtkStructuredGrid* grid = dynamic_cast<vtkStructuredGrid*>(cont->data());
	int dim[3];
	grid->GetDimensions(dim);

	Post2dWindowStructuredStreamlineSetSetting s;
	s.range.iMin = 0;
	s.range.iMax = 0;
	s.range.jMin = 0;
	s.range.jMax = dim[1] - 1;
	s.spaceMode = Post2dWindowStructuredStreamlineSetSetting::smNormal;
	s.spaceSamplingRate = 2;
	s.spaceDivision = 2;

	QSettings setting;
	s.color = setting.value("post2d/particlecolor", QColor(Qt::black)).value<QColor>();

	s.width = 1;
	m_settings.append(s);
}

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::setupActors()
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

	for (int i = 0; i < m_settings.count(); ++i) {
		Post2dWindowStructuredStreamlineSetSetting& setting = m_settings[i];

		vtkExtractGrid* ext = vtkExtractGrid::New();
		ext->SetInputData(zoneContainer->data());
		vtkSubdivideGrid* div = vtkSubdivideGrid::New();
		div->SetInputData(zoneContainer->data());

		ext->SetVOI(setting.range.iMin, setting.range.iMax, setting.range.jMin, setting.range.jMax, 0, 0);
		div->SetVOI(setting.range.iMin, setting.range.iMax, setting.range.jMin, setting.range.jMax, 0, 0);
		ext->SetSampleRate(1, 1, 1);
		if (setting.spaceMode == Post2dWindowStructuredStreamlineSetSetting::smSkip) {
			ext->SetSampleRate(setting.spaceSamplingRate, setting.spaceSamplingRate, 1);
		}
		div->SetDivideRate(setting.spaceDivision, setting.spaceDivision, 1);
		ext->Modified();
		div->Modified();

		m_extractGrids.append(ext);
		m_subdivideGrids.append(div);

		vtkActor* actor = vtkActor::New();
		vtkProperty* prop = actor->GetProperty();
		prop->SetLighting(false);
		prop->SetColor(setting.color.redF(), setting.color.greenF(), setting.color.blueF());
		prop->SetLineWidth(setting.width);

		renderer()->AddActor(actor);
		actorCollection()->AddItem(actor);

		vtkDataSetMapper* mapper = vtkDataSetMapper::New();
		actor->SetMapper(mapper);

		vtkStreamTracer* tracer = vtkStreamTracer::New();
		setupStreamTracer(tracer);
		tracer->SetSourceData(getSource(i));
		tracer->SetInputData(getRegion());
		tracer->SetInputArrayToProcess(0, 0, 0, 0, iRIC::toStr(m_currentSolution).c_str());

		mapper->SetInputConnection(tracer->GetOutputPort());

		m_streamlineActors.append(actor);
		m_streamlineMappers.append(mapper);
		m_streamTracers.append(tracer);
	}
}

vtkPointSet* Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::getSource(int i)
{
	vtkStructuredGrid* exGrid = nullptr;
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return nullptr;}
	Post2dWindowStructuredStreamlineSetSetting& setting = m_settings[i];
	switch (setting.spaceMode) {
	case Post2dWindowStructuredStreamlineSetSetting::smNormal:
	case Post2dWindowStructuredStreamlineSetSetting::smSkip:
		m_extractGrids[i]->Update();
		exGrid = m_extractGrids[i]->GetOutput();
		break;
	case Post2dWindowStructuredStreamlineSetSetting::smSubdivide:
		m_subdivideGrids[i]->Update();
		exGrid = m_subdivideGrids[i]->GetOutput();
		break;
	}
	return exGrid;
}

QDialog* Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::propertyDialog(QWidget* p)
{
	Post2dWindowStreamlineStructuredSettingDialog* dialog = new Post2dWindowStreamlineStructuredSettingDialog(p);
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {
		delete dialog;
		return nullptr;
	}
	dialog->setZoneData(cont);
	dialog->setActiveAvailable(cont->IBCExists());

	dialog->setSolution(m_currentSolution);
	dialog->setSettings(m_settings);
	dialog->setRegionMode(m_regionMode);

	return dialog;
}

class Post2dWindowStreamlineStructuredSetProperty : public QUndoCommand
{
public:
	Post2dWindowStreamlineStructuredSetProperty(const QString& sol, const QList<Post2dWindowStructuredStreamlineSetSetting>& settings, StructuredGridRegion::RegionMode rm, Post2dWindowNodeVectorStreamlineGroupStructuredDataItem* item)
		: QUndoCommand(QObject::tr("Update Streamline Setting")) {
		m_newEnabled = true;
		m_newSolution = sol;
		m_newSettings = settings;
		m_newRegionMode = rm;

		m_oldEnabled = item->isEnabled();
		m_oldSolution = item->m_currentSolution;
		m_oldSettings = item->m_settings;
		m_oldRegionMode = item->m_regionMode;

		m_item = item;
	}
	void undo() {
		m_item->setIsCommandExecuting(true);
		m_item->setEnabled(m_oldEnabled);
		m_item->setCurrentSolution(m_oldSolution);
		m_item->m_settings = m_oldSettings;
		m_item->m_regionMode = m_oldRegionMode;

		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
	void redo() {
		m_item->setIsCommandExecuting(true);
		m_item->setEnabled(m_newEnabled);
		m_item->setCurrentSolution(m_newSolution);
		m_item->m_settings = m_newSettings;
		m_item->m_regionMode = m_newRegionMode;

		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
private:
	bool m_oldEnabled;
	QString m_oldSolution;
	QList<Post2dWindowStructuredStreamlineSetSetting> m_oldSettings;
	StructuredGridRegion::RegionMode m_oldRegionMode;

	bool m_newEnabled;
	QString m_newSolution;
	QList<Post2dWindowStructuredStreamlineSetSetting> m_newSettings;
	StructuredGridRegion::RegionMode m_newRegionMode;

	Post2dWindowNodeVectorStreamlineGroupStructuredDataItem* m_item;
};

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	Post2dWindowStreamlineStructuredSettingDialog* dialog = dynamic_cast<Post2dWindowStreamlineStructuredSettingDialog*>(propDialog);
	iRICUndoStack::instance().push(new Post2dWindowStreamlineStructuredSetProperty(dialog->solution(), dialog->settings(), dialog->regionMode(), this));
}


void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	Post2dWindowNodeVectorStreamlineGroupDataItem::doLoadFromProjectMainFile(node);

	m_settings.clear();
	QDomNode streamlinesNode = iRIC::getChildNode(node, "Streamlines");
	if (! streamlinesNode.isNull()) {
		QDomNodeList streamlines = streamlinesNode.childNodes();
		for (int i = 0; i < streamlines.length(); ++i) {
			QDomElement elem = streamlines.at(i).toElement();
			Post2dWindowStructuredStreamlineSetSetting s;
			s.range.iMin = elem.attribute("regionIMin").toInt();
			s.range.iMax = elem.attribute("regionIMax").toInt();
			s.range.jMin = elem.attribute("regionJMin").toInt();
			s.range.jMax = elem.attribute("regionJMax").toInt();
			s.spaceMode = static_cast<Post2dWindowStructuredStreamlineSetSetting::SpaceMode>(elem.attribute("spaceMode").toInt());
			s.spaceSamplingRate = elem.attribute("spaceSamplingRate").toInt();
			s.spaceDivision = elem.attribute("spaceDivision").toInt();
			s.color = loadColorAttribute("color",elem, s.color);
			s.width = elem.attribute("width").toInt();
			m_settings.append(s);
		}
	}
	updateActorSettings();
}

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	Post2dWindowNodeVectorStreamlineGroupDataItem::doSaveToProjectMainFile(writer);

	writer.writeStartElement("Streamlines");
	for (int i = 0; i < m_settings.count(); ++i) {
		Post2dWindowStructuredStreamlineSetSetting& setting = m_settings[i];
		writer.writeStartElement("Streamline");
		writer.writeAttribute("regionIMin", QString::number(setting.range.iMin));
		writer.writeAttribute("regionIMax", QString::number(setting.range.iMax));
		writer.writeAttribute("regionJMin", QString::number(setting.range.jMin));
		writer.writeAttribute("regionJMax", QString::number(setting.range.jMax));
		writer.writeAttribute("spaceMode", QString::number(static_cast<int>(setting.spaceMode)));
		writer.writeAttribute("spaceSamplingRate", QString::number(setting.spaceSamplingRate));
		writer.writeAttribute("spaceDivision", QString::number(setting.spaceDivision));
		writeColorAttribute("color",setting.color, writer);
		writer.writeAttribute("width", QString::number(setting.width));
		writer.writeEndElement();
	}
	writer.writeEndElement();
}
