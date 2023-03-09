#include "../post3dwindowgraphicsview.h"
#include "post3dwindownodevectorstreamlinegroupstructureddataitem.h"
#include "post3dwindowstreamlinestructuredsettingdialog.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QSettings>
#include <QUndoCommand>
#include <QXmlStreamWriter>

#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkStructuredGrid.h>

Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::Post3dWindowNodeVectorStreamlineGroupStructuredDataItem(Post3dWindowDataItem* parent)
	: Post3dWindowNodeVectorStreamlineGroupDataItem(parent)
{
	setupDefaultValues();
}

Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::~Post3dWindowNodeVectorStreamlineGroupStructuredDataItem()
{

}

void Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::setupDefaultValues()
{
	m_settings.clear();

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	auto grid = vtkStructuredGrid::SafeDownCast(cont->data()->data());
	int dim[3];
	grid->GetDimensions(dim);

	Post3dWindowStructuredStreamlineSetSetting s;
	s.range.iMin = 0;
	s.range.iMax = 0;
	s.range.jMin = 0;
	s.range.jMax = dim[1] - 1;
	s.range.kMin = 0;
	s.range.kMax = dim[2] - 1;
	s.spaceMode = Post3dWindowStructuredStreamlineSetSetting::smNormal;
	s.spaceSamplingRate = 2;
	s.spaceDivision = 2;

	QSettings setting;
	s.color = setting.value("post3d/particlecolor", QColor(Qt::black)).value<QColor>();

	s.width = 1;
	m_settings.append(s);
}

void Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::setupActors()
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
		Post3dWindowStructuredStreamlineSetSetting& setting = m_settings[i];

		vtkExtractGrid* ext = vtkExtractGrid::New();
		ext->SetInputData(zoneContainer->data()->data());
		vtkSubdivideGrid* div = vtkSubdivideGrid::New();
		div->SetInputData(zoneContainer->data()->data());

		ext->SetVOI(setting.range.iMin, setting.range.iMax, setting.range.jMin, setting.range.jMax, setting.range.kMin, setting.range.kMax);
		div->SetVOI(setting.range.iMin, setting.range.iMax, setting.range.jMin, setting.range.jMax, setting.range.kMin, setting.range.kMax);
		ext->SetSampleRate(1, 1, 1);
		if (setting.spaceMode == Post3dWindowStructuredStreamlineSetSetting::smSkip) {
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
		auto v = dataModel()->graphicsView();
		prop->SetLineWidth(setting.width * v->devicePixelRatioF());

		renderer()->AddActor(actor);
		actorCollection()->AddItem(actor);

		vtkDataSetMapper* mapper = vtkDataSetMapper::New();
		mapper->ScalarVisibilityOff();
		actor->SetMapper(mapper);

		vtkStreamTracer* tracer = vtkStreamTracer::New();
		setupStreamTracer(tracer);
		tracer->SetSourceData(getSource(i));
		tracer->SetInputData(getRegion());
		tracer->SetInputArrayToProcess(0, 0, 0, 0, m_target.c_str());

		mapper->SetInputConnection(tracer->GetOutputPort());

		m_streamlineActors.append(actor);
		m_streamlineMappers.append(mapper);
		m_streamTracers.append(tracer);
	}
}

vtkPointSet* Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::getSource(int i)
{
	vtkStructuredGrid* exGrid = nullptr;
	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return nullptr;}
	Post3dWindowStructuredStreamlineSetSetting& setting = m_settings[i];
	switch (setting.spaceMode) {
	case Post3dWindowStructuredStreamlineSetSetting::smNormal:
	case Post3dWindowStructuredStreamlineSetSetting::smSkip:
		m_extractGrids[i]->Update();
		exGrid = m_extractGrids[i]->GetOutput();
		break;
	case Post3dWindowStructuredStreamlineSetSetting::smSubdivide:
		m_subdivideGrids[i]->Update();
		exGrid = m_subdivideGrids[i]->GetOutput();
		break;
	}
	return exGrid;
}

QDialog* Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::propertyDialog(QWidget* p)
{
	Post3dWindowStreamlineStructuredSettingDialog* dialog = new Post3dWindowStreamlineStructuredSettingDialog(p);
	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {
		delete dialog;
		return nullptr;
	}
	dialog->setZoneData(cont);
	dialog->setActiveAvailable(cont->IBCExists());

	dialog->setSolution(m_target);
	dialog->setSettings(m_settings);
	dialog->setRegionMode(m_regionMode);

	return dialog;
}

