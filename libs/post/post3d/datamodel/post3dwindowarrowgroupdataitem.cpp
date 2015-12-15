#include "../post3dwindowgraphicsview.h"
#include "../post3dwindowgraphicsview.h"
#include "post3dwindowarrowgroupdataitem.h"
#include "post3dwindowarrowgroupsettingdialog.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowzonedataitem.h"

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
#include <vtkGeometryFilter.h>
#include <vtkIdentityTransform.h>
#include <vtkLine.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkStructuredGrid.h>
#include <vtkTextProperty.h>
#include <vtkTransform.h>
#include <vtkTriangle.h>
#include <vtkVertex.h>

#include <cmath>

Post3dWindowArrowGroupDataItem::Post3dWindowArrowGroupDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem {tr("Arrow"), QIcon(":/libs/guibase/images/iconFolder.png"), p},
	m_oldCameraScale {1}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	m_scaleFactor = 1;

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();

	auto vectorNames = vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(cont->data()->GetPointData());
	if (vectorNames.size() > 0) {
		m_arrowSetting.target = vectorNames[0];
	}
	auto scalarNames = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cont->data()->GetPointData());
	if (scalarNames.size() > 0) {
		m_arrowSetting.colorTarget = scalarNames[0];
	}

	setupActors();
}

const std::string& Post3dWindowArrowGroupDataItem::target() const
{
	return m_target;
}

class Post3dWindowArrowGroupSetSetting : public QUndoCommand
{
public:
	Post3dWindowArrowGroupSetSetting(const ArrowSettingContainer& setting, const ArrowShapeContainer& shape, Post3dWindowArrowGroupDataItem* g)
	{
		m_oldEnabled = (g->standardItem()->checkState() == Qt::Checked);
		m_oldSetting = g->m_arrowSetting;
		m_oldShape = g->m_arrowShape;

		m_newEnabled = true;
		m_newSetting = setting;
		m_newShape = shape;

		m_group = g;
	}
	void redo() {
		m_group->m_isCommandExecuting = true;
		if (m_newEnabled) {
			m_group->standardItem()->setCheckState(Qt::Checked);
		} else {
			m_group->standardItem()->setCheckState(Qt::Unchecked);
		}
		m_group->m_arrowSetting = m_newSetting;
		m_group->m_arrowShape = m_newShape;
		m_group->m_isCommandExecuting = false;
	}
	void undo() {
		m_group->m_isCommandExecuting = true;
		if (m_oldEnabled) {
			m_group->standardItem()->setCheckState(Qt::Checked);
		} else {
			m_group->standardItem()->setCheckState(Qt::Unchecked);
		}
		m_group->m_arrowSetting = m_oldSetting;
		m_group->m_arrowShape = m_oldShape;
		m_group->m_isCommandExecuting = false;
	}
private:
	bool m_oldEnabled;
	ArrowSettingContainer m_oldSetting;
	ArrowShapeContainer m_oldShape;

	bool m_newEnabled;
	ArrowSettingContainer m_newSetting;
	ArrowShapeContainer m_newShape;

	Post3dWindowArrowGroupDataItem* m_group;
};

void Post3dWindowArrowGroupDataItem::setSetting(const ArrowSettingContainer& setting, const ArrowShapeContainer& shape)
{
	iRICUndoStack::instance().push(new Post3dWindowArrowGroupSetSetting(setting, shape, this));
}

void Post3dWindowArrowGroupDataItem::showSettingDialog()
{
	Post3dWindowArrowGroupSettingDialog* dialog = dynamic_cast<Post3dWindowArrowGroupSettingDialog*>(propertyDialog(mainWindow()));
	if (dialog == nullptr) {return;}
	int ret = dialog->exec();
	if (ret == QDialog::Accepted) {
		handlePropertyDialogAccepted(dialog);
	}
	delete dialog;
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
	dialog->setSetting(m_arrowSetting);
	dialog->setShape(m_arrowShape);

	return dialog;
}

