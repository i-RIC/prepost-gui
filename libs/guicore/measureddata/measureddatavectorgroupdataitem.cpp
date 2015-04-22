#include "measureddatavectorgroupdataitem.h"
#include "measureddatavectordataitem.h"
#include "measureddatafiledataitem.h"
#include "../datamodel/graphicswindowdatamodel.h"
#include <misc/stringtool.h>
#include "../scalarstocolors/lookuptablecontainer.h"
#include "../datamodel/vtk2dgraphicsview.h"
#include "measureddatafiledataitem.h"
#include "measureddatapointgroupdataitem.h"
#include "measureddatavectorsettingdialog.h"

#include <misc/iricundostack.h>

#include <cmath>

#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkTextProperty.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkLine.h>
#include <vtkTriangle.h>
#include <vtkProperty2D.h>
#include <vtkGeometryFilter.h>
#include <vtkActor2DCollection.h>
#include <vtkDoubleArray.h>
#include <vtkVertex.h>

#include <QSettings>
#include <QVector2D>
#include <QDomElement>
#include <QSet>

const double MeasuredDataVectorGroupDataItem::MINLIMIT = 1.0E-6;

MeasuredDataVectorGroupDataItem::MeasuredDataVectorGroupDataItem(GraphicsWindowDataItem *p)
	: GraphicsWindowDataItem(tr("Arrow"), QIcon(":/libs/guibase/images/iconFolder.png"), p)
{
	m_isDeletable = false;
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();

	m_oldCameraScale = 1;
	m_colorMode = MeasuredData::acmSpecific;
	m_lengthMode = MeasuredData::almAuto;
	m_standardValue = 1;
	m_legendLength = STANDARD_LENGTH;
	m_minimumValue = 0.001;

	m_scalarValueName = "";
	m_currentSolution = "";
	QSettings setting;
	m_color = setting.value("graphics/vectorcolor", QColor(Qt::black)).value<QColor>();
	m_scaleFactor = setting.value("graphics/vectorfactor", 1).value<double>();

	MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(parent())->measuredData();

	for (int i = 0; i < md->vectorNames().count(); ++i){
		QString name = md->vectorNames().at(i);
		MeasuredDataVectorDataItem* item = new MeasuredDataVectorDataItem(name, name, this);
		m_childItems.append(item);
	}
	setupActors();
	if (md->vectorNames().count() > 0){
		QString name = md->vectorNames().at(0);
		setCurrentSolution(name);
		updateActorSettings();
	}
}

MeasuredDataVectorGroupDataItem::~MeasuredDataVectorGroupDataItem()
{
	renderer()->RemoveActor(m_arrowActor);
}

void MeasuredDataVectorGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	setCurrentSolution(elem.attribute("solution"));
	QString colorMode = elem.attribute("colorMode");
	if (colorMode == "scalar") {
		m_colorMode = MeasuredData::acmScalar;
	} else {
		m_colorMode = MeasuredData::acmSpecific;
	}
	m_color = loadColorAttribute("color", node, Qt::black);
	m_scalarValueName = elem.attribute("scalarValue");

	QString lengthMode = elem.attribute("lengthMode");
	if (lengthMode == "custom"){
		m_lengthMode = MeasuredData::almCustom;
	} else {
		m_lengthMode = MeasuredData::almAuto;
	}
	if (m_lengthMode == MeasuredData::almCustom){
		m_standardValue = elem.attribute("standardValue").toDouble();
		m_legendLength = elem.attribute("legendLength").toDouble();
		m_minimumValue = elem.attribute("minimumValue").toDouble();
	}
	updateActorSettings();
}

void MeasuredDataVectorGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("solution", m_currentSolution);
	if (m_colorMode == MeasuredData::acmScalar){
		writer.writeAttribute("colorMode", "scalar");
	} else {
		writer.writeAttribute("colorMode", "specific");
	}
	writeColorAttribute("color", m_color, writer);
	writer.writeAttribute("scalarValue", m_scalarValueName);

	if (m_lengthMode == MeasuredData::almCustom){
		writer.writeAttribute("lengthMode", "custom");

		writer.writeAttribute("standardValue", QString::number(m_standardValue));
		writer.writeAttribute("legendLength", QString::number(m_legendLength));
		writer.writeAttribute("minimumValue", QString::number(m_minimumValue));
	} else {
		writer.writeAttribute("lengthMode", "auto");
	}
}

