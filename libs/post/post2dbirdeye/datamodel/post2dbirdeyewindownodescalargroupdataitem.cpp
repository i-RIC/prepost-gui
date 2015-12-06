#include "../post2dbirdeyewindowdatamodel.h"
#include "post2dbirdeyewindowgridtypedataitem.h"
#include "post2dbirdeyewindownodescalardataitem.h"
#include "post2dbirdeyewindownodescalargroupdataitem.h"
#include "post2dbirdeyewindowzonedataitem.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guibase/graphicsmisc.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/project/projectdata.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>
#include <post/post2d/datamodel/post2dwindowcontoursettingdialog.h>

#include <QDomNode>
#include <QList>
#include <QStandardItem>
#include <QUndoCommand>
#include <QXmlStreamWriter>

#include <vtkActor2DCollection.h>
#include <vtkActorCollection.h>
#include <vtkAppendPolyData.h>
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
#include <vtkStringArray.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkTextProperty.h>

Post2dBirdEyeWindowNodeScalarGroupDataItem::Post2dBirdEyeWindowNodeScalarGroupDataItem(Post2dBirdEyeWindowDataItem* p) :
	Post2dBirdEyeWindowDataItem {tr("Contour"), QIcon(":/libs/guibase/images/iconFolder.png"), p}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	setupActors();
	PostZoneDataContainer* cont = dynamic_cast<Post2dBirdEyeWindowZoneDataItem*>(parent())->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();
	vtkPointData* pd = cont->data()->GetPointData();
	for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(pd)) {
		auto item = new Post2dBirdEyeWindowNodeScalarDataItem(name, gt->solutionCaption(name), this);
		m_childItems.append(item);
		m_colorbarTitleMap.insert(name, name.c_str());
	}
	m_setting.opacity = 0;
}