class Post3dWindowArrowGroupDataItemUpdateOnRedo : public QUndoCommand
{
public:
	Post3dWindowArrowGroupDataItemUpdateOnRedo(Post3dWindowArrowGroupDataItem* item) : QUndoCommand() {
		m_item = item;
	}
	void redo() {
		m_item->updateActorSettings();
		m_item->renderGraphicsView();
	}
	void undo() {}
private:
	Post3dWindowArrowGroupDataItem* m_item;
};

class Post3dWindowArrowGroupDataItemUpdateOnUndo : public QUndoCommand
{
public:
	Post3dWindowArrowGroupDataItemUpdateOnUndo(Post3dWindowArrowGroupDataItem* item) : QUndoCommand() {
		m_item = item;
	}
	void redo() {}
	void undo() {
		m_item->updateActorSettings();
		m_item->renderGraphicsView();
	}
private:
	Post3dWindowArrowGroupDataItem* m_item;
};

void Post3dWindowArrowGroupDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	Post3dWindowArrowGroupSettingDialog* dialog = dynamic_cast<Post3dWindowArrowGroupSettingDialog*>(propDialog);
	iRICUndoStack& stack = iRICUndoStack::instance();
	stack.beginMacro(tr("Arrow Setting"));
	stack.push(new Post3dWindowArrowGroupDataItemUpdateOnUndo(this));
	setSetting(dialog->setting(), dialog->shape());
	setFaceMap(dialog->faceMap());
	stack.push(new Post3dWindowArrowGroupDataItemUpdateOnRedo(this));
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
		removeChildren();
		for (auto it = m_newMap.begin(); it != m_newMap.end(); ++it) {
			Post3dWindowFaceDataItem* f = new Post3dWindowFaceDataItem(it.key(), m_item);
			f->setSetting(it.value(), true);
			m_item->m_childItems.append(f);
		}
		m_item->updateItemMap();
		m_item->setupAppendFilter();
	}
	void undo() override {
		removeChildren();
		for (auto it = m_oldMap.begin(); it != m_oldMap.end(); ++it) {
			Post3dWindowFaceDataItem* f = new Post3dWindowFaceDataItem(it.key(), m_item);
			f->setSetting(it.value(), true);
			m_item->m_childItems.append(f);
		}
		m_item->updateItemMap();
		m_item->setupAppendFilter();
	}