class MeasuredDataVectorSelectValue : public QUndoCommand
{
public:
	MeasuredDataVectorSelectValue(const QString& newsol, MeasuredDataVectorGroupDataItem* item)
		: QUndoCommand(QObject::tr("Arrow Physical Value Change"))
	{
		m_newCurrentSolution = newsol;
		m_oldCurrentSolution = item->m_currentSolution;
		m_item = item;
	}
	void undo()
	{
		m_item->setIsCommandExecuting(true);
		m_item->setCurrentSolution(m_oldCurrentSolution);
		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
	void redo()
	{
		m_item->setIsCommandExecuting(true);
		m_item->setCurrentSolution(m_newCurrentSolution);
		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
private:
	QString m_oldCurrentSolution;
	QString m_newCurrentSolution;

	MeasuredDataVectorGroupDataItem* m_item;
};

void MeasuredDataVectorGroupDataItem::exclusivelyCheck(MeasuredDataVectorDataItem* item)
{
	if (m_isCommandExecuting){return;}
	iRICUndoStack& stack = iRICUndoStack::instance();
	if (item->standardItem()->checkState() != Qt::Checked){
		stack.push(new MeasuredDataVectorSelectValue("", this));
	}else{
		stack.push(new MeasuredDataVectorSelectValue(item->name(), this));
	}
}

void MeasuredDataVectorGroupDataItem::setupActors(){
	m_arrowActor = vtkSmartPointer<vtkActor>::New();
	renderer()->AddActor(m_arrowActor);
	m_arrowActor->GetProperty()->LightingOff();

	m_arrowMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_arrowActor->SetMapper(m_arrowMapper);

	m_hedgeHog = vtkSmartPointer<vtkHedgeHog>::New();
	m_hedgeHog->SetVectorModeToUseVector();
	m_hedgeHog->SetScaleFactor(m_scaleFactor);

	m_warpVector = vtkSmartPointer<vtkWarpVector>::New();

	m_arrowGlyph = vtkSmartPointer<vtkGlyph3D>::New();
	m_arrowGlyph->SetScaleModeToDataScalingOff();
	m_arrowGlyph->SetVectorModeToUseVector();
	m_arrowGlyph->SetInputConnection(m_warpVector->GetOutputPort());

	m_arrowSource = vtkSmartPointer<vtkConeSource>::New();
	m_arrowGlyph->SetSourceData(m_arrowSource->GetOutput());

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

	renderer()->AddActor2D(m_baseArrowActor);
}

void MeasuredDataVectorGroupDataItem::calculateStandardValue()
{
	if (m_lengthMode == MeasuredData::almCustom){return;}
	QVector<double> lenVec;
	MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(parent())->measuredData();
	if (md == 0 || md->vectorNames().count() == 0){return;}
	vtkPointSet* ps = md->pointData();
	if (m_currentSolution == ""){return;}
	vtkPointData* pd = ps->GetPointData();
	vtkDataArray* da = pd->GetArray(iRIC::toStr(m_currentSolution).c_str());
	for (vtkIdType i = 0; i < da->GetNumberOfTuples(); ++i){
		double* v = da->GetTuple3(i);
		QVector2D vec(*(v), *(v + 1));
		lenVec.append(vec.length());
	}
	qSort(lenVec);
	double sum = 0;
	int count = AUTO_AVERAGECOUNT;
	if (count > lenVec.count()){count = lenVec.count();}
	for (int i = 0; i < count; ++i){
		sum += lenVec.at(lenVec.count() - i - 1);
	}
	double average = sum / count;
	if (average == 0){average = 1;}

	int p = 0;
	double p2 = 1;
	while (average > 10){
		average /= 10.;
		++ p;
		p2 = 10;
	}
	while (average < 1){
		average *= 10;
		++ p;
		p2 = 0.1;
	}
	average = static_cast<int>(average);
	for (int i = 0; i < p; ++i){
		average *= p2;
	}
	// now average is calculated.
	m_standardValue = average;
	// minimum value is always 0.001 * standard value when auto mode.
	m_minimumValue = 0.001 * m_standardValue;
}

void MeasuredDataVectorGroupDataItem::informGridUpdate()
{
	updateActorSettings();
}

void MeasuredDataVectorGroupDataItem::updateActorSettings()
{
	m_arrowActor->VisibilityOff();
	m_legendTextActor->VisibilityOff();
	m_baseArrowActor->VisibilityOff();

	m_actorCollection->RemoveAllItems();
	m_actor2DCollection->RemoveAllItems();

	MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(parent())->measuredData();
	if (md == 0 || md->pointData() == 0){return;}
	if (m_currentSolution == ""){return;}
	vtkPointSet* ps = getPointSet();
	if (m_currentSolution == ""){return;}
	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0){return;}

	pd->SetActiveVectors(iRIC::toStr(m_currentSolution).c_str());
	m_hedgeHog->SetInputData(ps);
	m_warpVector->SetInputData(ps);

	calculateStandardValue();
	updateColorSetting();
	updatePolyData();
	updateLegendData();

	m_actorCollection->AddItem(m_arrowActor);
	m_actor2DCollection->AddItem(m_legendTextActor);
	m_actor2DCollection->AddItem(m_baseArrowActor);
	updateVisibilityWithoutRendering();
}

void MeasuredDataVectorGroupDataItem::updateColorSetting()
{
		MeasuredDataFileDataItem* fdi = dynamic_cast<MeasuredDataFileDataItem*> (parent());
		MeasuredDataPointGroupDataItem* pgdi = fdi->pointGroupDataItem();
	switch (m_colorMode){
	case MeasuredData::acmSpecific:
		m_arrowMapper->ScalarVisibilityOff();
		m_arrowActor->GetProperty()->SetColor(m_color.redF(), m_color.greenF(), m_color.blueF());
		break;
	case MeasuredData::acmScalar:
		// not implemented yet.
				m_arrowMapper->ScalarVisibilityOn();
				LookupTableContainer* stc = pgdi->lookupTable(m_scalarValueName);
				m_arrowMapper->SetScalarModeToUsePointFieldData();
				m_arrowMapper->SelectColorArray(iRIC::toStr(m_scalarValueName).c_str());
				m_arrowMapper->SetLookupTable(stc->vtkObj());
				m_arrowMapper->UseLookupTableScalarRangeOn();
		break;
	}
}

void MeasuredDataVectorGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void MeasuredDataVectorGroupDataItem::assignActionZValues(const ZDepthRange& range)
{
	m_arrowActor->SetPosition(0, 0, range.max());
}

void MeasuredDataVectorGroupDataItem::update()
{
	informGridUpdate();
}

void MeasuredDataVectorGroupDataItem::setCurrentSolution(const QString& currentSol)
{
	MeasuredDataVectorDataItem* current = 0;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it){
		MeasuredDataVectorDataItem* tmpItem = dynamic_cast<MeasuredDataVectorDataItem*>(*it);
		if (tmpItem->name() == currentSol){
			current = tmpItem;
		}
		tmpItem->standardItem()->setCheckState(Qt::Unchecked);
	}
	if (current != 0){
		current->standardItem()->setCheckState(Qt::Checked);
	}
	m_currentSolution = currentSol;
}

