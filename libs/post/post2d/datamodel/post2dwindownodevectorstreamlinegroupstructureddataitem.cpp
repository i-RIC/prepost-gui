#include "post2dwindownodevectorstreamlinegroupstructureddataitem.h"
#include "post2dwindowstreamlinestructuredsettingdialog.h"
#include "post2dwindowzonedataitem.h"

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

Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::Setting() :
	CompositeContainer {&range, &spaceMode, &spaceSamplingRate, &spaceDivision, &color, &width},
	range {},
	spaceMode {"spaceMode", smNormal},
	spaceSamplingRate {"spaceSamplingRate", 2},
	spaceDivision {"spaceDivision", 2},
	color {"color"},
	width {"width", 1}
{
	QSettings settings;
	color = settings.value("post2d/particlecolor", QColor(Qt::black)).value<QColor>();
}

Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Post2dWindowNodeVectorStreamlineGroupStructuredDataItem(Post2dWindowDataItem* parent) :
	Post2dWindowNodeVectorStreamlineGroupDataItem {parent}
{
	setupDefaultValues();
}

Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::~Post2dWindowNodeVectorStreamlineGroupStructuredDataItem()
{}

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::setupDefaultValues()
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

	for (int i = 0; i < m_stSettings.count(); ++i) {
		Setting& s = m_stSettings[i];

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

		vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
		vtkProperty* prop = actor->GetProperty();
		prop->SetLighting(false);
		prop->SetColor(s.color);
		prop->SetLineWidth(s.width);

		renderer()->AddActor(actor);
		actorCollection()->AddItem(actor);

		vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
		actor->SetMapper(mapper);

		vtkSmartPointer<vtkStreamTracer> tracer = vtkSmartPointer<vtkStreamTracer>::New();
		setupStreamTracer(tracer);
		tracer->SetSourceData(getSource(i));
		tracer->SetInputData(getRegion());
		tracer->SetInputArrayToProcess(0, 0, 0, 0, iRIC::toStr(m_setting.currentSolution).c_str());

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
	dialog->setSettings(m_setting, m_stSettings);

	return dialog;
}

class Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const Post2dWindowNodeVectorStreamlineGroupDataItem::Setting& s, const QList<Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting>& settings, Post2dWindowNodeVectorStreamlineGroupStructuredDataItem* item) :
		QUndoCommand {QObject::tr("Update Streamline Setting")},
		m_newSetting {s},
		m_newStSettings {settings},
		m_oldEnabled {item->isEnabled()},
		m_oldSetting {item->m_setting},
		m_oldStSettings {item->m_stSettings},
		m_item {item}
	{}
	void redo() {
		m_item->setEnabled(true);
		m_item->m_setting = m_newSetting;
		m_item->setCurrentSolution(m_newSetting.currentSolution);
		m_item->m_stSettings = m_newStSettings;

		m_item->updateActorSettings();
	}
	void undo() {
		m_item->setEnabled(m_oldEnabled);
		m_item->m_setting = m_oldSetting;
		m_item->setCurrentSolution(m_oldSetting.currentSolution);
		m_item->m_stSettings = m_oldStSettings;

		m_item->updateActorSettings();
	}

private:
	Post2dWindowNodeVectorStreamlineGroupDataItem::Setting m_newSetting;
	QList<Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting> m_newStSettings;

	bool m_oldEnabled;
	Post2dWindowNodeVectorStreamlineGroupDataItem::Setting m_oldSetting;
	QList<Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting> m_oldStSettings;

	Post2dWindowNodeVectorStreamlineGroupStructuredDataItem* m_item;
};

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	Post2dWindowStreamlineStructuredSettingDialog* dialog = dynamic_cast<Post2dWindowStreamlineStructuredSettingDialog*>(propDialog);
	pushRenderCommand(new SetSettingCommand(dialog->setting(), dialog->stSettings(), this), this, true);
}

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	Post2dWindowNodeVectorStreamlineGroupDataItem::doLoadFromProjectMainFile(node);

	m_stSettings.clear();
	QDomNode streamlinesNode = iRIC::getChildNode(node, "Streamlines");
	if (! streamlinesNode.isNull()) {
		QDomNodeList streamlines = streamlinesNode.childNodes();
		for (int i = 0; i < streamlines.length(); ++i) {
			Setting s;
			s.load(streamlines.at(i));
			m_stSettings.append(s);
		}
	}
	updateActorSettings();
}

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	Post2dWindowNodeVectorStreamlineGroupDataItem::doSaveToProjectMainFile(writer);

	writer.writeStartElement("Streamlines");
	for (int i = 0; i < m_stSettings.count(); ++i) {
		const Setting& s = m_stSettings[i];
		writer.writeStartElement("Streamline");
		s.save(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();
}