private:
	void removeChildren() {
		for (auto child : m_item->m_childItems) {
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
	m_arrowActor = vtkSmartPointer<vtkActor>::New();
	renderer()->AddActor(m_arrowActor);

	m_arrowMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_arrowActor->SetMapper(m_arrowMapper);

	m_hedgeHog = vtkSmartPointer<vtkHedgeHog>::New();
	m_hedgeHog->SetVectorModeToUseVector();
	m_hedgeHog->SetScaleFactor(m_scaleFactor);

	m_warpVector = vtkSmartPointer<vtkWarpVector>::New();

	m_appendFilter = vtkSmartPointer<vtkAppendFilter>::New();

	m_transformFilter = vtkSmartPointer<vtkTransformFilter>::New();
	m_transformFilter->SetInputConnection(m_appendFilter->GetOutputPort());
	vtkSmartPointer<vtkIdentityTransform> transform = vtkSmartPointer<vtkIdentityTransform>::New();
	m_transformFilter->SetTransform(transform);

	m_maskPoints = vtkSmartPointer<vtkMaskPoints>::New();
	m_maskPoints->SetInputConnection(m_transformFilter->GetOutputPort());

	m_arrowGlyph = vtkSmartPointer<vtkGlyph3D>::New();
	m_arrowGlyph->SetScaleModeToDataScalingOff();
	m_arrowGlyph->SetVectorModeToUseVector();
	m_arrowGlyph->SetInputConnection(m_warpVector->GetOutputPort());

	m_arrowSource = vtkSmartPointer<vtkConeSource>::New();
	m_arrowGlyph->SetSourceConnection(m_arrowSource->GetOutputPort());

	m_appendPolyData = vtkSmartPointer<vtkAppendPolyData>::New();
	m_appendPolyData->AddInputConnection(m_hedgeHog->GetOutputPort());
	m_appendPolyData->AddInputConnection(m_arrowGlyph->GetOutputPort());

	m_polyData = vtkSmartPointer<vtkPolyData>::New();
	m_arrowMapper->SetInputData(m_polyData);

	m_arrowActor->VisibilityOff();

	m_legendTextActor = vtkSmartPointer<vtkTextActor>::New();
	m_legendTextActor->SetTextScaleModeToNone();
	m_legendTextActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	m_legendTextActor->SetPosition(0.75, 0.02);
	vtkTextProperty* prop = m_legendTextActor->GetTextProperty();
	prop->SetColor(0, 0, 0);
	prop->SetFontFamilyToArial();
	prop->SetJustificationToCentered();
	prop->SetVerticalJustificationToBottom();

	m_legendTextActor->VisibilityOff();
	renderer()->AddActor2D(m_legendTextActor);

	m_baseArrowPolyData = vtkSmartPointer<vtkUnstructuredGrid>::New();

	vtkSmartPointer<vtkPolyDataMapper2D> mapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
	vtkSmartPointer<vtkGeometryFilter> f = vtkSmartPointer<vtkGeometryFilter>::New();
	f->SetInputData(m_baseArrowPolyData);
	mapper->SetInputConnection(f->GetOutputPort());

	m_baseArrowActor = vtkSmartPointer<vtkActor2D>::New();
	m_baseArrowActor->SetMapper(mapper);

	m_baseArrowActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	m_baseArrowActor->GetPositionCoordinate()->SetValue(.75, .02);
	m_baseArrowActor->GetProperty()->SetColor(0, 0, 0);
	m_baseArrowActor->VisibilityOff();

	m_activePoints = vtkSmartPointer<vtkPolyData>::New();
	m_warpVector->SetInputData(m_activePoints);
	m_hedgeHog->SetInputData(m_activePoints);

	renderer()->AddActor2D(m_baseArrowActor);
}

void Post3dWindowArrowGroupDataItem::updateActorSettings()
{
	const auto& s = m_arrowSetting;

	m_arrowActor->VisibilityOff();
	m_actorCollection->RemoveAllItems();
	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr) {return;}
	vtkPointSet* ps = cont->data();
	if (ps == nullptr) {return;}
	if (s.target == "") {return;}

	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0) {return;}
	m_maskPoints->SetOnRatio(s.samplingRate());

	setupAppendFilter();
	if (m_appendFilter->GetNumberOfInputConnections(0) == 0) {return;}

	calculateStandardValue();
	updatePolyData();
	updateLegendData();
	updateColorSetting();

	m_actorCollection->AddItem(m_arrowActor);
	m_actor2DCollection->AddItem(m_legendTextActor);
	m_actor2DCollection->AddItem(m_baseArrowActor);
	updateVisibilityWithoutRendering();
}

void Post3dWindowArrowGroupDataItem::updateColorSetting()
{
	Post3dWindowGridTypeDataItem* typedi = dynamic_cast<Post3dWindowGridTypeDataItem*>(parent()->parent());
	const auto& s = m_arrowSetting;
	const QColor& color = s.customColor();
	switch (s.colorMode()) {
	case ArrowSettingContainer::ColorMode::Custom:
		m_arrowMapper->ScalarVisibilityOff();
		m_arrowActor->GetProperty()->SetColor(color.redF(), color.greenF(), color.blueF());
		break;
	case ArrowSettingContainer::ColorMode::ByScalar:
		m_arrowMapper->ScalarVisibilityOn();
		LookupTableContainer* stc = typedi->nodeLookupTable(s.colorTarget);
		m_arrowMapper->SetScalarModeToUsePointFieldData();
		m_arrowMapper->SelectColorArray(s.colorAttribute.c_str());
		m_arrowMapper->SetLookupTable(stc->vtkObj());
		m_arrowMapper->UseLookupTableScalarRangeOn();
		break;
	}
}

