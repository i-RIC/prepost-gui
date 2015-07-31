#include "../post2dwindowdatamodel.h"
#include "post2dwindowcontoursettingdialog.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindownodescalardataitem.h"
#include "post2dwindownodescalargroupdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guibase/coordinatesystem.h>
#include <guibase/graphicsmisc.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDateTime>
#include <QDomNode>
#include <QList>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStandardItem>
#include <QUndoCommand>
#include <QVector3D>
#include <QXmlStreamWriter>

#include <vtkActor2DCollection.h>
#include <vtkActorCollection.h>
#include <vtkAppendPolyData.h>
#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkContourFilter.h>
#include <vtkDoubleArray.h>
#include <vtkGeometryFilter.h>
#include <vtkLODActor.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkScalarsToColors.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkTextProperty.h>

Post2dWindowNodeScalarGroupDataItem::Setting::Setting() :
	CompositeContainer
		{&numberOfDivisions, &currentSolution, &contour, &fillUpper, &fillLower,
		 &opacity, &regionMode, &range, &scalarBarSetting, &titleTextSetting, &labelTextSetting},
	numberOfDivisions {"numberOfDivisions", DEFAULT_NUMOFDIV},
	currentSolution {"solution"},
	contour {"contour", ContourSettingWidget::ColorFringe},
	fillUpper {"fillUpper", true},
	fillLower {"fillLower", true},
	regionMode {"regionMode", StructuredGridRegion::rmFull}
{
	opacity = 50;
	titleTextSetting.setPrefix("titleText");
	labelTextSetting.setPrefix("labelText");
}

Post2dWindowNodeScalarGroupDataItem::Post2dWindowNodeScalarGroupDataItem(Post2dWindowDataItem* p)
	: Post2dWindowDataItem(tr("Scalar"), QIcon(":/libs/guibase/images/iconFolder.png"), p)
{
	m_isDeletable = false;

	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();

	setupActors();
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();
	vtkPointData* pd = cont->data()->GetPointData();
	int number = pd->GetNumberOfArrays();
	for (int i = 0; i < number; i++) {
		vtkAbstractArray* tmparray = pd->GetArray(i);
		if (tmparray == nullptr) {
			continue;
		}
		if (tmparray->GetNumberOfComponents() > 1) {
			// vector attribute.
			continue;
		}
		QString name = pd->GetArray(i)->GetName();
		Post2dWindowNodeScalarDataItem* item = new Post2dWindowNodeScalarDataItem(name, gt->solutionCaption(name), this);
		m_childItems.append(item);
		m_colorbarTitleMap.insert(name, name);
	}
}

Post2dWindowNodeScalarGroupDataItem::~Post2dWindowNodeScalarGroupDataItem()
{
	vtkRenderer* r = renderer();
	r->RemoveActor(m_isolineActor);
	r->RemoveActor(m_contourActor);
	r->RemoveActor(m_fringeActor);
	m_scalarBarWidget->SetInteractor(0);
}

void Post2dWindowNodeScalarGroupDataItem::updateActorSettings()
{
	// make all the items invisible
	m_isolineActor->VisibilityOff();
	m_contourActor->VisibilityOff();
	m_fringeActor->VisibilityOff();
	m_scalarBarWidget->SetEnabled(0);
	m_actorCollection->RemoveAllItems();
	m_actor2DCollection->RemoveAllItems();
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return;}
	vtkPointSet* ps = cont->data();
	if (m_setting.currentSolution == "") {return;}
	// update current active scalar
	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0) {return;}
	createRangeClippedPolyData();
	createValueClippedPolyData();
	switch (ContourSettingWidget::Contour(m_setting.contour)) {
	case ContourSettingWidget::Points:
		// do nothing
		break;
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
	if (m_setting.scalarBarSetting.visible && (m_setting.currentSolution != "")) {
		m_scalarBarWidget->SetEnabled(1);
		setupScalarBarSetting();
	}

	assignActorZValues(m_zDepthRange);
	updateVisibilityWithoutRendering();
}

void Post2dWindowNodeScalarGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	setCurrentSolution(m_setting.currentSolution);
	m_setting.scalarBarSetting.saveToRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());

	QDomNodeList titles = node.childNodes();
	for (int i = 0; i < titles.count(); ++i) {
		QDomElement titleElem = titles.at(i).toElement();
		QString val = titleElem.attribute("value");
		QString title = titleElem.attribute("title");
		m_colorbarTitleMap[val] = title;
	}
	updateActorSettings();
}

