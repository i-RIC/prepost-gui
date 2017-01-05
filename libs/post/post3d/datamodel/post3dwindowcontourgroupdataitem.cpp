#include "post3dwindowcontourgroupdataitem.h"
#include "post3dwindowcontourgroupsettingdialog.h"
#include "post3dwindowcontourgrouptopdataitem.h"
#include "post3dwindowfacedataitem.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowzonedataitem.h"
#include "private/post3dwindowcontourgroupdataitem_setsettingcommand.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guibase/graphicsmisc.h>
#include <guicore/datamodel/graphicswindowdrawcommands.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>

#include <QDomNode>
#include <QDomElement>
#include <QXmlStreamWriter>
#include <QMainWindow>
#include <QStandardItem>
#include <QUndoCommand>

#include <vtkActor.h>
#include <vtkAlgorithmOutput.h>
#include <vtkAppendPolyData.h>
#include <vtkCellData.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkContourFilter.h>
#include <vtkDataArray.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkLODActor.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkTextProperty.h>

class Post3dWindowContourGroupUpdateOnRedo : public QUndoCommand
{
public:
	Post3dWindowContourGroupUpdateOnRedo(Post3dWindowContourGroupDataItem *item, QUndoCommand* parent = nullptr)
		: QUndoCommand(QObject::tr("Update"), parent)
		, m_item(item)
	{}
	void redo() {
		m_item->updateItemMap();
		m_item->updateChildActors();
	}
private:
	Post3dWindowContourGroupDataItem *m_item;
};

class Post3dWindowContourGroupUpdateOnUndo : public QUndoCommand
{
public:
	Post3dWindowContourGroupUpdateOnUndo(Post3dWindowContourGroupDataItem *item, QUndoCommand* parent = nullptr)
		: QUndoCommand(QObject::tr("Update"), parent)
		, m_item(item)
	{}
	void undo() {
		m_item->updateItemMap();
		m_item->updateChildActors();
	}
private:
	Post3dWindowContourGroupDataItem *m_item;
};

Post3dWindowContourGroupDataItem::Post3dWindowContourGroupDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem {tr("Contour"), QIcon(":/libs/guibase/images/iconFolder.png"), p},
	m_zScale {1}
{
	setupStandardItem(Checked, NotReorderable, Deletable);

	Post3dWindowZoneDataItem* zItem = dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent());
	PostZoneDataContainer* c = zItem->dataContainer();
	m_scalarSetting.target = c->data()->GetPointData()->GetArrayName(0);
	m_scalarSetting.numberOfDivisions = 10;

	setupScalarBarActor();
}

Post3dWindowContourGroupDataItem::~Post3dWindowContourGroupDataItem()
{
	m_scalarBarWidget->SetInteractor(0);
}

const ScalarSettingContainer& Post3dWindowContourGroupDataItem::scalarSetting() const
{
	return m_scalarSetting;
}

void Post3dWindowContourGroupDataItem::updateChildActors()
{
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		Post3dWindowFaceDataItem* f = dynamic_cast<Post3dWindowFaceDataItem*>(*it);
		f->update();
		vtkAlgorithmOutput* algo = f->getOutputPort();
		vtkActor* a = setupActorAndMapper(algo);
		f->setActor(a);
		f->updateZScale(m_zScale);
		a->Delete();
	}
	updateScalarBarActorSetting();
	updateVisibilityWithoutRendering();
}

vtkActor* Post3dWindowContourGroupDataItem::setupActorAndMapper(vtkAlgorithmOutput* algo)
{
	switch (m_scalarSetting.contour.value()) {
	case ContourSettingWidget::Points:
		// do nothing
		break;
	case ContourSettingWidget::Isolines:
		return setupIsolinesActorAndMapper(algo);
	case ContourSettingWidget::ContourFigure:
		return setupContourFigureActorAndMapper(algo);
	case ContourSettingWidget::ColorFringe:
	default:
		return setupColorFringeActorAndMapper(algo);
	}
	return 0;
}

