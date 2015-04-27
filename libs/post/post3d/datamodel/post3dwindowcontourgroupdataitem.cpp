#include "post3dwindowcontourgroupdataitem.h"
#include "post3dwindowcontourgroupsettingdialog.h"
#include "post3dwindowfacedataitem.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guibase/graphicsmisc.h>
#include <guicore/datamodel/graphicswindowdrawcommands.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>

#include <QDomElement>
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

Post3dWindowContourGroupDataItem::Post3dWindowContourGroupDataItem(Post3dWindowDataItem* p)
	: Post3dWindowDataItem(tr("Contour"), QIcon(":/libs/guibase/images/iconFolder.png"), p)
{
	m_zScale = 1;
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);
	m_standardItemCopy = m_standardItem->clone();
	m_isDeletable = false;

	m_contour = ContourSettingWidget::ColorFringe;
	Post3dWindowZoneDataItem* zItem = dynamic_cast<Post3dWindowZoneDataItem*>(parent());
	PostZoneDataContainer* c = zItem->dataContainer();
	m_currentSolution = c->data()->GetPointData()->GetArrayName(0);
	m_fillUpper = true;
	m_fillLower = true;

	m_numberOfDivision = 10;

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	vtkPointData* pd = cont->data()->GetPointData();
	int number = pd->GetNumberOfArrays();
	for (int i = 0; i < number; i++){
		vtkAbstractArray* tmparray = pd->GetArray(i);
		if (tmparray == nullptr){
			continue;
		}
		if (tmparray->GetNumberOfComponents() > 1){
			// vector attribute.
			continue;
		}
		QString name = pd->GetArray(i)->GetName();
		m_colorBarTitleMap.insert(name, name);
	}
	m_titleTextSetting.setPrefix("titleText");
	m_labelTextSetting.setPrefix("labelText");

	setupScalarBarActor();
}

Post3dWindowContourGroupDataItem::~Post3dWindowContourGroupDataItem()
{
	m_scalarBarWidget->SetInteractor(0);
}

class Post3dWindowContourGroupSetSetting : public QUndoCommand
{
public:
	Post3dWindowContourGroupSetSetting(
			const QString& oldsol, ContourSettingWidget::Contour oldc, int oldNum, const LookupTableContainer& oldlookup, bool oldUpper, bool oldLower, const QString& oldTitle, const ScalarBarSetting& oldSetting, const vtkTextPropertySettingContainer& oldTitleSetting, const vtkTextPropertySettingContainer& oldLabelSetting,
			const QString& newsol, ContourSettingWidget::Contour newc, int newNum, const LookupTableContainer& newlookup, bool newUpper, bool newLower, const QString& newTitle, const ScalarBarSetting& newSetting, const vtkTextPropertySettingContainer& newTitleSetting, const vtkTextPropertySettingContainer& newLabelSetting,
			Post3dWindowContourGroupDataItem* g, bool draw)
	{
		m_oldSolution = oldsol;
		m_oldContour = oldc;
		m_oldNumOfDivision = oldNum;
		m_oldLookupTable = oldlookup;
		m_oldFillUpper = oldUpper;
		m_oldFillLower = oldLower;
		m_oldTitle = oldTitle;
		m_oldScalarBarSetting = oldSetting;
		m_oldTitleTextSetting = oldTitleSetting;
		m_oldLabelTextSetting = oldLabelSetting;

		m_newSolution = newsol;
		m_newContour = newc;
		m_newNumOfDivision = newNum;
		m_newLookupTable = newlookup;
		m_newFillUpper = newUpper;
		m_newFillLower = newLower;
		m_newTitle = newTitle;
		m_newScalarBarSetting = newSetting;
		m_newTitleTextSetting = newTitleSetting;
		m_newLabelTextSetting = newLabelSetting;

		m_group = g;
		m_draw = draw;
	}
	void redo()
	{
		m_group->m_currentSolution = m_newSolution;
		m_group->m_contour = m_newContour;
		m_group->m_numberOfDivision = m_newNumOfDivision;
		m_group->m_fillUpper = m_newFillUpper;
		m_group->m_fillLower = m_newFillLower;
		m_group->m_colorBarTitleMap[m_newSolution] = m_newTitle;
		m_group->m_scalarBarSetting = m_newScalarBarSetting;
		m_group->m_scalarBarSetting.saveToRepresentation(m_group->m_scalarBarWidget->GetScalarBarRepresentation());
		m_group->m_titleTextSetting = m_newTitleTextSetting;
		m_group->m_titleTextSetting.applySetting(m_group->m_scalarBarWidget->GetScalarBarActor()->GetTitleTextProperty());
		m_group->m_labelTextSetting = m_newLabelTextSetting;
		m_group->m_labelTextSetting.applySetting(m_group->m_scalarBarWidget->GetScalarBarActor()->GetLabelTextProperty());

		Post3dWindowGridTypeDataItem* gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*>(m_group->parent()->parent());
		LookupTableContainer* lookup =  gtItem->lookupTable(m_group->m_currentSolution);
		*lookup = m_newLookupTable;
		if (m_draw){
			m_group->renderGraphicsView();
		}
	}
	void undo()
	{
		m_group->m_currentSolution = m_oldSolution;
		m_group->m_contour = m_oldContour;
		m_group->m_numberOfDivision = m_oldNumOfDivision;
		m_group->m_fillUpper = m_oldFillUpper;
		m_group->m_fillLower = m_oldFillLower;
		m_group->m_colorBarTitleMap[m_newSolution] = m_oldTitle;
		m_group->m_scalarBarSetting = m_oldScalarBarSetting;
		m_group->m_scalarBarSetting.saveToRepresentation(m_group->m_scalarBarWidget->GetScalarBarRepresentation());
		m_group->m_titleTextSetting = m_oldTitleTextSetting;
		m_group->m_titleTextSetting.applySetting(m_group->m_scalarBarWidget->GetScalarBarActor()->GetTitleTextProperty());
		m_group->m_labelTextSetting = m_oldLabelTextSetting;
		m_group->m_labelTextSetting.applySetting(m_group->m_scalarBarWidget->GetScalarBarActor()->GetLabelTextProperty());

		Post3dWindowGridTypeDataItem* gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*>(m_group->parent()->parent());
		LookupTableContainer* lookup =  gtItem->lookupTable(m_group->m_currentSolution);
		*lookup = m_oldLookupTable;
		if (m_draw){
			m_group->renderGraphicsView();
		}
	}
private:
	QString m_oldSolution;
	QString m_newSolution;
	ContourSettingWidget::Contour m_oldContour;
	ContourSettingWidget::Contour m_newContour;
	int m_oldNumOfDivision;
	int m_newNumOfDivision;
	LookupTableContainer m_oldLookupTable;
	LookupTableContainer m_newLookupTable;
	bool m_oldFillUpper;
	bool m_newFillUpper;
	bool m_oldFillLower;
	bool m_newFillLower;

