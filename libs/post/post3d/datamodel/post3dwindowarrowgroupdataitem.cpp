#include "../post3dwindowgraphicsview.h"
#include "post3dwindowarrowgroupdataitem.h"
#include "post3dwindowarrowgroupsettingdialog.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowzonedataitem.h"

#include <cmath>

#include <guicore/datamodel/graphicswindowdrawcommands.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include "../post3dwindowgraphicsview.h"

#include <QMainWindow>
#include <QSet>
#include <QSettings>
#include <QStandardItem>
#include <QUndoCommand>
#include <QVector2D>

#include <vtkActor2DCollection.h>
#include <vtkAlgorithmOutput.h>
#include <vtkCamera.h>
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
#include <vtkCellArray.h>

Post3dWindowArrowGroupDataItem::Post3dWindowArrowGroupDataItem(Post3dWindowDataItem* p)
	: Post3dWindowDataItem(tr("Arrow"), QIcon(":/libs/guibase/images/iconFolder.png"), p)
{
	m_oldCameraScale = 1;

	m_standardItem->setCheckable(true);
	m_standardItemCopy = m_standardItem->clone();
	m_isDeletable = false;

	m_lengthMode = lenAuto;
	m_standardValue = 1;
	m_legendLength = STANDARD_LENGTH;
	m_minimumValue = 0.01;
	m_scaleFactor = 1;

	m_mapping = Specific;
	QSettings setting;
	m_color = setting.value("graphics/vectorcolor", QColor(Qt::black)).value<QColor>();
	m_sampleRate = 1;

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	vtkPointData* pd = cont->data()->GetPointData();
	int number = pd->GetNumberOfArrays();
	bool first = true;
	for (int i = 0; i < number; i++) {
		if (pd->GetArray(i)->GetNumberOfComponents() == 1) {
			if (! first) { continue; }
			vtkAbstractArray* tmparray = pd->GetArray(i);
			if (tmparray == nullptr) {continue;}
			m_scalarValueName = tmparray->GetName();
			first = false;
			continue;
		}
		m_currentSolution = "";
		break;
	}
	setupActors();
}

class Post3dWindowArrowGroupSetSetting : public QUndoCommand
{
public:
	Post3dWindowArrowGroupSetSetting(
		const QString& newsol, Post3dWindowArrowGroupDataItem::LengthMode lm, double stdLen, int legendLen, double minLen, Post3dWindowArrowGroupDataItem::Mapping newMapping, const QColor& newColor, const QString& newScalar, int newRate, const ArrowSettingContainer& arrowSetting, Post3dWindowArrowGroupDataItem* g) {
		m_oldEnabled = (g->standardItem()->checkState() == Qt::Checked);
		m_oldSolution = g->m_currentSolution;
		m_oldLengthMode = g->m_lengthMode;
		m_oldStandardValue = g->m_standardValue;
		m_oldLegendLength = g->m_legendLength;
		m_oldMinimumValue = g->m_minimumValue;
		m_oldMapping = g->m_mapping;
		m_oldColor = g->m_color;
		m_oldScalar = g->m_scalarValueName;
		m_oldSampleRate = g->m_sampleRate;
		m_oldArrowSetting = g->m_arrowSetting;

		m_newEnabled = true;
		m_newSolution = newsol;
		m_newLengthMode = lm;
		m_newStandardValue = stdLen;
		m_newLegendLength = legendLen;
		m_newMinimumValue = minLen;
		m_newMapping = newMapping;
		m_newColor = newColor;
		m_newScalar = newScalar;
		m_newSampleRate = newRate;
		m_newArrowSetting = arrowSetting;

		m_group = g;
	}
	void redo() {
		m_group->m_isCommandExecuting = true;
		if (m_newEnabled) {
			m_group->standardItem()->setCheckState(Qt::Checked);
		} else {
			m_group->standardItem()->setCheckState(Qt::Unchecked);
		}
		m_group->m_currentSolution = m_newSolution;
		m_group->m_lengthMode = m_newLengthMode;
		m_group->m_standardValue = m_newStandardValue;
		m_group->m_legendLength = m_newLegendLength;
		m_group->m_minimumValue = m_newMinimumValue;
		m_group->m_mapping = m_newMapping;
		m_group->m_color = m_newColor;
		m_group->m_scalarValueName = m_newScalar;
		m_group->m_sampleRate = m_newSampleRate;
		m_group->m_arrowSetting = m_newArrowSetting;
		m_group->m_isCommandExecuting = false;
	}
	void undo() {
		m_group->m_isCommandExecuting = true;
		if (m_oldEnabled) {
			m_group->standardItem()->setCheckState(Qt::Checked);
		} else {
			m_group->standardItem()->setCheckState(Qt::Unchecked);
		}
		m_group->m_currentSolution = m_oldSolution;
		m_group->m_lengthMode = m_oldLengthMode;
		m_group->m_standardValue = m_oldStandardValue;
		m_group->m_legendLength = m_oldLegendLength;
		m_group->m_minimumValue = m_oldMinimumValue;
		m_group->m_mapping = m_oldMapping;
		m_group->m_color = m_oldColor;
		m_group->m_scalarValueName = m_oldScalar;
		m_group->m_sampleRate = m_oldSampleRate;
		m_group->m_arrowSetting = m_oldArrowSetting;
		m_group->m_isCommandExecuting = false;
	}
private:
	bool m_oldEnabled;
	QString m_oldSolution;
	Post3dWindowArrowGroupDataItem::LengthMode m_oldLengthMode;
	double m_oldStandardValue;
	int m_oldLegendLength;
	double m_oldMinimumValue;
	Post3dWindowArrowGroupDataItem::Mapping m_oldMapping;
	QColor m_oldColor;
	QString m_oldScalar;
	int m_oldSampleRate;
	ArrowSettingContainer m_oldArrowSetting;