Post2dBirdEyeWindowNodeScalarGroupDataItem::~Post2dBirdEyeWindowNodeScalarGroupDataItem()
{
	vtkRenderer* r = renderer();
	r->RemoveActor(m_isolineActor);
	r->RemoveActor(m_contourActor);
	r->RemoveActor(m_fringeActor);
	m_scalarBarWidget->SetInteractor(nullptr);
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
	if (cont == nullptr || cont->data() == nullptr) {return;}
	vtkPointSet* ps = cont->data();
	if (m_setting.currentSolution == "") {return;}

	// update current active scalar
	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0) {return;}

	m_warp->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, iRIC::toStr(cont->elevationName()).c_str());

	createRangeClippedPolyData();
	createValueClippedPolyData();
	switch (ContourSettingWidget::Contour(m_setting.contour)) {
	case ContourSettingWidget::Points:
		// do nothing.
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
	if (m_setting.scalarBarSetting.visible) {
		m_scalarBarWidget->SetEnabled(1);
		setupScalarBarSetting();
	}
	updateVisibilityWithoutRendering();
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	setCurrentSolution(m_setting.currentSolution);

	m_setting.scalarBarSetting.saveToRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());

	QDomNodeList titles = node.childNodes();
	for (int i = 0; i < titles.count(); ++i) {
		QDomElement titleElem = titles.at(i).toElement();
		std::string val = iRIC::toStr(titleElem.attribute("value"));
		QString title = titleElem.attribute("title");
		m_colorbarTitleMap[val] = title;
	}
	updateActorSettings();
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.scalarBarSetting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());

	m_setting.save(writer);

	// scalar bar titles
	QMapIterator<std::string, QString> i(m_colorbarTitleMap);
	while (i.hasNext()) {
		i.next();
		writer.writeStartElement("ScalarBarTitle");
		writer.writeAttribute("value", i.key().c_str());
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
	m_setting.scalarBarSetting.saveToRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());

	updateActorSettings();
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::update()
{
	updateActorSettings();
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::setupIsolineSetting()
{
	Post2dBirdEyeWindowGridTypeDataItem* typedi = dynamic_cast<Post2dBirdEyeWindowGridTypeDataItem*>(parent()->parent());
	LookupTableContainer* stc = typedi->lookupTable(m_setting.currentSolution);
	if (stc == nullptr) {return;}
	double range[2];
	stc->getValueRange(&range[0], &range[1]);

	m_warp->SetInputData(m_valueClippedPolyData);
	m_warp->Update();
	m_isolineFilter->SetInputData(m_warp->GetOutput());
	m_isolineFilter->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, iRIC::toStr(m_setting.currentSolution).c_str());
	m_isolineFilter->GenerateValues(m_setting.numberOfDivisions + 1, range);
	m_isolineMapper->SetLookupTable(stc->vtkObj());
	m_isolineMapper->SelectColorArray(iRIC::toStr(m_setting.currentSolution).c_str());
	m_actorCollection->AddItem(m_isolineActor);
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::setupColorContourSetting()
{
	Post2dBirdEyeWindowGridTypeDataItem* typedi = dynamic_cast<Post2dBirdEyeWindowGridTypeDataItem*>(parent()->parent());
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
	LookupTableContainer* stc = typedi->lookupTable(currentSolution());
	if (stc == nullptr) {return;}
	m_warp->SetInputData(m_valueClippedPolyData);
	m_warp->Update();
	m_fringeMapper->SetInputData(m_warp->GetOutput());
	m_fringeMapper->SetScalarModeToUsePointFieldData();
	m_fringeMapper->SelectColorArray(currentSolution().c_str());
	m_fringeMapper->SetLookupTable(stc->vtkObj());
	m_fringeMapper->UseLookupTableScalarRangeOn();
//_	m_fringeActor->GetProperty()->SetOpacity(m_opacityPercent / 100.);
	m_actorCollection->AddItem(m_fringeActor);
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::setupScalarBarSetting()
{
	Post2dBirdEyeWindowGridTypeDataItem* typedi = dynamic_cast<Post2dBirdEyeWindowGridTypeDataItem*>(parent()->parent());
	LookupTableContainer* stc = typedi->lookupTable(currentSolution());
	if (stc == nullptr) {return;}

	vtkScalarBarActor* a = m_scalarBarWidget->GetScalarBarActor();
	a->SetTitle(iRIC::toStr(m_colorbarTitleMap.value(currentSolution())).c_str());
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

QDialog* Post2dBirdEyeWindowNodeScalarGroupDataItem::propertyDialog(QWidget* p)
{
	Post2dWindowContourSettingDialog* dialog = new Post2dWindowContourSettingDialog(p);
	Post2dBirdEyeWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dBirdEyeWindowGridTypeDataItem*>(parent()->parent());
	dialog->setGridTypeDataItem(gtItem);
	Post2dBirdEyeWindowZoneDataItem* zItem = dynamic_cast<Post2dBirdEyeWindowZoneDataItem*>(parent());
	if (zItem->dataContainer() == nullptr || zItem->dataContainer()->data() == nullptr) {
		delete dialog;
		return nullptr;
	}
	dialog->setZoneData(zItem->dataContainer());
	dialog->hideOpacity();
	// region setting
	if (! zItem->dataContainer()->IBCExists()) {
		dialog->disableActive();
	}
	m_setting.scalarBarSetting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	dialog->setSetting(m_setting);
	// scalar bar setting
	dialog->setColorBarTitleMap(m_colorbarTitleMap);

	return dialog;
}

class Post2dBirdEyeWindowNodeScalarGroupDataItem::SetSettingCommand: public QUndoCommand
{
public:
	SetSettingCommand(const Post2dWindowContourSetting& s, const LookupTableContainer& ltc, const QString& colorbarTitle, Post2dBirdEyeWindowNodeScalarGroupDataItem* item) :
		QUndoCommand {QObject::tr("Update Contour Setting")},
		m_newSetting {s},
		m_newLookupTable {ltc},
		m_newScalarBarTitle {colorbarTitle},
		m_oldSetting {item->m_setting},
		m_oldScalarBarTitle {item->m_colorbarTitleMap[s.currentSolution]},
		m_item {item}
	{
		Post2dBirdEyeWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dBirdEyeWindowGridTypeDataItem*>(item->parent()->parent());
		LookupTableContainer* lut = gtItem->lookupTable(s.currentSolution);
		m_oldLookupTable = *lut;
	}
	void redo() {
		m_item->m_setting = m_newSetting;
		applySettings(m_newSetting.currentSolution, m_newLookupTable, m_newScalarBarTitle);
		m_item->updateActorSettings();
	}
	void undo() {
		m_item->m_setting = m_oldSetting;
		applySettings(m_oldSetting.currentSolution, m_oldLookupTable, m_oldScalarBarTitle);
		m_item->updateActorSettings();
	}
private:
	void applySettings(const std::string& sol, const LookupTableContainer& c, QString& title)
	{
		m_item->setCurrentSolution(sol);
		Post2dBirdEyeWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dBirdEyeWindowGridTypeDataItem*>(m_item->parent()->parent());
		LookupTableContainer* lut = gtItem->lookupTable(sol);
		*lut = c;
		lut->update();
		m_item->m_colorbarTitleMap[sol] = title;
		m_item->m_setting.scalarBarSetting.saveToRepresentation(m_item->m_scalarBarWidget->GetScalarBarRepresentation());
		m_item->m_setting.titleTextSetting.applySetting(m_item->m_scalarBarWidget->GetScalarBarActor()->GetTitleTextProperty());
		m_item->m_setting.labelTextSetting.applySetting(m_item->m_scalarBarWidget->GetScalarBarActor()->GetLabelTextProperty());
	}

	Post2dWindowContourSetting m_newSetting;
	LookupTableContainer m_newLookupTable;
	QString m_newScalarBarTitle;

	Post2dWindowContourSetting m_oldSetting;
	LookupTableContainer m_oldLookupTable;
	QString m_oldScalarBarTitle;

	Post2dBirdEyeWindowNodeScalarGroupDataItem* m_item;
};

void Post2dBirdEyeWindowNodeScalarGroupDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	Post2dWindowContourSettingDialog* dialog = dynamic_cast<Post2dWindowContourSettingDialog*>(propDialog);
	pushRenderCommand(new SetSettingCommand(dialog->setting(), dialog->lookupTable(), dialog->scalarBarTitle(), this), this, true);
}

class Post2dBirdEyeWindowNodeScalarGroupDataItem::SelectSolutionCommand : public QUndoCommand
{
public:
	SelectSolutionCommand(const std::string& newsol, Post2dBirdEyeWindowNodeScalarGroupDataItem* item) :
		QUndoCommand{ QObject::tr("Contour Physical Value Change") },
		m_newCurrentSolution(newsol),
		m_oldCurrentSolution(m_item->currentSolution()),
		m_item{ item }
	{}
	void redo() override {
		m_item->setCurrentSolution(m_newCurrentSolution);
		m_item->updateActorSettings();
	}
	void undo() override {
		m_item->setCurrentSolution(m_oldCurrentSolution);
		m_item->updateActorSettings();
	}
private:
	std::string m_newCurrentSolution;
	std::string m_oldCurrentSolution;

	Post2dBirdEyeWindowNodeScalarGroupDataItem* m_item;
};

void Post2dBirdEyeWindowNodeScalarGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (item->standardItem()->checkState() != Qt::Checked) {
		pushRenderCommand(new SelectSolutionCommand("", this), this, true);
	} else {
		pushRenderCommand(new SelectSolutionCommand(item->name(), this), this, true);
	}
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::setCurrentSolution(const std::string& currentSol)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(currentSol, m_childItems);
	m_setting.currentSolution = currentSol.c_str();
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::createRangeClippedPolyData()
{
	PostZoneDataContainer* cont = dynamic_cast<Post2dBirdEyeWindowZoneDataItem*>(parent())->dataContainer();
	if (dynamic_cast<vtkStructuredGrid*> (cont->data()) == nullptr) {
		// unstructured grid.
		vtkSmartPointer<vtkGeometryFilter> geoFilter = vtkSmartPointer<vtkGeometryFilter>::New();
		geoFilter->SetInputData(cont->data());
		geoFilter->Update();
		if (m_setting.regionMode == StructuredGridRegion::rmFull) {
			m_regionClippedPolyData = geoFilter->GetOutput();
		} else if (m_setting.regionMode == StructuredGridRegion::rmActive) {
			vtkSmartPointer<vtkClipPolyData> clipper = vtkSmartPointer<vtkClipPolyData>::New();
			clipper->SetInputConnection(geoFilter->GetOutputPort());
			clipper->SetValue(PostZoneDataContainer::IBCLimit);
			clipper->InsideOutOff();
			clipper->SetInputArrayToProcess(0, 0, 0, 0, iRIC::toStr(PostZoneDataContainer::IBC).c_str());
			clipper->Update();
			m_regionClippedPolyData = clipper->GetOutput();
		}
	} else {
		// structured grid.
		vtkSmartPointer<vtkStructuredGridGeometryFilter> geoFilter = vtkSmartPointer<vtkStructuredGridGeometryFilter>::New();
		geoFilter->SetInputData(cont->data());
		geoFilter->Update();
		if (m_setting.regionMode == StructuredGridRegion::rmFull) {
			m_regionClippedPolyData = geoFilter->GetOutput();
		} else if (m_setting.regionMode == StructuredGridRegion::rmActive) {
			vtkSmartPointer<vtkClipPolyData> clipper;
			clipper->SetInputConnection(geoFilter->GetOutputPort());
			clipper->SetValue(PostZoneDataContainer::IBCLimit);
			clipper->InsideOutOff();
			clipper->SetInputArrayToProcess(0, 0, 0, 0, iRIC::toStr(PostZoneDataContainer::IBC).c_str());
			clipper->Update();
			m_regionClippedPolyData = clipper->GetOutput();
		} else if (m_setting.regionMode == StructuredGridRegion::rmCustom) {
			const StructuredGridRegion::Range2d& r = m_setting.range;
			geoFilter->SetExtent(r.iMin, r.iMax, r.jMin, r.jMax, 0, 0);
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
	LookupTableContainer* stc = typedi->lookupTable(currentSolution());
	if (stc == nullptr) {return;}
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

void Post2dBirdEyeWindowNodeScalarGroupDataItem::innerUpdateZScale(double scale)
{
	m_contourActor->SetScale(1, 1, scale);
	m_isolineActor->SetScale(1, 1, scale);
	m_fringeActor->SetScale(1, 1, scale);
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::updateVisibility(bool visible)
{
	bool v = (m_standardItem->checkState() == Qt::Checked) && visible;
	m_scalarBarWidget->SetEnabled(m_setting.scalarBarSetting.visible.value() && v);
	Post2dBirdEyeWindowDataItem::updateVisibility(visible);
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::informSelection(VTKGraphicsView* /*v*/)
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