vtkActor* Post3dWindowContourGroupDataItem::setupIsolinesActorAndMapper(vtkAlgorithmOutput* algo)
{
	LookupTableContainer* lookup = lookupTable();
	double range[2];
	lookup->getValueRange(&(range[0]), &(range[1]));

	// setup filter.
	vtkContourFilter* contourFilter = vtkContourFilter::New();
	contourFilter->SetInputConnection(algo);
	contourFilter->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, iRIC::toStr(m_scalarSetting.target).c_str());
	contourFilter->GenerateValues(m_scalarSetting.numberOfDivisions + 1, range);

	// setup mapper
	vtkDataSetMapper* mapper = vtkDataSetMapper::New();
	mapper->SetInputConnection(contourFilter->GetOutputPort());
	contourFilter->Delete();
	mapper->SetLookupTable(lookup->vtkObj());
	mapper->SetScalarModeToUsePointFieldData();
	mapper->SelectColorArray(iRIC::toStr(m_scalarSetting.target).c_str());
	mapper->UseLookupTableScalarRangeOn();

	// setup actor
	vtkActor* actor = vtkLODActor::New();
	actor->SetMapper(mapper);
	mapper->Delete();
	actor->GetProperty()->SetLineWidth(1);
	actor->GetProperty()->SetLighting(false);

	return actor;
}

vtkActor* Post3dWindowContourGroupDataItem::setupContourFigureActorAndMapper(vtkAlgorithmOutput* algo)
{
	LookupTableContainer* lookup = lookupTable();

	// correspond to Post2dWindowNodeScalarGroupDataItem::createRangeClippedPolyData()
	vtkStructuredGridGeometryFilter* filter = vtkStructuredGridGeometryFilter::New();
	filter->SetInputConnection(algo);
	filter->Update();
	vtkPolyData* rangeClippedPolyData = vtkPolyData::New();
	rangeClippedPolyData->DeepCopy(filter->GetOutput());
	filter->Delete();

	// correspond to Post2dWindowNodeScalarGroupDataItem::createValueClippedPolyData()
	vtkPolyData* upperClipped = vtkPolyData::New();
	vtkPolyData* lowerClipped = vtkPolyData::New();
	double min, max;
	lookup->getValueRange(&min, &max);
	if (m_scalarSetting.fillLower) {
		lowerClipped->DeepCopy(rangeClippedPolyData);
		rangeClippedPolyData->Delete();
	} else {
		vtkClipPolyData* lowerClipper = vtkClipPolyData::New();
		lowerClipper->SetValue(min);
		lowerClipper->SetInputData(rangeClippedPolyData);
		lowerClipper->InsideOutOff();
		rangeClippedPolyData->GetPointData()->SetActiveScalars(iRIC::toStr(m_scalarSetting.target).c_str());

		lowerClipper->Update();
		lowerClipped->DeepCopy(lowerClipper->GetOutput());
		lowerClipped->GetPointData()->SetActiveScalars("");
		lowerClipper->Delete();
		rangeClippedPolyData->Delete();
	}
	if (m_scalarSetting.fillUpper) {
		upperClipped->DeepCopy(lowerClipped);
		lowerClipped->Delete();
	} else {
		vtkClipPolyData* upperClipper = vtkClipPolyData::New();
		upperClipper->SetValue(max);
		upperClipper->SetInputData(lowerClipped);
		upperClipper->InsideOutOn();
		lowerClipped->GetPointData()->SetActiveScalars(iRIC::toStr(m_scalarSetting.target).c_str());
		upperClipper->Update();
		upperClipped->DeepCopy(upperClipper->GetOutput());
		upperClipped->GetPointData()->SetActiveScalars("");
		upperClipper->Delete();
		lowerClipped->Delete();
	}
	vtkPolyData* valueClippedPolyData = vtkPolyData::New();
	valueClippedPolyData->DeepCopy(upperClipped);
	upperClipped->Delete();

	// setup filter
	valueClippedPolyData->GetPointData()->SetActiveScalars(iRIC::toStr(m_scalarSetting.target).c_str());
	vtkAppendPolyData* appendFilledContours = vtkAppendPolyData::New();
	double delta = (max - min) / static_cast<double>(m_scalarSetting.numberOfDivisions);
	std::vector<vtkClipPolyData*> clippersLo;
	std::vector<vtkClipPolyData*> clippersHi;

	for (int i = 0; i < m_scalarSetting.numberOfDivisions; i++) {
		double valueLo = min + static_cast<double>(i) * delta;
		double valueHi = min + static_cast<double>(i + 1) * delta;
		clippersLo.push_back(vtkClipPolyData::New());
		if (i == 0) {
			clippersLo[i]->SetValue(-HUGE_VAL);
			clippersLo[i]->SetInputData(valueClippedPolyData);
			valueClippedPolyData->Delete();
		} else {
			clippersLo[i]->SetValue(valueLo);
			clippersLo[i]->SetInputConnection(clippersLo[i - 1]->GetOutputPort());
		}
		clippersLo[i]->InsideOutOff();
		clippersLo[i]->Update();

		clippersHi.push_back(vtkClipPolyData::New());
		if (i < m_scalarSetting.numberOfDivisions - 1) {
			clippersHi[i]->SetValue(valueHi);
		} else {
			clippersHi[i]->SetValue(HUGE_VAL);
		}
		clippersHi[i]->SetInputConnection(clippersLo[i]->GetOutputPort());
		clippersHi[i]->GenerateClippedOutputOn();
		clippersHi[i]->InsideOutOn();
		clippersHi[i]->Update();
		if (clippersHi[i]->GetOutput()->GetNumberOfCells() == 0) {
			continue;
		}

		vtkDoubleArray* da = vtkDoubleArray::New();
		da->SetNumberOfComponents(1);
		da->SetNumberOfTuples(clippersHi[i]->GetOutput()->GetNumberOfCells());
		da->FillComponent(0, min + (max - min) * (i / (m_scalarSetting.numberOfDivisions - 1.0)));
		clippersHi[i]->GetOutput()->GetCellData()->SetScalars(da);
		appendFilledContours->AddInputConnection(clippersHi[i]->GetOutputPort());
		da->Delete();
	}
	for (int i = 0; i < m_scalarSetting.numberOfDivisions; i++) {
		clippersLo[i]->Delete();
		clippersHi[i]->Delete();
	}
	clippersLo.clear();
	clippersHi.clear();
	vtkCleanPolyData* filledContours = vtkCleanPolyData::New();
	filledContours->SetInputConnection(appendFilledContours->GetOutputPort());
	filledContours->Update();
	appendFilledContours->Delete();

	// setup mapper
	vtkDataSetMapper* mapper = vtkDataSetMapper::New();
	mapper->SetInputConnection(filledContours->GetOutputPort());
	filledContours->Delete();
	mapper->SetScalarRange(min, max);
	mapper->SetScalarModeToUseCellData();
	mapper->SetLookupTable(lookup->vtkObj());
	mapper->UseLookupTableScalarRangeOn();

	// setup actor
	vtkActor* actor = vtkActor::New();
	actor->SetMapper(mapper);
	mapper->Delete();
	actor->GetProperty()->SetInterpolationToFlat();
	actor->GetProperty()->SetLighting(false);

	return actor;
}

