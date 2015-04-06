#include "post2dbirdeyewindownodescalargroupdataitem.h"
#include "post2dbirdeyewindownodescalardataitem.h"
#include "post2dbirdeyewindowgridtypedataitem.h"
#include "post2dbirdeyewindowzonedataitem.h"
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridrelatedcondition.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/xmlsupport.h>
#include <misc/stringtool.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include "../post2dbirdeyewindowdatamodel.h"
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <post/post2d/datamodel/post2dwindowcontoursettingdialog.h>
#include <misc/iricundostack.h>
#include <guibase/graphicsmisc.h>
#include <guicore/datamodel/vtkgraphicsview.h>

#include <QList>
#include <QStandardItem>
#include <QXmlStreamWriter>
#include <QDomNode>
#include <QUndoCommand>

#include <vtkActorCollection.h>
#include <vtkActor2DCollection.h>
#include <vtkLODActor.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataMapper.h>
#include <vtkContourFilter.h>
#include <vtkPointData.h>
#include <vtkAppendPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkDoubleArray.h>
#include <vtkCellData.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkScalarBarActor.h>
#include <vtkTextProperty.h>
#include <vtkGeometryFilter.h>
#include <vtkStringArray.h>

Post2dBirdEyeWindowNodeScalarGroupDataItem::Post2dBirdEyeWindowNodeScalarGroupDataItem(Post2dBirdEyeWindowDataItem* p)
	: Post2dBirdEyeWindowDataItem(tr("Contour"), QIcon(":/libs/guibase/images/iconFolder.png"), p)
{
	m_isDeletable = false;

	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();

	setDefaultValues();
	setupActors();
	PostZoneDataContainer* cont = dynamic_cast<Post2dBirdEyeWindowZoneDataItem*>(parent())->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();
	vtkPointData* pd = cont->data()->GetPointData();
	int number = pd->GetNumberOfArrays();
	for (int i = 0; i < number; i++){
		vtkAbstractArray* tmparray = pd->GetArray(i);
		if (tmparray == 0){
			continue;
		}
		if (tmparray->GetNumberOfComponents() > 1){
			// vector attribute.
			continue;
		}
		QString name = pd->GetArray(i)->GetName();
		Post2dBirdEyeWindowNodeScalarDataItem* item = new Post2dBirdEyeWindowNodeScalarDataItem(name, gt->solutionCaption(name), this);
		m_childItems.append(item);
		m_colorbarTitleMap.insert(name, name);
	}
	m_titleTextSetting.setPrefix("titleText");
	m_labelTextSetting.setPrefix("labelText");
}