void Post3dWindowArrowGroupDataItem::updatePolyData()
{
	m_polyData->Initialize();
	m_activePoints->Initialize();
	vtkSmartPointer<vtkPoints> outPoints = vtkSmartPointer<vtkPoints>::New();
	outPoints->SetDataTypeToDouble();
	m_activePoints->SetPoints(outPoints);

	const auto& s = m_arrowSetting;
	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr) {return;}
	vtkPointSet* ps = cont->data();
	if (ps == nullptr) {return;}
	if (s.target == "") {return;}
	if (m_appendFilter->GetNumberOfInputConnections(0) == 0) {return;}
	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0) {return;}
	pd->SetActiveVectors(s.target);

	updateScaleFactor();
	double height = dataModel()->graphicsView()->stdDistance(m_arrowShape.arrowSize());
	m_hedgeHog->SetScaleFactor(m_scaleFactor);
	m_warpVector->SetScaleFactor(m_scaleFactor);
	m_arrowSource->SetHeight(height);
	m_arrowSource->SetAngle(15);
	m_arrowSource->Modified();

	m_maskPoints->Update();
	vtkPointSet* inPS = m_maskPoints->GetOutput();
	vtkDataArray* vectorArray = inPS->GetPointData()->GetArray(s.target);

	QSet<vtkIdType> pointIds;
	double minlimitsqr = s.minimumValue() * s.minimumValue();
	for (vtkIdType i = 0; i < inPS->GetNumberOfPoints(); ++i) {
		bool active = true;
		double val = 0;
		for (int j = 0; j < vectorArray->GetNumberOfComponents(); ++j) {
			double tmpval = vectorArray->GetComponent(i, j);
			val += tmpval * tmpval;
		}
		if (val < minlimitsqr) {
			active = false;
		}
		if (active) {
			pointIds.insert(i);
		}
	}

	vtkPointData* inPD = inPS->GetPointData();
	vtkPointData* outPD = m_activePoints->GetPointData();
	vtkPoints* inPoints = inPS->GetPoints();
	vtkSmartPointer<vtkCellArray> ca = vtkSmartPointer<vtkCellArray>::New();

	outPD->CopyAllocate(inPD, pointIds.size());
	vtkIdType newId = 0;
	for (auto it = pointIds.begin(); it != pointIds.end(); ++it) {
		vtkIdType pointid = *it;
		outPoints->InsertNextPoint(inPoints->GetPoint(pointid));
		outPD->CopyData(inPD, pointid, newId);
		ca->InsertNextCell(1, &newId);
		++ newId;
	}
	m_activePoints->SetVerts(ca);
	m_activePoints->Modified();

	m_appendPolyData->Update();
	m_polyData->DeepCopy(m_appendPolyData->GetOutput());
	m_arrowActor->GetProperty()->SetLineWidth(m_arrowShape.lineWidth());
	m_baseArrowActor->GetProperty()->SetLineWidth(m_arrowShape.lineWidth());
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
	if (scale != m_oldCameraScale) {
		updatePolyData();
		updateLegendData();
	}
	m_oldCameraScale = scale;
}