vtkActor* Post3dWindowContourGroupDataItem::setupColorFringeActorAndMapper(vtkAlgorithmOutput* algo)
{
	LookupTableContainer* lookup =  lookupTable();

	// setup mapper
	vtkDataSetMapper* mapper = vtkDataSetMapper::New();
	mapper->SetInputConnection(algo);
	mapper->SetScalarModeToUsePointFieldData();
	mapper->SelectColorArray(iRIC::toStr(m_scalarSetting.target).c_str());
	mapper->SetLookupTable(lookup->vtkObj());
	mapper->UseLookupTableScalarRangeOn();

	// setup actor
	vtkActor* actor = vtkLODActor::New();
	actor->SetMapper(mapper);
	mapper->Delete();
	actor->GetProperty()->SetLighting(false);

	return actor;
}

void Post3dWindowContourGroupDataItem::updateScalarBarActorSetting()
{
	LookupTableContainer* lookup = lookupTable();
	if (lookup == nullptr) { return; }

	vtkScalarBarActor* a = m_scalarBarWidget->GetScalarBarActor();
	std::string targetStr = iRIC::toStr(m_scalarSetting.target);

	Post3dWindowContourGroupTopDataItem* topitem = dynamic_cast<Post3dWindowContourGroupTopDataItem*>(parent());
	a->SetTitle(iRIC::toStr(topitem->m_colorBarTitleMap.value(targetStr)).c_str());
	a->SetLookupTable(lookup->vtkObj());

	m_standardItem->setText(topitem->m_colorBarTitleMap.value(targetStr));

	auto& s = m_scalarSetting.scalarBarSetting;

	a->SetNumberOfLabels(s.numberOfLabels);
	s.titleTextSetting.applySetting(a->GetTitleTextProperty());
	s.labelTextSetting.applySetting(a->GetLabelTextProperty());
	switch (m_scalarSetting.contour.value()) {
	case ContourSettingWidget::Points:
		// do nothing
		break;
	case ContourSettingWidget::Isolines:
		a->SetMaximumNumberOfColors(m_scalarSetting.numberOfDivisions);
		break;
	case ContourSettingWidget::ContourFigure:
		a->SetMaximumNumberOfColors(m_scalarSetting.numberOfDivisions);
		break;
	case ContourSettingWidget::ColorFringe:
		a->SetMaximumNumberOfColors(256);
		break;
	}
}

