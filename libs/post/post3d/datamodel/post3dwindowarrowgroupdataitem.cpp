#include "../post3dwindowgraphicsview.h"
#include "../post3dwindowgraphicsview.h"
#include "post3dwindowarrowgroupdataitem.h"
#include "post3dwindowarrowgroupsettingdialog.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowzonedataitem.h"
#include "private/post3dwindowarrowgroupdataitem_setsettingcommand.h"
#include "private/post3dwindowarrowgroupdataitem_updateonredocommand.h"
#include "private/post3dwindowarrowgroupdataitem_updateonundocommand.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/datamodel/graphicswindowdrawcommands.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QXmlStreamWriter>
#include <QMainWindow>
#include <QSet>
#include <QSettings>
#include <QStandardItem>
#include <QUndoCommand>
#include <QVector2D>

#include <vtkActor2DCollection.h>
#include <vtkAlgorithmOutput.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkIdentityTransform.h>
#include <vtkPointData.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>

#include <cmath>

Post3dWindowArrowGroupDataItem::Post3dWindowArrowGroupDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem {tr("Arrow"), QIcon(":/libs/guibase/images/iconFolder.png"), p}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();

	auto vectorNames = vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(cont->data()->GetPointData());
	if (vectorNames.size() > 0) {
		m_setting.target = vectorNames[0].c_str();
	}
	auto scalarNames = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cont->data()->GetPointData());
	if (scalarNames.size() > 0) {
		m_setting.colorTarget = scalarNames[0].c_str();
	}

	setupActors();
}

void Post3dWindowArrowGroupDataItem::setSetting(const ArrowSettingContainer& setting)
{
	iRICUndoStack::instance().push(new SetSettingCommand(setting, this));
}

QDialog* Post3dWindowArrowGroupDataItem::propertyDialog(QWidget* p)
{
	Post3dWindowArrowGroupSettingDialog* dialog = new Post3dWindowArrowGroupSettingDialog(p);
	dialog->setEnabled(true);
	PostZoneDataContainer* zoneData = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (zoneData == nullptr || zoneData->data() == nullptr) {
		delete dialog;
		return nullptr;
	}
	dialog->setZoneData(zoneData);
	dialog->setFaceMap(faceMap());
	dialog->setSetting(m_setting);

	return dialog;
}

void Post3dWindowArrowGroupDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	Post3dWindowArrowGroupSettingDialog* dialog = dynamic_cast<Post3dWindowArrowGroupSettingDialog*>(propDialog);
	iRICUndoStack& stack = iRICUndoStack::instance();
	stack.beginMacro(tr("Arrow Setting"));
	stack.push(new UpdateOnUndoCommand(this));
	setSetting(dialog->setting());
	setFaceMap(dialog->faceMap());
	stack.push(new UpdateOnRedoCommand(this));
	stack.endMacro();
}

QMap<QString, Post3dWindowFaceDataItem::Setting> Post3dWindowArrowGroupDataItem::faceMap()
{
	QMap<QString, Post3dWindowFaceDataItem::Setting> map;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		Post3dWindowFaceDataItem* fItem = dynamic_cast<Post3dWindowFaceDataItem*>(*it);
		map.insert(fItem->standardItem()->text(), fItem->setting());
	}
	return map;
}

class Post3dWindowArrowGroupSetFaceMap : public QUndoCommand
{
public:
	Post3dWindowArrowGroupSetFaceMap(const QMap<QString, Post3dWindowFaceDataItem::Setting>& oldMap, const QMap<QString, Post3dWindowFaceDataItem::Setting>& newMap, Post3dWindowArrowGroupDataItem* item) :
		m_oldMap (oldMap),
		m_newMap (newMap),
		m_item {item}
	{}
	void redo() override {
		applySettings(m_newMap);
	}
	void undo() override {
		applySettings(m_oldMap);
	}
private:
	void applySettings(const QMap<QString, Post3dWindowFaceDataItem::Setting>& map) {
		removeChildren();
		for (auto it = map.begin(); it != map.end(); ++it) {
			Post3dWindowFaceDataItem* f = new Post3dWindowFaceDataItem(it.key(), m_item);
			f->setSetting(it.value(), true);
			m_item->m_childItems.push_back(f);
		}
		m_item->updateItemMap();
		m_item->setupAppendFilter();
	}

	void removeChildren() {
		auto children = m_item->m_childItems;
		for (auto child : children) {
			delete child;
		}
		m_item->updateItemMap();
	}
	QMap<QString, Post3dWindowFaceDataItem::Setting> m_oldMap;
	QMap<QString, Post3dWindowFaceDataItem::Setting> m_newMap;
	Post3dWindowArrowGroupDataItem* m_item;
};

void Post3dWindowArrowGroupDataItem::setFaceMap(const QMap<QString, Post3dWindowFaceDataItem::Setting>& map)
{
	iRICUndoStack::instance().push(new Post3dWindowArrowGroupSetFaceMap(faceMap(), map, this));
}

void Post3dWindowArrowGroupDataItem::innerUpdateZScale(double scale)
{
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->Scale(1, 1, scale);
	m_transformFilter->SetTransform(transform);
	updatePolyData();
	updateLegendData();
}

void Post3dWindowArrowGroupDataItem::update()
{
	updateActorSettings();
}