	// for scalar bar
	ScalarBarSetting m_oldScalarBarSetting;
	vtkTextPropertySettingContainer m_oldTitleTextSetting;
	vtkTextPropertySettingContainer m_oldLabelTextSetting;
	ScalarBarSetting m_newScalarBarSetting;
	vtkTextPropertySettingContainer m_newTitleTextSetting;
	vtkTextPropertySettingContainer m_newLabelTextSetting;

	QString m_oldTitle;
	QString m_newTitle;

	Post3dWindowContourGroupDataItem* m_group;
	bool m_draw;
};

void Post3dWindowContourGroupDataItem::setSetting(const QString& sol, ContourSettingWidget::Contour c, int numOfDiv, const LookupTableContainer& lookup, bool upper, bool lower, const QString& title, const ScalarBarSetting& setting, const vtkTextPropertySettingContainer& titleSetting, const vtkTextPropertySettingContainer& labelSetting, bool draw)
{
	Post3dWindowGridTypeDataItem* gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*>(parent()->parent());
	LookupTableContainer* oldlookup =  gtItem->lookupTable(m_currentSolution);
	iRICUndoStack::instance().push(new Post3dWindowContourGroupSetSetting(
			m_currentSolution, m_contour, m_numberOfDivision, *oldlookup, m_fillUpper, m_fillLower, m_colorBarTitleMap[sol], m_scalarBarSetting, m_titleTextSetting, m_labelTextSetting,
			sol, c, numOfDiv, lookup, upper, lower, title, setting, titleSetting, labelSetting,
			this, draw));
}