void MeasuredDataVectorGroupDataItem::innerUpdate2Ds()
{
	vtkCamera* cam = renderer()->GetActiveCamera();
	double scale = cam->GetParallelScale();
	if (scale != m_oldCameraScale){
		updatePolyData();
		updateLegendData();
	}
	m_oldCameraScale = scale;
}

void MeasuredDataVectorGroupDataItem::updatePolyData()
{
	MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(parent())->measuredData();
	if (md == 0 || md->pointData() == 0){return;}
	if (m_currentSolution == ""){return;}
	updateScaleFactor();
	VTKGraphicsView* view = dataModel()->graphicsView();
	VTK2DGraphicsView* view2 = dynamic_cast<VTK2DGraphicsView*>(view);

	double height = view2->stdRadius(8);
	m_hedgeHog->SetScaleFactor(m_scaleFactor);
	m_warpVector->SetScaleFactor(m_scaleFactor);
	m_arrowSource->SetHeight(height);
	m_arrowSource->SetAngle(15);
	m_arrowSource->Modified();

	m_appendPolyData->Update();
	m_polyData->DeepCopy(m_appendPolyData->GetOutput());
}

void MeasuredDataVectorGroupDataItem::updateScaleFactor()
{
	VTKGraphicsView* view = dataModel()->graphicsView();
	VTK2DGraphicsView* view2 = dynamic_cast<VTK2DGraphicsView*>(view);
	double a = 1.0 / view2->stdRadius(1.0);
	m_scaleFactor = m_legendLength / (a * m_standardValue);
}