void Post2dWindowNodeScalarGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.scalarBarSetting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	m_setting.save(writer);
	// scalar bar titles
	QMapIterator<QString, QString> i(m_colorbarTitleMap);
	while (i.hasNext()) {
		i.next();
		writer.writeStartElement("ScalarBarTitle");
		writer.writeAttribute("value", i.key());
		writer.writeAttribute("title", i.value());
		writer.writeEndElement();
	}
}

void Post2dWindowNodeScalarGroupDataItem::setupActors()
{
	m_isolineActor = vtkActor::New();
	m_isolineActor->GetProperty()->SetLighting(false);
	m_isolineActor->GetProperty()->SetLineWidth(1);
	renderer()->AddActor(m_isolineActor);

	m_isolineMapper = vtkPolyDataMapper::New();
	m_isolineMapper->UseLookupTableScalarRangeOn();
	m_isolineMapper->SetScalarModeToUsePointFieldData();
	m_isolineActor->SetMapper(m_isolineMapper);

	m_isolineFilter = vtkContourFilter::New();
	m_isolineMapper->SetInputConnection(m_isolineFilter->GetOutputPort());

	m_contourActor = vtkSmartPointer<vtkLODActor>::New();
	// Draw 5000 points from grid vertices.
	m_contourActor->SetNumberOfCloudPoints(5000);
	// Make the point size a little big.
	m_contourActor->GetProperty()->SetPointSize(2);
	m_contourActor->GetProperty()->SetLighting(false);
	renderer()->AddActor(m_contourActor);

	m_contourMapper = vtkDataSetMapper::New();
	m_contourMapper->SetScalarVisibility(true);
	m_contourActor->SetMapper(m_contourMapper);

	m_regionClippedPolyData = vtkSmartPointer<vtkPolyData>::New();
	m_valueClippedPolyData = vtkSmartPointer<vtkPolyData>::New();
	m_colorContourPolyData = vtkSmartPointer<vtkPolyData>::New();

	m_fringeActor = vtkSmartPointer<vtkLODActor>::New();
	m_fringeActor->SetNumberOfCloudPoints(5000);
	m_fringeActor->GetProperty()->SetPointSize(2);
	m_fringeActor->GetProperty()->SetLighting(false);
	renderer()->AddActor(m_fringeActor);

	m_fringeMapper = vtkDataSetMapper::New();
	m_fringeMapper->SetScalarVisibility(true);
	m_fringeActor->SetMapper(m_fringeMapper);

	vtkRenderWindowInteractor* iren = renderer()->GetRenderWindow()->GetInteractor();

	m_scalarBarWidget = vtkSmartPointer<vtkScalarBarWidget>::New();
	iRIC::setupScalarBarProperty(m_scalarBarWidget->GetScalarBarActor());
	m_scalarBarWidget->SetInteractor(iren);
	m_scalarBarWidget->SetEnabled(0);

	// set the default lookup table
	m_isolineActor->VisibilityOff();
	m_contourActor->VisibilityOff();
	m_fringeActor->VisibilityOff();

	m_setting.scalarBarSetting.saveToRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	updateActorSettings();
}

void Post2dWindowNodeScalarGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void Post2dWindowNodeScalarGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_contourActor->SetPosition(0, 0, range.min());
	m_fringeActor->SetPosition(0, 0, range.min());
	m_isolineActor->SetPosition(0, 0, range.min());
}

void Post2dWindowNodeScalarGroupDataItem::update()
{
	updateActorSettings();
}

void Post2dWindowNodeScalarGroupDataItem::setupIsolineSetting()
{
	Post2dWindowGridTypeDataItem* typedi = dynamic_cast<Post2dWindowGridTypeDataItem*>(parent()->parent());
	LookupTableContainer* stc = typedi->lookupTable(m_setting.currentSolution);
	if (stc == nullptr) {return;}
	double range[2];
	stc->getValueRange(&range[0], &range[1]);
	m_isolineFilter->SetInputData(m_valueClippedPolyData);
	m_isolineFilter->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, iRIC::toStr(m_setting.currentSolution).c_str());
	m_isolineFilter->GenerateValues(m_setting.numberOfDivisions + 1, range);
	m_isolineMapper->SetLookupTable(stc->vtkObj());
	m_isolineMapper->SelectColorArray(iRIC::toStr(m_setting.currentSolution).c_str());
	m_actorCollection->AddItem(m_isolineActor);
}