void Post3dWindowContourGroupDataItem::updateChildActors()
{
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it){
		Post3dWindowFaceDataItem* f = dynamic_cast<Post3dWindowFaceDataItem*>(*it);
		f->update();
		vtkAlgorithmOutput *algo = f->getOutputPort();
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
	switch (m_contour){
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
	contourFilter->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, iRIC::toStr(m_currentSolution).c_str());
	contourFilter->GenerateValues(m_numberOfDivision + 1, range);

	// setup mapper
	vtkDataSetMapper* mapper = vtkDataSetMapper::New();
	mapper->SetInputConnection(contourFilter->GetOutputPort());
	contourFilter->Delete();
	mapper->SetLookupTable(lookup->vtkObj());
	mapper->SetScalarModeToUsePointFieldData();
	mapper->SelectColorArray(iRIC::toStr(m_currentSolution).c_str());
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
	if (m_fillLower){
		lowerClipped->DeepCopy(rangeClippedPolyData);
		rangeClippedPolyData->Delete();
	}else{
		vtkClipPolyData* lowerClipper = vtkClipPolyData::New();
		lowerClipper->SetValue(min);
		lowerClipper->SetInputData(rangeClippedPolyData);
		lowerClipper->InsideOutOff();
		rangeClippedPolyData->GetPointData()->SetActiveScalars(iRIC::toStr(m_currentSolution).c_str());

		lowerClipper->Update();
		lowerClipped->DeepCopy(lowerClipper->GetOutput());
		lowerClipped->GetPointData()->SetActiveScalars("");
		lowerClipper->Delete();
		rangeClippedPolyData->Delete();
	}
	if (m_fillUpper){
		upperClipped->DeepCopy(lowerClipped);
		lowerClipped->Delete();
	}else{
		vtkClipPolyData* upperClipper = vtkClipPolyData::New();
		upperClipper->SetValue(max);
		upperClipper->SetInputData(lowerClipped);
		upperClipper->InsideOutOn();
		lowerClipped->GetPointData()->SetActiveScalars(iRIC::toStr(m_currentSolution).c_str());
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
	valueClippedPolyData->GetPointData()->SetActiveScalars(iRIC::toStr(m_currentSolution).c_str());
	vtkAppendPolyData* appendFilledContours = vtkAppendPolyData::New();
	double delta = (max - min) / static_cast<double>(m_numberOfDivision);
	std::vector<vtkClipPolyData*> clippersLo;
	std::vector<vtkClipPolyData*> clippersHi;

	for (int i = 0; i < m_numberOfDivision; i++){
		double valueLo = min + static_cast<double> (i) * delta;
		double valueHi = min + static_cast<double> (i + 1) * delta;
		clippersLo.push_back(vtkClipPolyData::New());
		if (i == 0){
			clippersLo[i]->SetValue(-HUGE_VAL);
			clippersLo[i]->SetInputData(valueClippedPolyData);
			valueClippedPolyData->Delete();
		}else{
			clippersLo[i]->SetValue(valueLo);
			clippersLo[i]->SetInputConnection(clippersLo[i - 1]->GetOutputPort());
		}
		clippersLo[i]->InsideOutOff();
		clippersLo[i]->Update();

		clippersHi.push_back(vtkClipPolyData::New());
		if(i < m_numberOfDivision - 1){
			clippersHi[i]->SetValue(valueHi);
		}else{
			clippersHi[i]->SetValue(HUGE_VAL);
		}
		clippersHi[i]->SetInputConnection(clippersLo[i]->GetOutputPort());
		clippersHi[i]->GenerateClippedOutputOn();
		clippersHi[i]->InsideOutOn();
		clippersHi[i]->Update();
		if(clippersHi[i]->GetOutput()->GetNumberOfCells() == 0){
			continue;
		}

		vtkDoubleArray* da = vtkDoubleArray::New();
		da->SetNumberOfComponents(1);
		da->SetNumberOfTuples(clippersHi[i]->GetOutput()->GetNumberOfCells());
		da->FillComponent(0, min + (max - min) * (i / (m_numberOfDivision - 1.0)));
		clippersHi[i]->GetOutput()->GetCellData()->SetScalars(da);
		appendFilledContours->AddInputConnection(clippersHi[i]->GetOutputPort());
		da->Delete();
	}
	for (int i = 0; i < m_numberOfDivision; i++){
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
	mapper->SelectColorArray(iRIC::toStr(m_currentSolution).c_str());
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
	if (lookup == nullptr) return;

	vtkScalarBarActor* a = m_scalarBarWidget->GetScalarBarActor();
	a->SetTitle(iRIC::toStr(m_colorBarTitleMap.value(m_currentSolution)).c_str());
	a->SetLookupTable(lookup->vtkObj());
	a->SetNumberOfLabels(m_scalarBarSetting.numberOfLabels);
	m_titleTextSetting.applySetting(a->GetTitleTextProperty());
	m_labelTextSetting.applySetting(a->GetLabelTextProperty());
	switch (m_contour) {
	case ContourSettingWidget::Points:
		// do nothing
		break;
	case ContourSettingWidget::Isolines:
		a->SetMaximumNumberOfColors(m_numberOfDivision);
		break;
	case ContourSettingWidget::ContourFigure:
		a->SetMaximumNumberOfColors(m_numberOfDivision);
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

	m_scalarBarSetting.saveToRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	updateScalarBarActorSetting();
}

void Post3dWindowContourGroupDataItem::update()
{
	updateChildActors();
}

void Post3dWindowContourGroupDataItem::updateVisibility(bool visible)
{
	PostZoneDataContainer* zoneData = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	bool dataOK = (zoneData != nullptr && zoneData->data() != nullptr);
	if (! dataOK){
		m_scalarBarWidget->SetEnabled(0);
		return;
	}

	GraphicsWindowDataItem::updateVisibility(visible);
/*
	bool childVis = false;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it){
		Post3dWindowFaceDataItem* f = dynamic_cast<Post3dWindowFaceDataItem*>(*it);
		if (f->standardItem()->checkState() == Qt::Checked){
			childVis = true;
			break;
		}
	}
*/
	bool v = (m_standardItem->checkState() == Qt::Checked) && visible;
	m_scalarBarWidget->SetEnabled(m_scalarBarSetting.visible && v);
}

void Post3dWindowContourGroupDataItem::handleStandardItemChange()
{
	GraphicsWindowDataItem::updateVisibility();
}

void Post3dWindowContourGroupDataItem::showSettingDialog()
{
	Post3dWindowContourGroupSettingDialog* dialog = dynamic_cast<Post3dWindowContourGroupSettingDialog*>(propertyDialog(mainWindow()));
	if (dialog == nullptr){return;}
	int ret = dialog->exec();
	if (ret == QDialog::Accepted){
		handlePropertyDialogAccepted(dialog);
	}
	delete dialog;
}

QDialog* Post3dWindowContourGroupDataItem::propertyDialog(QWidget* p)
{
	Post3dWindowContourGroupSettingDialog* dialog = new Post3dWindowContourGroupSettingDialog(p);
	dialog->setEnabled(true);
	Post3dWindowGridTypeDataItem* gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*>(parent()->parent());
	dialog->setGridTypeDataItem(gtItem);
	PostZoneDataContainer* zoneData = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (zoneData == nullptr || zoneData->data() == nullptr){
		delete dialog;
		return nullptr;
	}
	dialog->setZoneData(zoneData);
	dialog->setCurrentSolution(m_currentSolution);
	dialog->setContour(m_contour);
	dialog->setNumberOfDivision(m_numberOfDivision);
	dialog->setLookupTable(*(lookupTable()));
	dialog->setFillUpper(m_fillUpper);
	dialog->setFillLower(m_fillLower);

	dialog->setFaceMap(faceMap());

	// for scalar bar setting
	dialog->setColorBarTitleMap(m_colorBarTitleMap);
	m_scalarBarSetting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	dialog->setScalarBarSetting(m_scalarBarSetting);
	dialog->setTitleTextSetting(m_titleTextSetting);
	dialog->setLabelTextSetting(m_labelTextSetting);

	return dialog;
}

void Post3dWindowContourGroupDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	Post3dWindowContourGroupSettingDialog* dialog = dynamic_cast<Post3dWindowContourGroupSettingDialog*>(propDialog);
	iRICUndoStack& stack = iRICUndoStack::instance();
	stack.beginMacro(tr("Contour Setting"));
	stack.push(new GraphicsWindowDrawOnUndo(this));
	setSetting(dialog->currentSolution(), dialog->contour(), dialog->numberOfDivision(), dialog->lookupTable(), dialog->fillUpper(), dialog->fillLower(), dialog->scalarBarTitle(), dialog->scalarBarSetting(), dialog->titleTextSetting(), dialog->labelTextSetting(), false);
	setFaceMap(dialog->faceMap());
	stack.push(new GraphicsWindowDrawOnRedo(this));
	stack.endMacro();
	// temporary this command is not undo-able
	stack.clear();
}

LookupTableContainer* Post3dWindowContourGroupDataItem::lookupTable()
{
	Post3dWindowGridTypeDataItem* gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*>(parent()->parent());
	return gtItem->lookupTable(m_currentSolution);
}

QMap<QString, Post3dWindowFaceDataItem::Setting> Post3dWindowContourGroupDataItem::faceMap()
{
	QMap<QString, Post3dWindowFaceDataItem::Setting> map;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it){
		Post3dWindowFaceDataItem* fItem = dynamic_cast<Post3dWindowFaceDataItem*>(*it);
		map.insert(fItem->standardItem()->text(), fItem->setting());
	}
	return map;
}

class Post3dWindowContourGroupSetFaceMap : public QUndoCommand
{
public:
	Post3dWindowContourGroupSetFaceMap(const QMap<QString, Post3dWindowFaceDataItem::Setting>& oldMap, const QMap<QString, Post3dWindowFaceDataItem::Setting>& newMap, Post3dWindowContourGroupDataItem* item)
	{
		m_oldMap = oldMap;
		m_newMap = newMap;
		m_item = item;
	}
	void redo(){
		removeChildren();
		for (auto it = m_newMap.begin(); it != m_newMap.end(); ++it){
			Post3dWindowFaceDataItem* f = new Post3dWindowFaceDataItem(it.key(), m_item);
			f->setSetting(it.value(), true);
			m_item->m_childItems.append(f);
		}
		m_item->updateItemMap();
		m_item->updateChildActors();
	}
	void undo(){
		removeChildren();
		for (auto it = m_oldMap.begin(); it != m_oldMap.end(); ++it){
			Post3dWindowFaceDataItem* f = new Post3dWindowFaceDataItem(it.key(), m_item);
			f->setSetting(it.value(), true);
			m_item->m_childItems.append(f);
		}
		m_item->updateItemMap();
		m_item->updateChildActors();
	}
private:
	void removeChildren()
	{
		for (auto it = m_item->m_childItems.begin(); it != m_item->m_childItems.end(); ++it){
			delete (*it);
		}
		m_item->updateItemMap();
	}
	QMap<QString, Post3dWindowFaceDataItem::Setting> m_oldMap;
	QMap<QString, Post3dWindowFaceDataItem::Setting> m_newMap;
	Post3dWindowContourGroupDataItem* m_item;
};

void Post3dWindowContourGroupDataItem::setFaceMap(const QMap<QString, Post3dWindowFaceDataItem::Setting>& map)
{
	iRICUndoStack::instance().push(new Post3dWindowContourGroupSetFaceMap(faceMap(), map, this));
}

void Post3dWindowContourGroupDataItem::innerUpdateZScale(double scale)
{
	m_zScale = scale;
}

void Post3dWindowContourGroupDataItem::informSelection(VTKGraphicsView * /*v*/)
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
	QDomElement elem = node.toElement();
	m_currentSolution = elem.attribute("solution");
	m_fillUpper = static_cast<bool>(elem.attribute("fillUpper").toInt());
	m_fillLower = static_cast<bool>(elem.attribute("fillLower").toInt());
	m_contour = static_cast<ContourSettingWidget::Contour>(elem.attribute("contour").toInt());

	m_scalarBarSetting.load(node);
	m_scalarBarSetting.saveToRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	m_titleTextSetting.load(node);
	m_labelTextSetting.load(node);

	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i){
		QDomElement childElem = children.at(i).toElement();
		if (childElem.nodeName() == "ScalarBarTitle"){
			QString val = childElem.attribute("value");
			QString title = childElem.attribute("title");
			m_colorBarTitleMap[val] = title;
		} else if (childElem.nodeName() == "FaceSetting"){
			QString label = childElem.attribute("label");
			Post3dWindowFaceDataItem* item = new Post3dWindowFaceDataItem(label, this);
			item->loadFromProjectMainFile(childElem);
			m_childItems.append(item);
		}
	}
	updateChildActors();
	updateScalarBarActorSetting();
}

void Post3dWindowContourGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("solution", m_currentSolution);
	writer.writeAttribute("fillUpper", QString::number(static_cast<int>(m_fillUpper)));
	writer.writeAttribute("fillLower", QString::number(static_cast<int>(m_fillLower)));
	writer.writeAttribute("contour", QString::number(static_cast<int>(m_contour)));

	m_scalarBarSetting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	m_scalarBarSetting.save(writer);
	m_titleTextSetting.save(writer);
	m_labelTextSetting.save(writer);

	// scalar bar titles
	QMapIterator<QString, QString> i(m_colorBarTitleMap);
	while (i.hasNext()){
		i.next();
		writer.writeStartElement("ScalarBarTitle");
		writer.writeAttribute("value", i.key());
		writer.writeAttribute("title", i.value());
		writer.writeEndElement();
	}
	for (int i = 0; i < m_childItems.count(); ++i){
		Post3dWindowFaceDataItem* fitem = dynamic_cast<Post3dWindowFaceDataItem*> (m_childItems.at(i));
		writer.writeStartElement("FaceSetting");
		fitem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}