void MeasuredDataVectorGroupDataItem::updateLegendData()
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

	if (m_colorMode == MeasuredData::acmSpecific){
		// specified color.
		m_baseArrowActor->GetProperty()->SetColor(m_color.red() / 255., m_color.green() / 255., m_color.blue() / 255.);
	} else if (m_colorMode == MeasuredData::acmScalar){
		// always black.
		m_baseArrowActor->GetProperty()->SetColor(0, 0, 0);
	}
}

QDialog* MeasuredDataVectorGroupDataItem::propertyDialog(QWidget* p)
{
	MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(parent())->measuredData();
	if (md == 0 || md->pointData() == 0){
		return 0;
	}
	if (md->vectorNames().size() == 0){
		return 0;
	}
	MeasuredDataVectorSettingDialog* dialog = new MeasuredDataVectorSettingDialog(p);
	dialog->setData(md);
	dialog->setSolution(m_currentSolution);
	dialog->setColorMode(m_colorMode);
	dialog->setColor(m_color);
	dialog->setScalarValue(m_scalarValueName);
	dialog->setLengthMode(m_lengthMode);
	dialog->setStandardValue(m_standardValue);
	dialog->setLegendLength(m_legendLength);
	dialog->setMinimumValue(m_minimumValue);

	return dialog;
}