	bool m_newEnabled;
	QString m_newSolution;
	Post3dWindowArrowGroupDataItem::LengthMode m_newLengthMode;
	double m_newStandardValue;
	double m_newMinimumValue;
	int m_newLegendLength;
	Post3dWindowArrowGroupDataItem::Mapping m_newMapping;
	QColor m_newColor;
	QString m_newScalar;
	int m_newSampleRate;
	ArrowSettingContainer m_newArrowSetting;

	Post3dWindowArrowGroupDataItem* m_group;
};

void Post3dWindowArrowGroupDataItem::setSetting(const QString& sol, LengthMode lenMode, double stdVal, int legendLen, double minVal, Mapping mapping, const QColor& color, const QString& scalar, int rate, const ArrowSettingContainer& arrowSetting)
{
	iRICUndoStack::instance().push(new Post3dWindowArrowGroupSetSetting(
																	 sol, lenMode, stdVal, legendLen, minVal, mapping, color, scalar, rate, arrowSetting, this));
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
	dialog->setCurrentSolution(m_currentSolution);
	dialog->setLengthMode(m_lengthMode);
	dialog->setStandardValue(m_standardValue);
	dialog->setLegendLength(m_legendLength);
	dialog->setMinimumValue(m_minimumValue);
	dialog->setMapping(m_mapping);
	dialog->setColor(m_color);
	dialog->setScalarValueName(m_scalarValueName);
	dialog->setSampleRate(m_sampleRate);
	dialog->setFaceMap(faceMap());
	dialog->setArrowSetting(m_arrowSetting);

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
	setSetting(dialog->currentSolution(), dialog->lengthMode(), dialog->standardValue(), dialog->legendLength(), dialog->minimumValue(), dialog->mapping(), dialog->color(), dialog->scalarValueName(), dialog->sampleRate(), dialog->arrowSetting());
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
	Post3dWindowArrowGroupSetFaceMap(const QMap<QString, Post3dWindowFaceDataItem::Setting>& oldMap, const QMap<QString, Post3dWindowFaceDataItem::Setting>& newMap, Post3dWindowArrowGroupDataItem* item) {
		m_oldMap = oldMap;
		m_newMap = newMap;
		m_item = item;
	}
	void redo() {
		removeChildren();
		for (auto it = m_newMap.begin(); it != m_newMap.end(); ++it) {
			Post3dWindowFaceDataItem* f = new Post3dWindowFaceDataItem(it.key(), m_item);
			f->setSetting(it.value(), true);
			m_item->m_childItems.append(f);
		}
		m_item->updateItemMap();
		m_item->setupAppendFilter();
	}
	void undo() {
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
		for (auto it = m_item->m_childItems.begin(); it != m_item->m_childItems.end(); ++it) {
			delete(*it);
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
	m_arrowActor->VisibilityOff();
	m_actorCollection->RemoveAllItems();
	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr) {return;}
	vtkPointSet* ps = cont->data();
	if (ps == nullptr) {return;}
	if (m_currentSolution == "") {return;}
	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0) {return;}
	m_maskPoints->SetOnRatio(m_sampleRate);

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
	switch (m_mapping) {
	case Specific:
		m_arrowMapper->ScalarVisibilityOff();
		m_arrowActor->GetProperty()->SetColor(m_color.redF(), m_color.greenF(), m_color.blueF());
		break;
	case Scalar:
		m_arrowMapper->ScalarVisibilityOn();
		LookupTableContainer* stc = typedi->lookupTable(iRIC::toStr(m_scalarValueName).c_str());
		m_arrowMapper->SetScalarModeToUsePointFieldData();
		m_arrowMapper->SelectColorArray(iRIC::toStr(m_scalarValueName).c_str());
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

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr) {return;}
	vtkPointSet* ps = cont->data();
	if (ps == nullptr) {return;}
	if (m_currentSolution == "") {return;}
	if (m_appendFilter->GetNumberOfInputConnections(0) == 0) {return;}
	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0) {return;}
	pd->SetActiveVectors(iRIC::toStr(m_currentSolution).c_str());
	updateScaleFactor();
	double height = dataModel()->graphicsView()->stdDistance(m_arrowSetting.arrowSize());
	m_hedgeHog->SetScaleFactor(m_scaleFactor);
	m_warpVector->SetScaleFactor(m_scaleFactor);
	m_arrowSource->SetHeight(height);
	m_arrowSource->SetAngle(15);
	m_arrowSource->Modified();

	m_maskPoints->Update();
	vtkPointSet* inPS = m_maskPoints->GetOutput();
	vtkDataArray* vectorArray = inPS->GetPointData()->GetArray(iRIC::toStr(m_currentSolution).c_str());
	QSet<vtkIdType> pointIds;
	double minlimitsqr = m_minimumValue * m_minimumValue;
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
	m_arrowActor->GetProperty()->SetLineWidth(m_arrowSetting.lineWidth());
	m_baseArrowActor->GetProperty()->SetLineWidth(m_arrowSetting.lineWidth());
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
	double vectorOffset = 18;
	double arrowLen = m_legendLength;
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

	QString lenStr = QString("%1\n\n%2").arg(m_currentSolution).arg(m_standardValue);
	m_legendTextActor->SetInput(iRIC::toStr(lenStr).c_str());

	if (m_mapping == Specific) {
		// specified color.
		m_baseArrowActor->GetProperty()->SetColor(m_color.red() / 255., m_color.green() / 255., m_color.blue() / 255.);
	} else if (m_mapping == Scalar) {
		// always black.
		m_baseArrowActor->GetProperty()->SetColor(0, 0, 0);
	}
}