void Post3dWindowArrowGroupDataItem::setupActors()
{
	renderer()->AddActor(m_arrowsActor.actor());
	m_appendFilter = vtkSmartPointer<vtkAppendFilter>::New();

	m_transformFilter = vtkSmartPointer<vtkTransformFilter>::New();
	m_transformFilter->SetInputConnection(m_appendFilter->GetOutputPort());
	vtkSmartPointer<vtkIdentityTransform> transform = vtkSmartPointer<vtkIdentityTransform>::New();
	m_transformFilter->SetTransform(transform);

	m_maskPoints = vtkSmartPointer<vtkMaskPoints>::New();
	m_maskPoints->SetInputConnection(m_transformFilter->GetOutputPort());

	m_arrowsActor.actor()->VisibilityOff();

	renderer()->AddActor2D(m_legendActors.arrowActor());
	renderer()->AddActor2D(m_legendActors.textActor());

	m_legendActors.arrowActor()->VisibilityOff();
	m_legendActors.textActor()->VisibilityOff();

	m_legendActors.setPosition(0.75, 0.02);
	m_legendActors.setColor(0, 0, 0);
}

void Post3dWindowArrowGroupDataItem::updateActorSettings()
{
	const auto& s = m_setting;

	m_arrowsActor.actor()->VisibilityOff();

	m_actorCollection->RemoveAllItems();
	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr) {return;}
	vtkPointSet* ps = cont->data();
	if (ps == nullptr) {return;}
	if (s.target == "") {return;}

	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0) {return;}
	if (s.samplingMode == ArrowSettingContainer::SamplingMode::Rate) {
		m_maskPoints->SetOnRatio(s.samplingRate);
	} else {
		m_maskPoints->SetOnRatio(1);
	}

	setupAppendFilter();

	calculateStandardValue();
	updatePolyData();
	updateLegendData();
	updateColorSetting();

	m_actorCollection->AddItem(m_arrowsActor.actor());
	m_actor2DCollection->AddItem(m_legendActors.textActor());
	m_actor2DCollection->AddItem(m_legendActors.arrowActor());
	updateVisibilityWithoutRendering();
}

void Post3dWindowArrowGroupDataItem::updateColorSetting()
{
	Post3dWindowGridTypeDataItem* typedi = dynamic_cast<Post3dWindowGridTypeDataItem*>(parent()->parent());
	const auto& s = m_setting;
	switch (s.colorMode.value()) {
	case ArrowSettingContainer::ColorMode::Custom:
		m_arrowsActor.setColorModeToCustom(s.customColor);
		break;
	case ArrowSettingContainer::ColorMode::ByScalar:
		LookupTableContainer* stc = typedi->nodeLookupTable(s.colorTarget);
		m_arrowsActor.setColorModeToValue(iRIC::toStr(s.colorTarget), stc->vtkObj());
		break;
	}
}

void Post3dWindowArrowGroupDataItem::updatePolyData()
{
	const auto& s = m_setting;
	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr) {return;}
	vtkPointSet* ps = cont->data();
	if (ps == nullptr) {return;}
	if (s.target == "") {return;}
	if (m_appendFilter->GetNumberOfInputConnections(0) == 0) {return;}

	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0) {return;}

	pd->SetActiveVectors(iRIC::toStr(s.target).c_str());
	m_arrowsActor.setScaleFactor(m_setting.scaleFactor(dataModel()->graphicsView()->stdDistance(1)));

	double height = dataModel()->graphicsView()->stdDistance(s.arrowSize);
	m_arrowsActor.setConeHeight(height);

	m_maskPoints->Update();
	vtkPointSet* inPS = m_maskPoints->GetOutput();

	vtkPolyData* filteredData = m_setting.buildFilteredData(inPS);
	m_arrowsActor.setPolyData(filteredData);
	filteredData->Delete();

	m_arrowsActor.setLineWidth(m_setting.lineWidth);
	m_legendActors.setLineWidth(m_setting.lineWidth);
}

void Post3dWindowArrowGroupDataItem::setupAppendFilter()
{
	m_appendFilter->RemoveAllInputs();
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		Post3dWindowFaceDataItem* f = dynamic_cast<Post3dWindowFaceDataItem*>(*it);
		if (f->standardItem()->checkState() == Qt::Checked) {
			f->update();
			m_appendFilter->AddInputConnection(f->filter()->GetOutputPort());
		}
	}
}

void Post3dWindowArrowGroupDataItem::innerUpdate2Ds()
{
	vtkCamera* cam = renderer()->GetActiveCamera();
	double scale = cam->GetParallelScale();
	if (scale != m_setting.oldCameraScale) {
		updatePolyData();
		updateLegendData();
	}
	m_setting.oldCameraScale = scale;
}

void Post3dWindowArrowGroupDataItem::updateLegendData()
{
	const auto& s = m_setting;

	m_legendActors.update(iRIC::toStr(s.target), s.legendLength, s.standardValue, s.arrowSize, 15.0);

	if (s.colorMode == ArrowSettingContainer::ColorMode::Custom) {
		// specified color.
		m_legendActors.setColor(s.customColor);
	} else if (s.colorMode == ArrowSettingContainer::ColorMode::ByScalar) {
		// always black.
		m_legendActors.setColor(0, 0, 0);
	}
}

void Post3dWindowArrowGroupDataItem::calculateStandardValue()
{
	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr) {return;}

	m_setting.updateStandardValueIfNeeded(cont->data());
}

void Post3dWindowArrowGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);

	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement childElem = children.at(i).toElement();
		if (childElem.nodeName() == "FaceSetting") {
			QString label = childElem.attribute("label");
			Post3dWindowFaceDataItem* item = new Post3dWindowFaceDataItem(label, this);
			item->loadFromProjectMainFile(childElem);
			m_childItems.push_back(item);
		}
	}
	updateActorSettings();
}

void Post3dWindowArrowGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);

	for (int i = 0; i < m_childItems.size(); ++i) {
		Post3dWindowFaceDataItem* fitem = dynamic_cast<Post3dWindowFaceDataItem*>(m_childItems.at(i));
		writer.writeStartElement("FaceSetting");
		fitem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}