void Post3dWindowContourGroupDataItem::setupScalarBarActor()
{
	vtkRenderWindowInteractor* iren = renderer()->GetRenderWindow()->GetInteractor();

	m_scalarBarWidget = vtkSmartPointer<vtkScalarBarWidget>::New();
	iRIC::setupScalarBarProperty(m_scalarBarWidget->GetScalarBarActor());
	m_scalarBarWidget->SetEnabled(0);
	m_scalarBarWidget->SetInteractor(iren);

	m_scalarSetting.scalarBarSetting.saveToRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	updateScalarBarActorSetting();
}

void Post3dWindowContourGroupDataItem::update()
{
	updateChildActors();
}

void Post3dWindowContourGroupDataItem::updateVisibility(bool visible)
{
	PostZoneDataContainer* zoneData = dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	bool dataOK = (zoneData != nullptr && zoneData->data() != nullptr);
	if (! dataOK) {
		m_scalarBarWidget->SetEnabled(0);
		return;
	}

	bool childVis = false;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it){
		Post3dWindowFaceDataItem* f = dynamic_cast<Post3dWindowFaceDataItem*>(*it);
		if (f->standardItem()->checkState() == Qt::Checked){
			childVis = true;
			break;
		}
	}

	bool v = (m_standardItem->checkState() == Qt::Checked) && visible && childVis;
	m_scalarBarWidget->SetEnabled(m_scalarSetting.scalarBarSetting.visible && v);

	GraphicsWindowDataItem::updateVisibility(visible);
}

void Post3dWindowContourGroupDataItem::handleStandardItemChange()
{
	GraphicsWindowDataItem::updateVisibility();
}

QDialog* Post3dWindowContourGroupDataItem::propertyDialog(QWidget* p)
{
	Post3dWindowContourGroupSettingDialog* dialog = new Post3dWindowContourGroupSettingDialog(p);
	dialog->setEnabled(true);
	Post3dWindowGridTypeDataItem* gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*>(parent()->parent()->parent());
	dialog->setGridTypeDataItem(gtItem);
	PostZoneDataContainer* zoneData = dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	if (zoneData == nullptr || zoneData->data() == nullptr) {
		delete dialog;
		return nullptr;
	}
	dialog->setZoneData(zoneData);

	m_scalarSetting.scalarBarSetting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	Post3dWindowContourGroupTopDataItem* topitem = dynamic_cast<Post3dWindowContourGroupTopDataItem*>(parent());
	dialog->setColorBarTitleMap(topitem->m_colorBarTitleMap);

	dialog->setScalarSetting(m_scalarSetting);
	dialog->setLookupTable(*(lookupTable()));
	dialog->setFaceMap(faceMap());

	return dialog;
}

void Post3dWindowContourGroupDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	QUndoCommand* contourSetting = new QUndoCommand(tr("Contour Setting"));
	undoCommands(propDialog, contourSetting);
	iRICUndoStack::instance().push(contourSetting);
}

LookupTableContainer* Post3dWindowContourGroupDataItem::lookupTable()
{
	Post3dWindowGridTypeDataItem* gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*>(parent()->parent()->parent());
	return gtItem->nodeLookupTable(iRIC::toStr(m_scalarSetting.target));
}

QMap<QString, Post3dWindowFaceDataItem::Setting> Post3dWindowContourGroupDataItem::faceMap() const
{
	QMap<QString, Post3dWindowFaceDataItem::Setting> map;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		Post3dWindowFaceDataItem* fItem = dynamic_cast<Post3dWindowFaceDataItem*>(*it);
		map.insert(fItem->standardItem()->text(), fItem->setting());
	}
	return map;
}