class Post3dWindowStreamlineStructuredSetProperty : public QUndoCommand
{
public:
	Post3dWindowStreamlineStructuredSetProperty(const std::string& sol, const QList<Post3dWindowStructuredStreamlineSetSetting>& settings, StructuredGridRegion::RegionMode rm, Post3dWindowNodeVectorStreamlineGroupStructuredDataItem* item) :
		QUndoCommand(QObject::tr("Update Streamline Setting"))
	{
		m_newEnabled = true;
		m_newSolution = sol;
		m_newSettings = settings;
		m_newRegionMode = rm;

		m_oldEnabled = item->isEnabled();
		m_oldSolution = item->m_target;
		m_oldSettings = item->m_settings;
		m_oldRegionMode = item->m_regionMode;

		m_item = item;
	}
	void undo() {
		m_item->setIsCommandExecuting(true);
		m_item->setEnabled(m_oldEnabled);
		m_item->setTarget(m_oldSolution);
		m_item->m_settings = m_oldSettings;
		m_item->m_regionMode = m_oldRegionMode;

		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
	void redo() {
		m_item->setIsCommandExecuting(true);
		m_item->setEnabled(m_newEnabled);
		m_item->setTarget(m_newSolution);
		m_item->m_settings = m_newSettings;
		m_item->m_regionMode = m_newRegionMode;

		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
private:
	bool m_oldEnabled;
	std::string m_oldSolution;
	QList<Post3dWindowStructuredStreamlineSetSetting> m_oldSettings;
	StructuredGridRegion::RegionMode m_oldRegionMode;

	bool m_newEnabled;
	std::string m_newSolution;
	QList<Post3dWindowStructuredStreamlineSetSetting> m_newSettings;
	StructuredGridRegion::RegionMode m_newRegionMode;

	Post3dWindowNodeVectorStreamlineGroupStructuredDataItem* m_item;
};

void Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	Post3dWindowStreamlineStructuredSettingDialog* dialog = dynamic_cast<Post3dWindowStreamlineStructuredSettingDialog*>(propDialog);
	iRICUndoStack::instance().push(new Post3dWindowStreamlineStructuredSetProperty(dialog->solution(), dialog->settings(), dialog->regionMode(), this));
}

void Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	Post3dWindowNodeVectorStreamlineGroupDataItem::doLoadFromProjectMainFile(node);

	m_settings.clear();
	QDomNode streamlinesNode = iRIC::getChildNode(node, "Streamlines");
	if (! streamlinesNode.isNull()) {
		QDomNodeList streamlines = streamlinesNode.childNodes();
		for (int i = 0; i < streamlines.length(); ++i) {
			QDomElement elem = streamlines.at(i).toElement();
			Post3dWindowStructuredStreamlineSetSetting s;
			s.range.iMin = elem.attribute("regionIMin").toInt();
			s.range.iMax = elem.attribute("regionIMax").toInt();
			s.range.jMin = elem.attribute("regionJMin").toInt();
			s.range.jMax = elem.attribute("regionJMax").toInt();
			s.range.kMin = elem.attribute("regionKMin").toInt();
			s.range.kMax = elem.attribute("regionKMax").toInt();
			s.spaceMode = static_cast<Post3dWindowStructuredStreamlineSetSetting::SpaceMode>(elem.attribute("spaceMode").toInt());
			s.spaceSamplingRate = elem.attribute("spaceSamplingRate").toInt();
			s.spaceDivision = elem.attribute("spaceDivision").toInt();
			s.color = loadColorAttribute("color", elem, s.color);
			s.width = elem.attribute("width").toInt();
			m_settings.append(s);
		}
	}
	updateActorSettings();
}

void Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	Post3dWindowNodeVectorStreamlineGroupDataItem::doSaveToProjectMainFile(writer);

	writer.writeStartElement("Streamlines");
	for (int i = 0; i < m_settings.count(); ++i) {
		Post3dWindowStructuredStreamlineSetSetting& setting = m_settings[i];
		writer.writeStartElement("Streamline");
		writer.writeAttribute("regionIMin", QString::number(setting.range.iMin));
		writer.writeAttribute("regionIMax", QString::number(setting.range.iMax));
		writer.writeAttribute("regionJMin", QString::number(setting.range.jMin));
		writer.writeAttribute("regionJMax", QString::number(setting.range.jMax));
		writer.writeAttribute("regionKMin", QString::number(setting.range.kMin));
		writer.writeAttribute("regionKMax", QString::number(setting.range.kMax));
		writer.writeAttribute("spaceMode", QString::number(static_cast<int>(setting.spaceMode)));
		writer.writeAttribute("spaceSamplingRate", QString::number(setting.spaceSamplingRate));
		writer.writeAttribute("spaceDivision", QString::number(setting.spaceDivision));
		writeColorAttribute("color",setting.color, writer);
		writer.writeAttribute("width", QString::number(setting.width));
		writer.writeEndElement();
	}
	writer.writeEndElement();
}