class MeasuredDataVectorSetProperty : public QUndoCommand
{
public:
	MeasuredDataVectorSetProperty(const QString& solutionName, MeasuredData::ArrowColorMode colorMode, const QColor& color, const QString& scalarName, MeasuredData::ArrowLengthMode lm, double stdLen, int legendLen, double minVal, MeasuredDataVectorGroupDataItem* item)
		: QUndoCommand(QObject::tr("Update Arrow Setting"))
	{
		m_newSolutionName = solutionName;
		m_newColorMode = colorMode;
		m_newColor = color;
		m_newScalarName = scalarName;
		m_newLengthMode = lm;
		m_newStandardValue = stdLen;
		m_newLegendLength = legendLen;
		m_newMinimumValue = minVal;

		m_oldSolutionName = item->m_currentSolution;
		m_oldColorMode = item->m_colorMode;
		m_oldColor = item->m_color;
		m_oldScalarName = item->m_scalarValueName;
		m_oldLengthMode = item->m_lengthMode;
		m_oldStandardValue = item->m_standardValue;
		m_oldLegendLength = item->m_legendLength;
		m_oldMinimumValue = item->m_minimumValue;

		m_item = item;
	}
	void redo()
	{
		m_item->setIsCommandExecuting(true);
		m_item->setCurrentSolution(m_newSolutionName);
		m_item->m_colorMode = m_newColorMode;
		m_item->m_color = m_newColor;
		m_item->m_scalarValueName = m_newScalarName;
		m_item->m_lengthMode = m_newLengthMode;
		m_item->m_standardValue = m_newStandardValue;
		m_item->m_legendLength = m_newLegendLength;
		m_item->m_minimumValue = m_newMinimumValue;

		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
	void undo()
	{
		m_item->setIsCommandExecuting(true);
		m_item->setCurrentSolution(m_oldSolutionName);
		m_item->m_colorMode = m_oldColorMode;
		m_item->m_color = m_oldColor;
		m_item->m_scalarValueName = m_oldScalarName;
		m_item->m_lengthMode = m_oldLengthMode;
		m_item->m_standardValue = m_oldStandardValue;
		m_item->m_legendLength = m_oldLegendLength;
		m_item->m_minimumValue = m_oldMinimumValue;

		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
private:
	QString m_newSolutionName;
	MeasuredData::ArrowColorMode m_newColorMode;
	QColor m_newColor;
	QString m_newScalarName;
	MeasuredData::ArrowLengthMode m_newLengthMode;
	double m_newStandardValue;
	int m_newLegendLength;
	double m_newMinimumValue;

	QString m_oldSolutionName;
	MeasuredData::ArrowColorMode m_oldColorMode;
	QColor m_oldColor;
	QString m_oldScalarName;
	MeasuredData::ArrowLengthMode m_oldLengthMode;
	double m_oldStandardValue;
	int m_oldLegendLength;
	double m_oldMinimumValue;

	MeasuredDataVectorGroupDataItem* m_item;
};

void MeasuredDataVectorGroupDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	MeasuredDataVectorSettingDialog* dialog = dynamic_cast<MeasuredDataVectorSettingDialog*>(propDialog);
	iRICUndoStack::instance().push(new MeasuredDataVectorSetProperty(dialog->solution(), dialog->colorMode(), dialog->color(), dialog->scalarValue(), dialog->lengthMode(), dialog->standardValue(), dialog->legendLength(), dialog->minimumValue(), this));
}

vtkPointSet* MeasuredDataVectorGroupDataItem::getPointSet()
{
		MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(parent())->measuredData();
		vtkPointSet* ps = md->polyData();

		vtkDoubleArray* vectorArray = vtkDoubleArray::SafeDownCast(ps->GetPointData()->GetArray(iRIC::toStr(m_currentSolution).c_str()));
		QSet<vtkIdType> points;
		double minlimitsqr = m_minimumValue * m_minimumValue;
		for (vtkIdType i = 0; i < ps->GetNumberOfPoints(); ++i){
				bool active = true;
				double val = 0;
				for (int j = 0; j < vectorArray->GetNumberOfComponents(); ++j){
						double tmpval = vectorArray->GetComponent(i, j);
						val += tmpval * tmpval;
				}
				if (val < minlimitsqr){
						active = false;
				}
				if (active){
						points.insert(i);
				}
		}

		m_activePoints = vtkSmartPointer<vtkUnstructuredGrid>::New();
		vtkPointData* inPD = ps->GetPointData();
		vtkPointData* outPD = m_activePoints->GetPointData();
		vtkPoints* inPoints = ps->GetPoints();
		vtkSmartPointer<vtkPoints> outPoints = vtkSmartPointer<vtkPoints>::New();

		outPoints->SetDataTypeToDouble();
		outPD->CopyAllocate(inPD, points.size());
		int newId = 0;
		for (auto it = points.begin(); it != points.end(); ++it){
				vtkIdType pointid = *it;
				outPoints->InsertNextPoint(inPoints->GetPoint(pointid));
				outPD->CopyData(inPD, pointid, newId);
				vtkVertex* vertex = vtkVertex::New();
				vertex->GetPointIds()->SetId(0, newId);
				m_activePoints->InsertNextCell(vertex->GetCellType(), vertex->GetPointIds());
				vertex->Delete();
				++ newId;
		}

		m_activePoints->SetPoints(outPoints);
		m_activePoints->BuildLinks();
		m_activePoints->Modified();
		return m_activePoints;
}

void MeasuredDataVectorGroupDataItem::doApplyOffset(double /*x*/, double /*y*/)
{
	this->updateActorSettings();
}