void Post2dWindowNodeScalarGroupDataItem::setupColorContourSetting()
{
	Post2dWindowGridTypeDataItem* typedi = dynamic_cast<Post2dWindowGridTypeDataItem*>(parent()->parent());
	LookupTableContainer* stc = typedi->lookupTable(m_setting.currentSolution);
	if (stc == nullptr) {return;}
	double range[2];
	stc->getValueRange(&range[0], &range[1]);

	m_valueClippedPolyData->GetPointData()->SetActiveScalars(iRIC::toStr(m_setting.currentSolution).c_str());
	vtkSmartPointer<vtkAppendPolyData> appendFilledContours = vtkSmartPointer<vtkAppendPolyData>::New();
	double delta = (range[1] - range[0]) / static_cast<double>(m_setting.numberOfDivisions);
	std::vector< vtkSmartPointer<vtkClipPolyData> > clippersLo;
	std::vector< vtkSmartPointer<vtkClipPolyData> > clippersHi;

	for (int i = 0; i < m_setting.numberOfDivisions; i++) {
		double valueLo = range[0] + static_cast<double>(i) * delta;
		double valueHi = range[0] + static_cast<double>(i + 1) * delta;
		clippersLo.push_back(vtkSmartPointer<vtkClipPolyData>::New());
		if (i == 0) {
			clippersLo[i]->SetValue(-HUGE_VAL);
			clippersLo[i]->SetInputData(m_valueClippedPolyData);
		} else {
			clippersLo[i]->SetValue(valueLo);
			clippersLo[i]->SetInputConnection(clippersLo[i - 1]->GetOutputPort());
		}
		clippersLo[i]->InsideOutOff();
		clippersLo[i]->Update();

		clippersHi.push_back(vtkSmartPointer<vtkClipPolyData>::New());
		if (i < m_setting.numberOfDivisions - 1) {
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

		vtkSmartPointer<vtkDoubleArray> cd = vtkSmartPointer<vtkDoubleArray>::New();
		cd->SetNumberOfComponents(1);
		cd->SetNumberOfTuples(clippersHi[i]->GetOutput()->GetNumberOfCells());
		cd->FillComponent(0, range[0] + (range[1] - range[0]) * (i / (m_setting.numberOfDivisions - 1.0)));

		clippersHi[i]->GetOutput()->GetCellData()->SetScalars(cd);
		appendFilledContours->AddInputConnection(clippersHi[i]->GetOutputPort());
	}

	vtkSmartPointer<vtkCleanPolyData> filledContours = vtkSmartPointer<vtkCleanPolyData>::New();
	filledContours->SetInputConnection(appendFilledContours->GetOutputPort());
	filledContours->Update();
	m_colorContourPolyData->DeepCopy(filledContours->GetOutput());
	m_valueClippedPolyData->GetPointData()->SetActiveScalars("");

	m_contourMapper->SetInputData(m_colorContourPolyData);
	m_contourMapper->SetScalarRange(range[0], range[1]);
	m_contourMapper->SetScalarModeToUseCellData();
	m_contourActor->GetProperty()->SetInterpolationToFlat();
	m_contourActor->GetProperty()->SetOpacity(m_setting.opacity);
	m_contourMapper->SetLookupTable(stc->vtkObj());
	m_contourMapper->UseLookupTableScalarRangeOn();
	m_actorCollection->AddItem(m_contourActor);
}

void Post2dWindowNodeScalarGroupDataItem::setupColorFringeSetting()
{
	Post2dWindowGridTypeDataItem* typedi = dynamic_cast<Post2dWindowGridTypeDataItem*>(parent()->parent());
	LookupTableContainer* stc = typedi->lookupTable(m_setting.currentSolution);
	if (stc == nullptr) {return;}
	m_fringeMapper->SetInputData(m_valueClippedPolyData);
	m_fringeMapper->SetScalarModeToUsePointFieldData();
	m_fringeMapper->SelectColorArray(iRIC::toStr(m_setting.currentSolution).c_str());
	m_fringeMapper->SetLookupTable(stc->vtkObj());
	m_fringeMapper->UseLookupTableScalarRangeOn();
	m_fringeActor->GetProperty()->SetOpacity(m_setting.opacity);
	m_actorCollection->AddItem(m_fringeActor);
}

void Post2dWindowNodeScalarGroupDataItem::setupScalarBarSetting()
{
	Post2dWindowGridTypeDataItem* typedi = dynamic_cast<Post2dWindowGridTypeDataItem*>(parent()->parent());
	LookupTableContainer* stc = typedi->lookupTable(m_setting.currentSolution);
	if (stc == nullptr) {return;}

	vtkScalarBarActor* a = m_scalarBarWidget->GetScalarBarActor();
	a->SetTitle(iRIC::toStr(m_colorbarTitleMap.value(m_setting.currentSolution)).c_str());
	a->SetLookupTable(stc->vtkObj());
	a->SetNumberOfLabels(m_setting.scalarBarSetting.numberOfLabels);
	m_setting.titleTextSetting.applySetting(a->GetTitleTextProperty());
	m_setting.labelTextSetting.applySetting(a->GetLabelTextProperty());

	switch (ContourSettingWidget::Contour(m_setting.contour)) {
	case ContourSettingWidget::Points:
		// do nothing
		break;
	case ContourSettingWidget::Isolines:
		a->SetMaximumNumberOfColors(m_setting.numberOfDivisions);
		break;
	case ContourSettingWidget::ContourFigure:
		a->SetMaximumNumberOfColors(m_setting.numberOfDivisions);
		break;
	case ContourSettingWidget::ColorFringe:
		a->SetMaximumNumberOfColors(256);
		break;
	}
}

QDialog* Post2dWindowNodeScalarGroupDataItem::propertyDialog(QWidget* p)
{
	Post2dWindowContourSettingDialog* dialog = new Post2dWindowContourSettingDialog(p);
	Post2dWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*>(parent()->parent());
	dialog->setGridTypeDataItem(gtItem);
	Post2dWindowZoneDataItem* zItem = dynamic_cast<Post2dWindowZoneDataItem*>(parent());
	if (zItem->dataContainer() == nullptr || zItem->dataContainer()->data() == nullptr) {
		delete dialog;
		return nullptr;
	}
	dialog->setZoneData(zItem->dataContainer());
	if (! zItem->dataContainer()->IBCExists()) {
		dialog->disableActive();
	}
	m_setting.scalarBarSetting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	dialog->setSetting(m_setting);
	dialog->setColorBarTitleMap(m_colorbarTitleMap);

	return dialog;
}

class Post2dWindowContourSetProperty : public QUndoCommand
{
public:
	Post2dWindowContourSetProperty(const Post2dWindowNodeScalarGroupDataItem::Setting& s, const LookupTableContainer& ltc, const QString& colorbarTitle, Post2dWindowNodeScalarGroupDataItem* item) :
		QUndoCommand {QObject::tr("Update Contour Setting")},
		m_newSetting {s},
		m_newLookupTable {ltc},
		m_newScalarBarTitle {colorbarTitle},
		m_oldSetting {item->m_setting},
		m_oldScalarBarTitle {item->m_colorbarTitleMap[s.currentSolution]}
	{
		Post2dWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*>(item->parent()->parent());
		LookupTableContainer* lut = gtItem->lookupTable(s.currentSolution);
		m_oldLookupTable = *lut;

		m_item = item;
	}
	void undo() {
		m_item->setIsCommandExecuting(true);
		m_item->m_setting = m_oldSetting;
		m_item->setCurrentSolution(m_oldSetting.currentSolution);
		Post2dWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*>(m_item->parent()->parent());
		LookupTableContainer* lut = gtItem->lookupTable(m_newSetting.currentSolution);
		*lut = m_oldLookupTable;
		lut->update();
		m_item->m_colorbarTitleMap[m_newSetting.currentSolution] = m_oldScalarBarTitle;
		applySettings();
		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
	void redo() {
		m_item->setIsCommandExecuting(true);
		m_item->m_setting = m_newSetting;
		m_item->setCurrentSolution(m_newSetting.currentSolution);
		Post2dWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*>(m_item->parent()->parent());
		LookupTableContainer* lut = gtItem->lookupTable(m_newSetting.currentSolution);
		*lut = m_newLookupTable;
		lut->update();
		m_item->m_colorbarTitleMap[m_newSetting.currentSolution] = m_newScalarBarTitle;
		applySettings();
		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
private:
	void applySettings()
	{
		m_item->m_setting.scalarBarSetting.saveToRepresentation(m_item->m_scalarBarWidget->GetScalarBarRepresentation());
		m_item->m_setting.titleTextSetting.applySetting(m_item->m_scalarBarWidget->GetScalarBarActor()->GetTitleTextProperty());
		m_item->m_setting.labelTextSetting.applySetting(m_item->m_scalarBarWidget->GetScalarBarActor()->GetLabelTextProperty());
	}

	Post2dWindowNodeScalarGroupDataItem::Setting m_newSetting;
	LookupTableContainer m_newLookupTable;
	QString m_newScalarBarTitle;

	Post2dWindowNodeScalarGroupDataItem::Setting m_oldSetting;
	LookupTableContainer m_oldLookupTable;
	QString m_oldScalarBarTitle;

	Post2dWindowNodeScalarGroupDataItem* m_item;
};

void Post2dWindowNodeScalarGroupDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	Post2dWindowContourSettingDialog* dialog = dynamic_cast<Post2dWindowContourSettingDialog*>(propDialog);
	iRICUndoStack::instance().push(new Post2dWindowContourSetProperty(dialog->setting(), dialog->lookupTable(), dialog->scalarBarTitle(), this));
}

class Post2dWindowContourSelectSolution : public QUndoCommand
{
public:
	Post2dWindowContourSelectSolution(const QString& newsol, Post2dWindowNodeScalarGroupDataItem* item)
		: QUndoCommand(QObject::tr("Contour Physical Value Change")) {
		m_newCurrentSolution = newsol;
		m_oldCurrentSolution = item->m_setting.currentSolution;
		m_item = item;
	}
	void undo() {
		m_item->setIsCommandExecuting(true);
		m_item->setCurrentSolution(m_oldCurrentSolution);
		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
	void redo() {
		m_item->setIsCommandExecuting(true);
		m_item->setCurrentSolution(m_newCurrentSolution);
		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
private:
	QString m_oldCurrentSolution;
	QString m_newCurrentSolution;

	Post2dWindowNodeScalarGroupDataItem* m_item;
};

void Post2dWindowNodeScalarGroupDataItem::exclusivelyCheck(Post2dWindowNodeScalarDataItem* item)
{
	if (m_isCommandExecuting) {return;}
	iRICUndoStack& stack = iRICUndoStack::instance();
	if (item->standardItem()->checkState() != Qt::Checked) {
		stack.push(new Post2dWindowContourSelectSolution("", this));
	} else {
		stack.push(new Post2dWindowContourSelectSolution(item->name(), this));
	}
}

void Post2dWindowNodeScalarGroupDataItem::setCurrentSolution(const QString& currentSol)
{
	Post2dWindowNodeScalarDataItem* current = nullptr;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		Post2dWindowNodeScalarDataItem* tmpItem = dynamic_cast<Post2dWindowNodeScalarDataItem*>(*it);
		if (tmpItem->name() == currentSol) {
			current = tmpItem;
		}
		tmpItem->standardItem()->setCheckState(Qt::Unchecked);
	}
	if (current != nullptr) {
		current->standardItem()->setCheckState(Qt::Checked);
	}
	m_setting.currentSolution = currentSol;
}

void Post2dWindowNodeScalarGroupDataItem::createRangeClippedPolyData()
{
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	vtkPolyData* polyData = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->filteredData();
	if (cont->gridType()->defaultGridType() == SolverDefinitionGridType::gtUnstructured2DGrid) {
		// unstructured grid.
		vtkPointSet* ps = polyData;
		vtkSmartPointer<vtkGeometryFilter> geoFilter = vtkSmartPointer<vtkGeometryFilter>::New();
		geoFilter->SetInputData(ps);
		if (m_setting.regionMode == StructuredGridRegion::rmFull) {
			geoFilter->Update();
			m_regionClippedPolyData = geoFilter->GetOutput();
		} else if (m_setting.regionMode == StructuredGridRegion::rmActive) {
			vtkSmartPointer<vtkClipPolyData> clipper = vtkSmartPointer<vtkClipPolyData>::New();
			clipper->SetInputConnection(geoFilter->GetOutputPort());
			clipper->SetValue(PostZoneDataContainer::IBCLimit);
			clipper->InsideOutOff();
			ps->GetPointData()->SetActiveScalars(iRIC::toStr(PostZoneDataContainer::IBC).c_str());
			clipper->Update();
			m_regionClippedPolyData = vtkSmartPointer<vtkPolyData>::New();
			m_regionClippedPolyData->DeepCopy(clipper->GetOutput());
		}
	} else {
		// structured grid.
		if (m_setting.regionMode == StructuredGridRegion::rmFull) {
			m_regionClippedPolyData = polyData;
		} else if (m_setting.regionMode == StructuredGridRegion::rmActive) {
			vtkSmartPointer<vtkClipPolyData> clipper = vtkSmartPointer<vtkClipPolyData>::New();
			clipper->SetInputData(polyData);
			clipper->SetValue(PostZoneDataContainer::IBCLimit);
			clipper->InsideOutOff();
			cont->data()->GetPointData()->SetActiveScalars(iRIC::toStr(PostZoneDataContainer::IBC).c_str());
			clipper->Update();
			m_regionClippedPolyData = vtkSmartPointer<vtkPolyData>::New();
			m_regionClippedPolyData->DeepCopy(clipper->GetOutput());
		} else if (m_setting.regionMode == StructuredGridRegion::rmCustom) {
			vtkSmartPointer<vtkStructuredGridGeometryFilter> geoFilter = vtkSmartPointer<vtkStructuredGridGeometryFilter>::New();
			geoFilter->SetInputData(cont->data());
			StructuredGridRegion::Range2d r = m_setting.range;
			geoFilter->SetExtent(r.iMin, r.iMax, r.jMin, r.jMax, 0, 0);
			geoFilter->Update();
			m_regionClippedPolyData = geoFilter->GetOutput();
		}
	}
}

void Post2dWindowNodeScalarGroupDataItem::createValueClippedPolyData()
{
	vtkSmartPointer<vtkPolyData> upperClipped;
	vtkSmartPointer<vtkPolyData> lowerClipped;

	Post2dWindowGridTypeDataItem* typedi = dynamic_cast<Post2dWindowGridTypeDataItem*>(parent()->parent());
	LookupTableContainer* stc = typedi->lookupTable(m_setting.currentSolution);
	if (stc == 0) {return;}
	double min, max;
	stc->getValueRange(&min, &max);
	if (m_setting.fillLower) {
		lowerClipped = m_regionClippedPolyData;
	} else {
		vtkSmartPointer<vtkClipPolyData> lowerClipper = vtkSmartPointer<vtkClipPolyData>::New();
		lowerClipper->SetValue(min);
		lowerClipper->SetInputData(m_regionClippedPolyData);
		lowerClipper->InsideOutOff();
		m_regionClippedPolyData->GetPointData()->SetActiveScalars(iRIC::toStr(m_setting.currentSolution).c_str());

		lowerClipper->Update();
		lowerClipped = lowerClipper->GetOutput();
		m_regionClippedPolyData->GetPointData()->SetActiveScalars("");
	}
	if (m_setting.fillUpper) {
		upperClipped = lowerClipped;
	} else {
		vtkSmartPointer<vtkClipPolyData> upperClipper = vtkSmartPointer<vtkClipPolyData>::New();
		upperClipper->SetValue(max);
		upperClipper->SetInputData(lowerClipped);
		upperClipper->InsideOutOn();
		lowerClipped->GetPointData()->SetActiveScalars(iRIC::toStr(m_setting.currentSolution).c_str());
		upperClipper->Update();
		upperClipped = upperClipper->GetOutput();
		lowerClipped->GetPointData()->SetActiveScalars("");
	}
	m_valueClippedPolyData->DeepCopy(upperClipped);
}

bool Post2dWindowNodeScalarGroupDataItem::hasTransparentPart()
{
	if (standardItem()->checkState() == Qt::Unchecked) {return false;}
	if (m_setting.currentSolution == "") {return false;}
	if (m_setting.contour == ContourSettingWidget::Isolines) {return false;}
	if (m_setting.opacity == 100) {return false;}
	return true;
}

void Post2dWindowNodeScalarGroupDataItem::updateVisibility(bool visible)
{
	bool v = (m_standardItem->checkState() == Qt::Checked) && visible;
	m_scalarBarWidget->SetEnabled(m_setting.scalarBarSetting.visible.value() && v && (m_setting.currentSolution != ""));
	Post2dWindowDataItem::updateVisibility(visible);
}

void Post2dWindowNodeScalarGroupDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	m_scalarBarWidget->SetRepositionable(1);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->initNodeAttributeBrowser();
}

void Post2dWindowNodeScalarGroupDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	m_scalarBarWidget->SetRepositionable(0);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->clearNodeAttributeBrowser();
}

void Post2dWindowNodeScalarGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->updateNodeAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeScalarGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void Post2dWindowNodeScalarGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->fixNodeAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeScalarGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->showNodeAttributeBrowserAction();
	menu->addAction(abAction);
}