void Post3dWindowArrowGroupDataItem::calculateStandardValue()
{
	if (m_lengthMode == lenCustom) {return;}
	QVector<double> lenVec;
	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return;}
	vtkPointSet* ps = cont->data();
	if (m_currentSolution == "") {return;}
	vtkPointData* pd = ps->GetPointData();
	vtkDataArray* da = pd->GetArray(iRIC::toStr(m_currentSolution).c_str());

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
	m_standardValue = average;
}

void Post3dWindowArrowGroupDataItem::updateScaleFactor()
{
	double a = 1.0 / dataModel()->graphicsView()->stdDistance(1.0);
	m_scaleFactor = m_legendLength / (a * m_standardValue);
}

void Post3dWindowArrowGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	m_currentSolution = elem.attribute("solution");
	m_scalarValueName = elem.attribute("scalarValueName");
	m_color = loadColorAttribute("color", node, m_color);
	m_oldCameraScale = elem.attribute("oldCameraScale").toDouble();
	m_scaleFactor = elem.attribute("scaleFactor").toDouble();
	m_mapping = static_cast<Mapping>(elem.attribute("mapping").toInt());
	m_lengthMode = static_cast<LengthMode>(elem.attribute("lengthMode").toInt());
	m_standardValue = elem.attribute("standardValue").toDouble();
	m_legendLength = elem.attribute("legendLength").toDouble();
	m_minimumValue = elem.attribute("minimumValue").toDouble();
	m_arrowSetting.load(node);

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
	writer.writeAttribute("solution", m_currentSolution);
	writer.writeAttribute("scalarValueName", m_scalarValueName);
	writeColorAttribute("color", m_color, writer);
	writer.writeAttribute("oldCameraScale", QString::number(m_oldCameraScale));
	writer.writeAttribute("scaleFactor", QString::number(m_scaleFactor));
	writer.writeAttribute("mapping", QString::number(static_cast<int>(m_mapping)));
	writer.writeAttribute("lengthMode", QString::number(static_cast<int>(m_lengthMode)));
	writer.writeAttribute("standardValue", QString::number(m_standardValue));
	writer.writeAttribute("legendLength", QString::number(m_legendLength));
	writer.writeAttribute("minimumValue", QString::number(m_minimumValue));
	m_arrowSetting.save(writer);

	for (int i = 0; i < m_childItems.count(); ++i) {
		Post3dWindowFaceDataItem* fitem = dynamic_cast<Post3dWindowFaceDataItem*>(m_childItems.at(i));
		writer.writeStartElement("FaceSetting");
		fitem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}