void Post3dWindowArrowGroupDataItem::updateLegendData()
{
	const auto& s = m_arrowSetting;
	double vectorOffset = 18;
	double arrowLen = s.legendLength();
	m_baseArrowPolyData->Initialize();
	m_baseArrowPolyData->Allocate(3);

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	m_baseArrowPolyData->SetPoints(points);
	// add line
	points->InsertNextPoint(- arrowLen * .5, vectorOffset, 0);
	points->InsertNextPoint(arrowLen * .5, vectorOffset, 0);
	vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
	line->GetPointIds()->SetId(0, 0);
	line->GetPointIds()->SetId(1, 1);
	m_baseArrowPolyData->InsertNextCell(line->GetCellType(), line->GetPointIds());

	// add triangle
	points->InsertNextPoint(arrowLen * .5 - 8, vectorOffset + 3, 0);
	points->InsertNextPoint(arrowLen * .5 - 8, vectorOffset - 3, 0);
	vtkSmartPointer<vtkTriangle> tri = vtkSmartPointer<vtkTriangle>::New();
	tri->GetPointIds()->SetId(0, 1);
	tri->GetPointIds()->SetId(1, 2);
	tri->GetPointIds()->SetId(2, 3);
	m_baseArrowPolyData->InsertNextCell(tri->GetCellType(), tri->GetPointIds());

	QString lenStr = QString("%1\n\n%2").arg(s.target).arg(s.standardValue());
	m_legendTextActor->SetInput(iRIC::toStr(lenStr).c_str());

	const QColor& color = s.customColor();
	if (s.colorMode() == ArrowSettingContainer::ColorMode::Custom) {
		// specified color.
		m_baseArrowActor->GetProperty()->SetColor(color.redF(), color.greenF(), color.blueF());
	} else if (s.colorMode() == ArrowSettingContainer::ColorMode::ByScalar) {
		// always black.
		m_baseArrowActor->GetProperty()->SetColor(0, 0, 0);
	}
}

void Post3dWindowArrowGroupDataItem::calculateStandardValue()
{
	auto& s = m_arrowSetting;
	if (s.lengthMode() == ArrowSettingContainer::LengthMode::Custom) {return;}
	QVector<double> lenVec;
	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return;}
	vtkPointSet* ps = cont->data();
	if (s.target == "") {return;}
	vtkPointData* pd = ps->GetPointData();
	vtkDataArray* da = pd->GetArray(s.target);

	for (vtkIdType i = 0; i < da->GetNumberOfTuples(); ++i) {
		double* v = da->GetTuple3(i);
		QVector2D vec(*(v), *(v + 1));
		lenVec.append(vec.length());
	}
	qSort(lenVec);
	double sum = 0;
	int count = AUTO_AVERAGECOUNT;
	if (count > lenVec.count()) {count = lenVec.count();}
	for (int i = 0; i < count; ++i) {
		sum += lenVec.at(lenVec.count() - i - 1);
	}
	double average = sum / count;
	if (average == 0) {average = 1.0;}

	int p = 0;
	double p2 = 1;
	while (average > 10) {
		average /= 10.;
		++ p;
		p2 = 10;
	}
	while (average < 1) {
		average *= 10;
		++ p;
		p2 = 0.1;
	}
	average = static_cast<int>(average);
	for (int i = 0; i < p; ++i) {
		average *= p2;
	}
	// now average is calculated.
	s.setStandardValue(average);
}

void Post3dWindowArrowGroupDataItem::updateScaleFactor()
{
	double a = 1.0 / dataModel()->graphicsView()->stdDistance(1.0);
	const auto& s = m_arrowSetting;
	m_scaleFactor = s.legendLength() / (a * s.standardValue());
}

void Post3dWindowArrowGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_oldCameraScale = iRIC::getDoubleAttribute(node, "oldCameraScale", 1);
	m_arrowSetting.load(node);
	m_arrowShape.load(node);

	updateScaleFactor();

	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement childElem = children.at(i).toElement();
		if (childElem.nodeName() == "FaceSetting") {
			QString label = childElem.attribute("label");
			Post3dWindowFaceDataItem* item = new Post3dWindowFaceDataItem(label, this);
			item->loadFromProjectMainFile(childElem);
			m_childItems.append(item);
		}
	}
	updateActorSettings();
}

void Post3dWindowArrowGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	iRIC::setDoubleAttribute(writer, "oldCameraScale", m_oldCameraScale);
	m_arrowSetting.save(writer);
	m_arrowShape.save(writer);

	for (int i = 0; i < m_childItems.count(); ++i) {
		Post3dWindowFaceDataItem* fitem = dynamic_cast<Post3dWindowFaceDataItem*>(m_childItems.at(i));
		writer.writeStartElement("FaceSetting");
		fitem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}