bool Post2dWindowNodeScalarGroupDataItem::checkKmlExportCondition()
{
	// check the condition.
	if (m_setting.contour != ContourSettingWidget::ContourFigure) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("To export KML for street view, display with Contour Fringe."));
		return false;
	}
	Post2dWindowGridTypeDataItem* typedi = dynamic_cast<Post2dWindowGridTypeDataItem*>(parent()->parent());
	LookupTableContainer* stc = typedi->lookupTable(m_setting.currentSolution);
	if (stc->autoRange()) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("To export KML for street view, value range should be set up manually."));
		return false;
	}
	CoordinateSystem* cs = projectData()->mainfile()->coordinateSystem();
	if (cs == nullptr) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("To export KML for street view, coordinate system should be specified."));
		return false;
	}

	return true;
}

bool Post2dWindowNodeScalarGroupDataItem::exportKMLHeader(QXmlStreamWriter& writer)
{
	Post2dWindowGridTypeDataItem* typedi = dynamic_cast<Post2dWindowGridTypeDataItem*>(parent()->parent());
	LookupTableContainer* stc = typedi->lookupTable(m_setting.currentSolution);

	writer.writeStartElement("Document");
	writer.writeTextElement("name", "iRIC Calculation Result");
	writer.writeTextElement("open", "1");

	// output styles.
	for (int i = 0; i < m_setting.numberOfDivisions; ++i) {
		double val = stc->manualMin() + i * (stc->manualMax() - stc->manualMin()) / (m_setting.numberOfDivisions - 1);
		double rgb[3];
		stc->vtkObj()->GetColor(val, rgb);
		QColor col;
		col.setRedF(rgb[0]);
		col.setGreenF(rgb[1]);
		col.setBlueF(rgb[2]);
		QString colorStr = QString("%1%2%3%4").arg("c8").arg(col.blue(), 2, 16, QChar('0')).arg(col.green(), 2, 16, QChar('0')).arg(col.red(), 2, 16, QChar('0'));

		writer.writeStartElement("Style");
		writer.writeAttribute("id", QString("PolyColor%1").arg(i + 1));

		writer.writeStartElement("LineStyle");
		writer.writeTextElement("color", colorStr);
		writer.writeEndElement();

		writer.writeStartElement("PolyStyle");
		writer.writeTextElement("color", colorStr);
		writer.writeEndElement();

		writer.writeEndElement();
	}

	return true;
}