Post2dBirdEyeWindowNodeScalarGroupDataItem::~Post2dBirdEyeWindowNodeScalarGroupDataItem()
{
	vtkRenderer* r = renderer();
	r->RemoveActor(m_isolineActor);
	r->RemoveActor(m_contourActor);
	r->RemoveActor(m_fringeActor);
	m_scalarBarWidget->SetInteractor(0);
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::setDefaultValues()
{
	m_numberOfDivisions = DEFAULT_NUMOFDIV;
	m_contour = ContourSettingWidget::ColorFringe;
	m_regionMode = StructuredGridRegion::rmFull;
	m_fillUpper = true;
	m_fillLower = true;

	m_opacityPercent = 0;
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::updateActorSettings()
{
	// make all the items invisible
	m_isolineActor->VisibilityOff();
	m_contourActor->VisibilityOff();
	m_fringeActor->VisibilityOff();
	m_scalarBarWidget->SetEnabled(0);
	m_actorCollection->RemoveAllItems();
	m_actor2DCollection->RemoveAllItems();
	PostZoneDataContainer* cont = dynamic_cast<Post2dBirdEyeWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == 0 || cont->data() == 0){return;}
	vtkPointSet* ps = cont->data();
	if (m_currentSolution == ""){return;}

	// update current active scalar
	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0){return;}

	m_warp->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, iRIC::toStr(cont->elevationName()).c_str());

	createRangeClippedPolyData();
	createValueClippedPolyData();
	switch(m_contour)
	{
	case ContourSettingWidget::Isolines:
		setupIsolineSetting();
		break;
	case ContourSettingWidget::ContourFigure:
		setupColorContourSetting();
		break;
	case ContourSettingWidget::ColorFringe:
		setupColorFringeSetting();
		break;
	}
	if (m_scalarBarSetting.visible){
		m_scalarBarWidget->SetEnabled(1);
		setupScalarBarSetting();
	}
	updateVisibilityWithoutRendering();
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	setCurrentSolution(elem.attribute("solution"));
	m_numberOfDivisions = elem.attribute("numberOfDivisions").toInt();
	m_fillUpper = static_cast<bool>(elem.attribute("fillUpper").toInt());
	m_fillLower = static_cast<bool>(elem.attribute("fillLower").toInt());
	m_contour = static_cast<ContourSettingWidget::Contour>(elem.attribute("contour").toInt());
	m_opacityPercent = loadOpacityPercent(elem);

	m_regionMode = static_cast<StructuredGridRegion::RegionMode>(elem.attribute("regionMode").toInt());
	m_range.iMin = elem.attribute("regionIMin").toInt();
	m_range.iMax = elem.attribute("regionIMax").toInt();
	m_range.jMin = elem.attribute("regionJMin").toInt();
	m_range.jMax = elem.attribute("regionJMax").toInt();

	m_scalarBarSetting.load(node);
	m_scalarBarSetting.saveToRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	m_titleTextSetting.load(node);
	m_labelTextSetting.load(node);

	QDomNodeList titles = node.childNodes();
	for (int i = 0; i < titles.count(); ++i){
		QDomElement titleElem = titles.at(i).toElement();
		QString val = titleElem.attribute("value");
		QString title = titleElem.attribute("title");
		m_colorbarTitleMap[val] = title;
	}
	updateActorSettings();
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("solution", m_currentSolution);
	writer.writeAttribute("numberOfDivisions", QString::number(m_numberOfDivisions));
	writer.writeAttribute("fillUpper", QString::number(static_cast<int>(m_fillUpper)));
	writer.writeAttribute("fillLower", QString::number(static_cast<int>(m_fillLower)));
	writer.writeAttribute("contour", QString::number(static_cast<int>(m_contour)));
	writeOpacityPercent(m_opacityPercent, writer);

	// region setting
	writer.writeAttribute("regionMode", QString::number(static_cast<int>(m_regionMode)));
	writer.writeAttribute("regionIMin", QString::number(m_range.iMin));
	writer.writeAttribute("regionIMax", QString::number(m_range.iMax));
	writer.writeAttribute("regionJMin", QString::number(m_range.iMin));
	writer.writeAttribute("regionJMax", QString::number(m_range.jMax));

	m_scalarBarSetting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	m_scalarBarSetting.save(writer);
	m_titleTextSetting.save(writer);
	m_labelTextSetting.save(writer);

	// scalar bar titles
	QMapIterator<QString, QString> i(m_colorbarTitleMap);
	while (i.hasNext()){
		i.next();
		writer.writeStartElement("ScalarBarTitle");
		writer.writeAttribute("value", i.key());
		writer.writeAttribute("title", i.value());
		writer.writeEndElement();
	}
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::setupActors()
{
	m_warp = vtkSmartPointer<vtkWarpScalar>::New();
	m_warp->SetUseNormal(1);
	m_warp->SetNormal(0, 0, 1);
	m_warp->SetScaleFactor(1);

	m_isolineActor = vtkActor::New();
	m_isolineActor->GetProperty()->SetLighting(true);
	m_isolineActor->GetProperty()->SetLineWidth(1);
	renderer()->AddActor(m_isolineActor);

	m_isolineMapper = vtkPolyDataMapper::New();
	m_isolineMapper->UseLookupTableScalarRangeOn();
	m_isolineMapper->SetScalarModeToUsePointFieldData();
	m_isolineActor->SetMapper(m_isolineMapper);

	m_isolineFilter = vtkContourFilter::New();
	m_isolineMapper->SetInputConnection(m_isolineFilter->GetOutputPort());

	m_contourActor = vtkLODActor::New();
	// Draw 5000 points from grid vertices.
	m_contourActor->SetNumberOfCloudPoints(5000);
	// Make the point size a little big.
	m_contourActor->GetProperty()->SetPointSize(2);
	m_contourActor->GetProperty()->SetLighting(true);
	renderer()->AddActor(m_contourActor);

	m_contourMapper = vtkDataSetMapper::New();
	m_contourMapper->SetScalarVisibility(true);
	m_contourActor->SetMapper(m_contourMapper);

	m_regionClippedPolyData = vtkSmartPointer<vtkPolyData>::New();
	m_valueClippedPolyData = vtkSmartPointer<vtkPolyData>::New();
	m_colorContourPolyData = vtkSmartPointer<vtkPolyData>::New();

	m_fringeActor = vtkLODActor::New();
	m_fringeActor->SetNumberOfCloudPoints(5000);
	m_fringeActor->GetProperty()->SetPointSize(2);
	m_fringeActor->GetProperty()->SetLighting(true);
	renderer()->AddActor(m_fringeActor);

	m_fringeMapper = vtkDataSetMapper::New();
	m_fringeMapper->SetScalarVisibility(true);
	m_fringeActor->SetMapper(m_fringeMapper);

	vtkRenderWindowInteractor* iren = renderer()->GetRenderWindow()->GetInteractor();

	m_scalarBarWidget = vtkSmartPointer<vtkScalarBarWidget>::New();
	iRIC::setupScalarBarProperty(m_scalarBarWidget->GetScalarBarActor());
	m_scalarBarWidget->SetEnabled(0);
	m_scalarBarWidget->SetInteractor(iren);

	m_isolineActor->VisibilityOff();
	m_contourActor->VisibilityOff();
	m_fringeActor->VisibilityOff();
	m_scalarBarSetting.saveToRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());

	updateActorSettings();
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::update()
{
	updateActorSettings();
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::setupIsolineSetting()
{
	Post2dBirdEyeWindowGridTypeDataItem* typedi = dynamic_cast<Post2dBirdEyeWindowGridTypeDataItem*>(parent()->parent());
	LookupTableContainer* stc = typedi->lookupTable(m_currentSolution);
	if (stc == 0){return;}
	double range[2];
	stc->getValueRange(&range[0], &range[1]);

	m_warp->SetInputData(m_valueClippedPolyData);
	m_warp->Update();
	m_isolineFilter->SetInputData(m_warp->GetOutput());
	m_isolineFilter->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, iRIC::toStr(m_currentSolution).c_str());
	m_isolineFilter->GenerateValues(m_numberOfDivisions + 1, range);
	m_isolineMapper->SetLookupTable(stc->vtkObj());
	m_isolineMapper->SelectColorArray(iRIC::toStr(m_currentSolution).c_str());
	m_actorCollection->AddItem(m_isolineActor);
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::setupColorContourSetting()
{
	Post2dBirdEyeWindowGridTypeDataItem* typedi = dynamic_cast<Post2dBirdEyeWindowGridTypeDataItem*>(parent()->parent());
	LookupTableContainer* stc = typedi->lookupTable(m_currentSolution);
	if (stc == 0){return;}
	double range[2];
	stc->getValueRange(&range[0], &range[1]);

	m_valueClippedPolyData->GetPointData()->SetActiveScalars(iRIC::toStr(m_currentSolution).c_str());
	vtkSmartPointer<vtkAppendPolyData> appendFilledContours = vtkSmartPointer<vtkAppendPolyData>::New();
	double delta = (range[1] - range[0]) / static_cast<double>(m_numberOfDivisions);
	std::vector< vtkSmartPointer<vtkClipPolyData> > clippersLo;
	std::vector< vtkSmartPointer<vtkClipPolyData> > clippersHi;

	for (int i = 0; i < m_numberOfDivisions; i++){
		double valueLo = range[0] + static_cast<double> (i) * delta;
		double valueHi = range[0] + static_cast<double> (i + 1) * delta;
		clippersLo.push_back(vtkSmartPointer<vtkClipPolyData>::New());
		if (i == 0){
			clippersLo[i]->SetValue(-HUGE_VAL);
			clippersLo[i]->SetInputData(m_valueClippedPolyData);
		}else{
			clippersLo[i]->SetValue(valueLo);
			clippersLo[i]->SetInputConnection(clippersLo[i - 1]->GetOutputPort());
		}
		clippersLo[i]->InsideOutOff();
		clippersLo[i]->Update();

		clippersHi.push_back(vtkSmartPointer<vtkClipPolyData>::New());
		if(i < m_numberOfDivisions - 1){
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

		vtkSmartPointer<vtkDoubleArray> cd = vtkSmartPointer<vtkDoubleArray>::New();
		cd->SetNumberOfComponents(1);
		cd->SetNumberOfTuples(clippersHi[i]->GetOutput()->GetNumberOfCells());
		cd->FillComponent(0, range[0] + (range[1] - range[0]) * (i / (m_numberOfDivisions - 1.0)));

		clippersHi[i]->GetOutput()->GetCellData()->SetScalars(cd);
		appendFilledContours->AddInputConnection(clippersHi[i]->GetOutputPort());
	}

	vtkSmartPointer<vtkCleanPolyData> filledContours = vtkSmartPointer<vtkCleanPolyData>::New();
	filledContours->SetInputConnection(appendFilledContours->GetOutputPort());
	filledContours->Update();
	m_colorContourPolyData->DeepCopy(filledContours->GetOutput());
	m_valueClippedPolyData->GetPointData()->SetActiveScalars("");

	m_warp->SetInputData(m_colorContourPolyData);
	m_warp->Update();
	m_contourMapper->SetInputData(m_warp->GetOutput());
	m_contourMapper->SetScalarRange(range[0], range[1]);
	m_contourMapper->SetScalarModeToUseCellData();
	m_contourActor->GetProperty()->SetInterpolationToFlat();
	m_contourMapper->SetLookupTable(stc->vtkObj());
	m_contourMapper->UseLookupTableScalarRangeOn();
	m_actorCollection->AddItem(m_contourActor);
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::setupColorFringeSetting()
{
	Post2dBirdEyeWindowGridTypeDataItem* typedi = dynamic_cast<Post2dBirdEyeWindowGridTypeDataItem*>(parent()->parent());
	LookupTableContainer* stc = typedi->lookupTable(m_currentSolution);
	if (stc == 0){return;}
	m_warp->SetInputData(m_valueClippedPolyData);
	m_warp->Update();
	m_fringeMapper->SetInputData(m_warp->GetOutput());
	m_fringeMapper->SetScalarModeToUsePointFieldData();
	m_fringeMapper->SelectColorArray(iRIC::toStr(m_currentSolution).c_str());
	m_fringeMapper->SetLookupTable(stc->vtkObj());
	m_fringeMapper->UseLookupTableScalarRangeOn();
//_	m_fringeActor->GetProperty()->SetOpacity(m_opacityPercent / 100.);
	m_actorCollection->AddItem(m_fringeActor);
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::setupScalarBarSetting()
{
	Post2dBirdEyeWindowGridTypeDataItem* typedi = dynamic_cast<Post2dBirdEyeWindowGridTypeDataItem*>(parent()->parent());
	LookupTableContainer* stc = typedi->lookupTable(m_currentSolution);
	if (stc == 0){return;}

	vtkScalarBarActor* a = m_scalarBarWidget->GetScalarBarActor();
	a->SetTitle(iRIC::toStr(m_colorbarTitleMap.value(m_currentSolution)).c_str());
	a->SetLookupTable(stc->vtkObj());
	a->SetNumberOfLabels(m_scalarBarSetting.numberOfLabels);
	m_titleTextSetting.applySetting(a->GetTitleTextProperty());
	m_labelTextSetting.applySetting(a->GetLabelTextProperty());
	switch (m_contour) {
	case ContourSettingWidget::Isolines:
		a->SetMaximumNumberOfColors(m_numberOfDivisions);
		break;
	case ContourSettingWidget::ContourFigure:
		a->SetMaximumNumberOfColors(m_numberOfDivisions);
		break;
	case ContourSettingWidget::ColorFringe:
		a->SetMaximumNumberOfColors(256);
		break;
	}
}

QDialog* Post2dBirdEyeWindowNodeScalarGroupDataItem::propertyDialog(QWidget* p)
{
	Post2dWindowContourSettingDialog* dialog = new Post2dWindowContourSettingDialog(p);
	Post2dBirdEyeWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dBirdEyeWindowGridTypeDataItem*>(parent()->parent());
	dialog->setGridTypeDataItem(gtItem);
	Post2dBirdEyeWindowZoneDataItem* zItem = dynamic_cast<Post2dBirdEyeWindowZoneDataItem*>(parent());
	if (zItem->dataContainer() == 0 || zItem->dataContainer()->data() == 0){
		delete dialog;
		return 0;
	}
	dialog->setEnabled(true);
	dialog->setZoneData(zItem->dataContainer());
	dialog->setCurrentSolution(m_currentSolution);
	dialog->setContour(m_contour);
	dialog->setNumberOfDivision(m_numberOfDivisions);
	dialog->setFillLower(m_fillLower);
	dialog->setFillUpper(m_fillUpper);
	dialog->hideOpacity();

	// region setting
	if (! zItem->dataContainer()->IBCExists()){
		dialog->disableActive();
	}
	dialog->setRegionMode(m_regionMode);
	dialog->setRange(m_range);

	// scalar bar setting
	dialog->setColorBarTitleMap(m_colorbarTitleMap);
	m_scalarBarSetting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	dialog->setScalarBarSetting(m_scalarBarSetting);
	dialog->setTitleTextSetting(m_titleTextSetting);
	dialog->setLabelTextSetting(m_labelTextSetting);

	return dialog;
}

class Post2dBirdEyeWindowContourSetProperty : public QUndoCommand
{
public:
	Post2dBirdEyeWindowContourSetProperty(bool enabled, ContourSettingWidget::Contour cont, const QString& sol, int numDiv, const LookupTableContainer& ltc, StructuredGridRegion::RegionMode regionmode, StructuredGridRegion::Range2d range, bool fillupper, bool filllower, QString colorbarTitle, const ScalarBarSetting& scalarBarSetting, const vtkTextPropertySettingContainer& titleCont, const vtkTextPropertySettingContainer& labelCont, Post2dBirdEyeWindowNodeScalarGroupDataItem* item)
		: QUndoCommand(QObject::tr("Update Contour Setting"))
	{
		m_newEnabled = enabled;
		m_newContour = cont;
		m_newCurrentSolution = sol;
		m_newNumberOfDivision = numDiv;
		m_newLookupTable = ltc;
		m_newRegionMode = regionmode;
		m_newRange = range;
		m_newFillUpper = fillupper;
		m_newFillLower = filllower;
		m_newScalarBarTitle = colorbarTitle;
		m_newScalarBarSetting = scalarBarSetting;
		m_newTitleTextSetting = titleCont;
		m_newLabelTextSetting = labelCont;

		m_oldEnabled = item->isEnabled();
		m_oldContour = item->m_contour;
		m_oldCurrentSolution = item->m_currentSolution;
		m_oldNumberOfDivision = item->m_numberOfDivisions;
		Post2dBirdEyeWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dBirdEyeWindowGridTypeDataItem*>(item->parent()->parent());
		LookupTableContainer* lut = gtItem->lookupTable(m_newCurrentSolution);
		m_oldLookupTable = *lut;
		m_oldRegionMode = item->m_regionMode;
		m_oldRange = item->m_range;
		m_oldFillUpper = item->m_fillUpper;
		m_oldFillLower = item->m_fillLower;
		m_oldScalarBarTitle = item->m_colorbarTitleMap.value(sol);
		m_oldScalarBarSetting = item->m_scalarBarSetting;
		m_oldTitleTextSetting = item->m_titleTextSetting;
		m_oldLabelTextSetting = item->m_labelTextSetting;

		m_item = item;
	}
	void undo()
	{
		m_item->setIsCommandExecuting(true);
		m_item->setEnabled(m_oldEnabled);
		m_item->m_contour = m_oldContour;
		m_item->setCurrentSolution(m_oldCurrentSolution);
		m_item->m_numberOfDivisions = m_oldNumberOfDivision;
		Post2dBirdEyeWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dBirdEyeWindowGridTypeDataItem*>(m_item->parent()->parent());
		LookupTableContainer* lut = gtItem->lookupTable(m_newCurrentSolution);
		*lut = m_oldLookupTable;
		lut->update();
		m_item->m_regionMode = m_oldRegionMode;
		m_item->m_range = m_oldRange;
		m_item->m_fillUpper = m_oldFillUpper;
		m_item->m_fillLower = m_oldFillLower;
		m_item->m_scalarBarSetting = m_oldScalarBarSetting;
		m_item->m_scalarBarSetting.saveToRepresentation(m_item->m_scalarBarWidget->GetScalarBarRepresentation());
		m_item->m_titleTextSetting = m_oldTitleTextSetting;
		m_item->m_titleTextSetting.applySetting(m_item->m_scalarBarWidget->GetScalarBarActor()->GetTitleTextProperty());
		m_item->m_labelTextSetting = m_oldLabelTextSetting;
		m_item->m_labelTextSetting.applySetting(m_item->m_scalarBarWidget->GetScalarBarActor()->GetLabelTextProperty());
		m_item->m_colorbarTitleMap[m_newCurrentSolution] = m_oldScalarBarTitle;
		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
	void redo()
	{
		m_item->setIsCommandExecuting(true);
		m_item->setEnabled(m_newEnabled);
		m_item->m_contour = m_newContour;
		m_item->setCurrentSolution(m_newCurrentSolution);
		m_item->m_numberOfDivisions = m_newNumberOfDivision;
		Post2dBirdEyeWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dBirdEyeWindowGridTypeDataItem*>(m_item->parent()->parent());
		LookupTableContainer* lut = gtItem->lookupTable(m_newCurrentSolution);
		*lut = m_newLookupTable;
		lut->update();
		m_item->m_regionMode = m_newRegionMode;
		m_item->m_range = m_newRange;
		m_item->m_fillUpper = m_newFillUpper;
		m_item->m_fillLower = m_newFillLower;
		m_item->m_scalarBarSetting = m_newScalarBarSetting;
		m_item->m_scalarBarSetting.saveToRepresentation(m_item->m_scalarBarWidget->GetScalarBarRepresentation());
		m_item->m_titleTextSetting = m_newTitleTextSetting;
		m_item->m_titleTextSetting.applySetting(m_item->m_scalarBarWidget->GetScalarBarActor()->GetTitleTextProperty());
		m_item->m_labelTextSetting = m_newLabelTextSetting;
		m_item->m_labelTextSetting.applySetting(m_item->m_scalarBarWidget->GetScalarBarActor()->GetLabelTextProperty());
		m_item->m_colorbarTitleMap[m_newCurrentSolution] = m_newScalarBarTitle;
		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
private:
	bool m_oldEnabled;
	ContourSettingWidget::Contour m_oldContour;
	QString m_oldCurrentSolution;
	int m_oldNumberOfDivision;
	LookupTableContainer m_oldLookupTable;
	StructuredGridRegion::RegionMode m_oldRegionMode;
	StructuredGridRegion::Range2d m_oldRange;
	bool m_oldFillUpper;
	bool m_oldFillLower;

	QString m_oldScalarBarTitle;
	ScalarBarSetting m_oldScalarBarSetting;
	vtkTextPropertySettingContainer m_oldTitleTextSetting;
	vtkTextPropertySettingContainer m_oldLabelTextSetting;

	bool m_newEnabled;
	ContourSettingWidget::Contour m_newContour;
	QString m_newCurrentSolution;
	int m_newNumberOfDivision;
	LookupTableContainer m_newLookupTable;
	StructuredGridRegion::RegionMode m_newRegionMode;
	StructuredGridRegion::Range2d m_newRange;
	bool m_newFillUpper;
	bool m_newFillLower;

	QString m_newScalarBarTitle;
	ScalarBarSetting m_newScalarBarSetting;
	vtkTextPropertySettingContainer m_newTitleTextSetting;
	vtkTextPropertySettingContainer m_newLabelTextSetting;

	Post2dBirdEyeWindowNodeScalarGroupDataItem* m_item;
};

void Post2dBirdEyeWindowNodeScalarGroupDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	Post2dWindowContourSettingDialog* dialog = dynamic_cast<Post2dWindowContourSettingDialog*>(propDialog);
	iRICUndoStack::instance().push(new Post2dBirdEyeWindowContourSetProperty(true, dialog->contour(), dialog->currentSolution(), dialog->numberOfDivision(), dialog->lookupTable(), dialog->regionMode(), dialog->range(), dialog->fillUpper(), dialog->fillLower(), dialog->scalarBarTitle(), dialog->scalarBarSetting(), dialog->titleTextSetting(), dialog->labelTextSetting(), this));
}

class Post2dBirdEyeWindowContourSelectSolution : public QUndoCommand
{
public:
	Post2dBirdEyeWindowContourSelectSolution(const QString& newsol, Post2dBirdEyeWindowNodeScalarGroupDataItem* item)
		: QUndoCommand(QObject::tr("Contour Physical Value Change"))
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

	Post2dBirdEyeWindowNodeScalarGroupDataItem* m_item;
};

void Post2dBirdEyeWindowNodeScalarGroupDataItem::exclusivelyCheck(Post2dBirdEyeWindowNodeScalarDataItem* item)
{
	if (m_isCommandExecuting){return;}
	iRICUndoStack& stack = iRICUndoStack::instance();
	if (item->standardItem()->checkState() != Qt::Checked){
		stack.push(new Post2dBirdEyeWindowContourSelectSolution("", this));
	}else{
		stack.push(new Post2dBirdEyeWindowContourSelectSolution(item->name(), this));
	}
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::setCurrentSolution(const QString& currentSol)
{
	QList<GraphicsWindowDataItem*>::iterator it;
	Post2dBirdEyeWindowNodeScalarDataItem* current = 0;
	for (it = m_childItems.begin(); it != m_childItems.end(); ++it){
		Post2dBirdEyeWindowNodeScalarDataItem* tmpItem = dynamic_cast<Post2dBirdEyeWindowNodeScalarDataItem*>(*it);
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

void Post2dBirdEyeWindowNodeScalarGroupDataItem::createRangeClippedPolyData()
{
	PostZoneDataContainer* cont = dynamic_cast<Post2dBirdEyeWindowZoneDataItem*>(parent())->dataContainer();
	if (cont->gridType()->defaultGridType() == SolverDefinitionGridType::gtUnstructured2DGrid){
		// unstructured grid.
		vtkPointSet* ps = cont->data();
		vtkSmartPointer<vtkGeometryFilter> geoFilter = vtkSmartPointer<vtkGeometryFilter>::New();
		geoFilter->SetInputData(ps);
		geoFilter->Update();
		if (m_regionMode == StructuredGridRegion::rmFull){
			m_regionClippedPolyData = geoFilter->GetOutput();
		} else if (m_regionMode == StructuredGridRegion::rmActive){
			vtkSmartPointer<vtkClipPolyData> clipper;
			clipper->SetInputConnection(geoFilter->GetOutputPort());
			clipper->SetValue(PostZoneDataContainer::IBCLimit);
			clipper->InsideOutOff();
			clipper->SetInputArrayToProcess(0, 0, 0, 0, iRIC::toStr(PostZoneDataContainer::IBC).c_str());
			clipper->Update();
			m_regionClippedPolyData = clipper->GetOutput();
		}
	} else {
		// structured grid.
		vtkPointSet* ps = cont->data();
		vtkSmartPointer<vtkStructuredGridGeometryFilter> geoFilter = vtkSmartPointer<vtkStructuredGridGeometryFilter>::New();
		geoFilter->SetInputData(ps);
		geoFilter->Update();
		if (m_regionMode == StructuredGridRegion::rmFull){
			m_regionClippedPolyData = geoFilter->GetOutput();
		} else if (m_regionMode == StructuredGridRegion::rmActive){
			vtkSmartPointer<vtkClipPolyData> clipper;
			clipper->SetInputConnection(geoFilter->GetOutputPort());
			clipper->SetValue(PostZoneDataContainer::IBCLimit);
			clipper->InsideOutOff();
			clipper->SetInputArrayToProcess(0, 0, 0, 0, iRIC::toStr(PostZoneDataContainer::IBC).c_str());
			clipper->Update();
			m_regionClippedPolyData = clipper->GetOutput();
		} else if (m_regionMode == StructuredGridRegion::rmCustom){
			geoFilter->SetExtent(m_range.iMin, m_range.iMax, m_range.jMin, m_range.jMax, 0, 0);
			geoFilter->Update();
			m_regionClippedPolyData = geoFilter->GetOutput();
		}
	}
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::createValueClippedPolyData()
{
	vtkSmartPointer<vtkPolyData> upperClipped;
	vtkSmartPointer<vtkPolyData> lowerClipped;

	Post2dBirdEyeWindowGridTypeDataItem* typedi = dynamic_cast<Post2dBirdEyeWindowGridTypeDataItem*>(parent()->parent());
	LookupTableContainer* stc = typedi->lookupTable(m_currentSolution);
	if (stc == 0){return;}
	double min, max;
	stc->getValueRange(&min, &max);
	if (m_fillLower){
		lowerClipped = m_regionClippedPolyData;
	} else {
		vtkSmartPointer<vtkClipPolyData> lowerClipper = vtkSmartPointer<vtkClipPolyData>::New();
		lowerClipper->SetValue(min);
		lowerClipper->SetInputData(m_regionClippedPolyData);
		lowerClipper->InsideOutOff();
		m_regionClippedPolyData->GetPointData()->SetActiveScalars(iRIC::toStr(m_currentSolution).c_str());

		lowerClipper->Update();
		lowerClipped = lowerClipper->GetOutput();
		m_regionClippedPolyData->GetPointData()->SetActiveScalars("");
	}
	if (m_fillUpper){
		upperClipped = lowerClipped;
	} else {
		vtkSmartPointer<vtkClipPolyData> upperClipper = vtkSmartPointer<vtkClipPolyData>::New();
		upperClipper->SetValue(max);
		upperClipper->SetInputData(lowerClipped);
		upperClipper->InsideOutOn();
		lowerClipped->GetPointData()->SetActiveScalars(iRIC::toStr(m_currentSolution).c_str());
		upperClipper->Update();
		upperClipped = upperClipper->GetOutput();
		lowerClipped->GetPointData()->SetActiveScalars("");
	}
	m_valueClippedPolyData->DeepCopy(upperClipped);
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::innerUpdateZScale(double scale)
{
	m_contourActor->SetScale(1, 1, scale);
	m_isolineActor->SetScale(1, 1, scale);
	m_fringeActor->SetScale(1, 1, scale);
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::updateVisibility(bool visible)
{
	bool v = (m_standardItem->checkState() == Qt::Checked) && visible;
	m_scalarBarWidget->SetEnabled(m_scalarBarSetting.visible && v);
	Post2dBirdEyeWindowDataItem::updateVisibility(visible);
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::informSelection(VTKGraphicsView * /*v*/)
{
	m_scalarBarWidget->SetRepositionable(1);
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	m_scalarBarWidget->SetRepositionable(0);
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
}