class Post3dWindowContourGroupSetFaceMap : public QUndoCommand
{
public:
	Post3dWindowContourGroupSetFaceMap(const QMap<QString, Post3dWindowFaceDataItem::Setting>& oldMap, const QMap<QString, Post3dWindowFaceDataItem::Setting>& newMap, QString target, Post3dWindowContourGroupDataItem* item, QUndoCommand* parent = nullptr) :
		QUndoCommand(parent) 
	{
		m_oldMap = oldMap;
		m_newMap = newMap;
		m_item = item;
		m_oldTarget = item->m_scalarSetting.target;
		m_newTarget = target;
	}
	void redo() {
		removeChildren();
		for (auto it = m_newMap.begin(); it != m_newMap.end(); ++it) {
			Post3dWindowFaceDataItem* f = new Post3dWindowFaceDataItem(it.key(), m_item);
			f->setSetting(it.value(), true);
			m_item->m_childItems.push_back(f);
		}
		m_item->m_scalarSetting.target = m_newTarget;
	}
	void undo() {
		removeChildren();
		for (auto it = m_oldMap.begin(); it != m_oldMap.end(); ++it) {
			Post3dWindowFaceDataItem* f = new Post3dWindowFaceDataItem(it.key(), m_item);
			f->setSetting(it.value(), true);
			m_item->m_childItems.push_back(f);
		}
		m_item->m_scalarSetting.target = m_oldTarget;
	}
private:
	void removeChildren() {
		auto copy = m_item->m_childItems;
		for (auto child : copy) {
			delete child;
		}
		m_item->updateItemMap();
	}
	QMap<QString, Post3dWindowFaceDataItem::Setting> m_oldMap;
	QMap<QString, Post3dWindowFaceDataItem::Setting> m_newMap;
	Post3dWindowContourGroupDataItem* m_item;
	QString m_oldTarget;
	QString m_newTarget;
};

void Post3dWindowContourGroupDataItem::setFaceMap(const QMap<QString, Post3dWindowFaceDataItem::Setting>& map, QString target)
{
	iRICUndoStack::instance().push(new Post3dWindowContourGroupSetFaceMap(faceMap(), map, target, this));
}

void Post3dWindowContourGroupDataItem::undoCommands(QDialog* propDialog, QUndoCommand* parent)
{
	Post3dWindowContourGroupSettingDialog* dialog = dynamic_cast<Post3dWindowContourGroupSettingDialog*>(propDialog);

	Q_ASSERT(parent != nullptr); // the following won't get deleted if parent is null

	new GraphicsWindowDrawOnUndo(this, parent);
	new Post3dWindowContourGroupUpdateOnUndo(this, parent);
	new SetSettingCommand(dialog->scalarSetting(), dialog->lookupTable(), dialog->scalarBarTitle(), this, parent);
	new Post3dWindowContourGroupSetFaceMap(faceMap(), dialog->faceMap(), dialog->scalarSetting().target, this, parent);
	new Post3dWindowContourGroupUpdateOnRedo(this, parent);
	new GraphicsWindowDrawOnRedo(this, parent);
}

void Post3dWindowContourGroupDataItem::innerUpdateZScale(double scale)
{
	m_zScale = scale;
}

void Post3dWindowContourGroupDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	m_scalarBarWidget->SetRepositionable(1);
}

void Post3dWindowContourGroupDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	m_scalarBarWidget->SetRepositionable(0);
}

void Post3dWindowContourGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
}

void Post3dWindowContourGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void Post3dWindowContourGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
}

void Post3dWindowContourGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_scalarSetting.load(node);

	m_scalarSetting.scalarBarSetting.saveToRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());

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
	updateChildActors();
	updateScalarBarActorSetting();
}

void Post3dWindowContourGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_scalarSetting.scalarBarSetting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());

	m_scalarSetting.save(writer);

	for (int i = 0; i < m_childItems.size(); ++i) {
		Post3dWindowFaceDataItem* fitem = dynamic_cast<Post3dWindowFaceDataItem*>(m_childItems.at(i));
		writer.writeStartElement("FaceSetting");
		fitem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}