bool Post2dWindowNodeScalarGroupDataItem::exportKMLFooter(QXmlStreamWriter& writer)
{
	writer.writeEndElement();
	return true;
}

bool Post2dWindowNodeScalarGroupDataItem::exportKMLForTimestep(QXmlStreamWriter& writer, int /*index*/, double time)
{
	CoordinateSystem* cs = projectData()->mainfile()->coordinateSystem();
	Post2dWindowGridTypeDataItem* typedi = dynamic_cast<Post2dWindowGridTypeDataItem*>(parent()->parent());
	LookupTableContainer* stc = typedi->lookupTable(m_setting.currentSolution);

	double div = (stc->manualMax() - stc->manualMin()) / m_setting.numberOfDivisions;

	// Folder start
	writer.writeStartElement("Folder");
	// TimeStamp Start
	QDateTime datetime(QDate(2011, 1, 1));
	datetime = datetime.addSecs(static_cast<int>(time));
	writer.writeStartElement("TimeStamp");
	writer.writeTextElement("when", datetime.toString("yyyy-MM-ddTHH:mm:ssZ"));
	// TimeStamp End
	writer.writeEndElement();

	// name
	writer.writeTextElement("name", QString("iRIC output t = %1").arg(time));

	// output each cell data.
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	vtkPointSet* ps = cont->data();
	vtkDataArray* da = ps->GetPointData()->GetArray(iRIC::toStr(m_setting.currentSolution).c_str());
	vtkStructuredGrid* stGrid = dynamic_cast<vtkStructuredGrid*>(ps);
	bool isStructured = (stGrid != nullptr);

	for (vtkIdType cellId = 0; cellId < ps->GetNumberOfCells(); ++cellId) {

		QList<QVector3D> points;
		vtkCell* cell = ps->GetCell(cellId);
		QList<vtkIdType> pointIds;

		for (int pId = 0; pId < cell->GetNumberOfPoints(); ++pId) {
			vtkIdType pointId = cell->GetPointId(pId);
			pointIds.append(pointId);
		}

		for (int i = 0; i < pointIds.size(); ++i) {
			vtkIdType pointId = pointIds.at(i);
			double v[3];
			double lon, lat;
			ps->GetPoint(pointId, v);
			cs->mapGridToGeo(v[0], v[1], &lon, &lat);
			double val = da->GetTuple1(pointId);
			points.append(QVector3D(lon, lat, val));
		}
		// find north, south, west, east
		double north = 0, south = 0, west = 0, east = 0;
		double minval = 0;
		double maxval = 0;
		double sum = 0;
		for (int i = 0; i < points.size(); ++i) {
			QVector3D v = points.at(i);
			sum += v.z();
			if (i == 0 || v.y() > north) {north = v.y();}
			if (i == 0 || v.y() < south) {south = v.y();}
			if (i == 0 || v.x() > east) {east = v.x();}
			if (i == 0 || v.x() < west) {west = v.x();}
			if (i == 0 || v.z() > maxval) {maxval = v.z();}
			if (i == 0 || v.z() < minval) {minval = v.z();}
		}

		double averageDepth = sum / points.size();

		if (averageDepth < stc->manualMin()) {continue;}

		writer.writeStartElement("Placemark");
		// name
		writer.writeTextElement("name", QString("depth = %1 [m]").arg(averageDepth));
		// description
		QString descString;
		if (isStructured) {
			int i, j, k;
			cont->getCellIJKIndex(static_cast<int>(cellId), &i, &j, &k);
			descString = QString("i = %1 j = %2").arg(i + 1).arg(j + 1);
		} else {
			descString = QString("index = %1").arg(cellId + 1);
		}
		writer.writeTextElement("description", descString);
		// styleurl
		int colorIndex = (averageDepth / div) + 1;
		if (colorIndex < 1) {colorIndex = 1;}
		if (colorIndex > m_setting.numberOfDivisions) {colorIndex = m_setting.numberOfDivisions;}

		QString styleUrl = QString("#PolyColor%1").arg(colorIndex);
		writer.writeTextElement("styleUrl", styleUrl);
		// Region Start
		writer.writeStartElement("Region");

		// LOD
//		writer.writeStartElement("Lod");
//		writer.writeTextElement("minLodPixels", "10");
//		writer.writeEndElement();

		// LatLonAltBox Start
		writer.writeStartElement("LatLonAltBox");
		writer.writeTextElement("north", QString::number(north, 'g', 12));
		writer.writeTextElement("south", QString::number(south, 'g', 12));
		writer.writeTextElement("east", QString::number(east, 'g', 12));
		writer.writeTextElement("west", QString::number(west, 'g', 12));
		writer.writeTextElement("minAltitude", QString::number(minval, 'g', 12));
		writer.writeTextElement("maxAltitude", QString::number(maxval, 'g', 12));
		writer.writeTextElement("altitudeMode", "relativeToGround");
		// LatLonAltBox End
		writer.writeEndElement();

		// Region End
		writer.writeEndElement();
		// Polygon Start
		writer.writeStartElement("Polygon");
		writer.writeTextElement("extrude", "1");
		writer.writeTextElement("altitudeMode", "relativeToGround");
		// outerBoundaryIs Start
		writer.writeStartElement("outerBoundaryIs");
		// LinearRing Start
		writer.writeStartElement("LinearRing");
		QStringList coords;
		for (int i = 0; i < points.size(); ++i) {
			QVector3D v = points.at(i);
			coords.append(QString("%1,%2,%3").arg(QString::number(v.x(), 'f', 12)).arg(QString::number(v.y(), 'f', 12)).arg(QString::number(v.z(), 'g', 12)));
		}
		QVector3D vlast = points.at(0);
		coords.append(QString("%1,%2,%3").arg(QString::number(vlast.x(), 'f', 12)).arg(QString::number(vlast.y(), 'f', 12)).arg(QString::number(vlast.z(), 'g', 12)));
		QString coordsStr("\r\n");
		coordsStr.append(coords.join("\r\n"));
		coordsStr.append("\r\n");
		writer.writeTextElement("coordinates", coordsStr);

		// LinearRing End
		writer.writeEndElement();
		// outerBoundaryIs End
		writer.writeEndElement();

		// Polygon End
		writer.writeEndElement();

		// Placemark end
		writer.writeEndElement();
	}

	// Folder end
	writer.writeEndElement();
	return true;
}
