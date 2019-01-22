#include "../post2dwindowdatamodel.h"
#include "post2dwindowcontoursettingdialog.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowcellscalargroupdataitem.h"
#include "post2dwindownodescalargroupdataitem_shapeexporter.h"
#include "post2dwindowcellscalargrouptopdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <cs/coordinatesystem.h>
#include <guibase/graphicsmisc.h>
#include <guibase/iricactivecellfilter.h>
#include <guibase/vtkCustomScalarBarActor.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/datamodel/graphicswindowdrawcommands.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <guicore/scalarstocolors/scalarstocolorscontainerutil.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDateTime>
#include <QDomNode>
#include <QFileInfo>
#include <QList>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStandardItem>
#include <QTextStream>
#include <QUndoCommand>
#include <QVector3D>
#include <QXmlStreamWriter>

#include <vtkActor2DCollection.h>
#include <vtkActorCollection.h>
#include <vtkAppendPolyData.h>
#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkCellDataToPointData.h>
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
#include <vtkScalarsToColors.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkTextProperty.h>

#include <vtkPolyDataWriter.h>

Post2dWindowCellScalarGroupDataItem::Post2dWindowCellScalarGroupDataItem(Post2dWindowDataItem* p, CheckFlag cflag, ReorderFlag rflag, DeleteFlag dflag) :
	Post2dWindowDataItem {tr("Scalar (cell center)"), QIcon(":/libs/guibase/images/iconPaper.png"), p}
{
	setupStandardItem(cflag, rflag, dflag);

	setupActors();

	//m_shapeExporter = new ShapeExporter(this);
}

Post2dWindowCellScalarGroupDataItem::~Post2dWindowCellScalarGroupDataItem()
{
	vtkRenderer* r = renderer();
	r->RemoveActor(m_isolineActor);
	r->RemoveActor(m_contourActor);
	r->RemoveActor(m_fringeActor);
	m_scalarBarWidget->SetInteractor(0);


	m_contourActor->Delete();
	m_contourMapper->Delete();

	m_isolineActor->Delete();
	m_isolineFilter->Delete();

	m_fringeActor->Delete();
	m_fringeMapper->Delete();
	m_scalarBarWidget->Delete();

	//delete m_shapeExporter;
}

std::string Post2dWindowCellScalarGroupDataItem::target() const
{
	return m_setting.target;
}

void Post2dWindowCellScalarGroupDataItem::setTarget(const std::string& target)
{
	m_setting.target = target.c_str();
	updateActorSettings();
}

void Post2dWindowCellScalarGroupDataItem::updateActorSettings()
{
	// make all the items invisible
	m_isolineActor->VisibilityOff();
	m_contourActor->VisibilityOff();
	m_fringeActor->VisibilityOff();
	m_scalarBarWidget->SetEnabled(0);
	m_actorCollection->RemoveAllItems();
	m_actor2DCollection->RemoveAllItems();
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return;}
	vtkPointSet* ps = cont->data();
	if (m_setting.target == "") {return;}
	// update current active scalar
	vtkCellData* cd = ps->GetCellData();
	Q_ASSERT(cd != nullptr);
	if (cd->GetNumberOfArrays() == 0) { return; }

	std::string targetStr = iRIC::toStr(m_setting.target);
	ScalarsToColorsContainerUtil::setValueRange(&m_lookupTableContainer, cd->GetArray(targetStr.c_str()));

	Post2dWindowCellScalarGroupTopDataItem* topitem = dynamic_cast<Post2dWindowCellScalarGroupTopDataItem*>(parent());
	m_standardItem->setText(topitem->m_colorbarTitleMap.value(targetStr));
	m_standardItemCopy->setText(topitem->m_colorbarTitleMap.value(targetStr));

	vtkPolyData* polyData = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->filteredData();
	if (polyData == nullptr) return;
	vtkPolyData* rcp = createRangeClippedPolyData(polyData);
	vtkPolyData* vcp = createValueClippedPolyData(rcp);
	rcp->Delete();
	switch (ContourSettingWidget::Contour(m_setting.contour)) {
	case ContourSettingWidget::Points:
		// do nothing
		break;
	case ContourSettingWidget::Isolines:
		setupIsolineSetting(vcp);
		break;
	case ContourSettingWidget::ContourFigure:
		setupColorContourSetting(vcp);
		break;
	case ContourSettingWidget::ColorFringe:
		setupColorFringeSetting(vcp);
		break;
	}
	vcp->Delete();
	if (m_setting.scalarBarSetting.visible && (m_setting.target != "")) {
		m_scalarBarWidget->SetEnabled(1);
		setupScalarBarSetting();
	}

	assignActorZValues(m_zDepthRange);
	updateVisibilityWithoutRendering();
}

void Post2dWindowCellScalarGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	setTarget(m_setting.target);
	m_setting.scalarBarSetting.saveToRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	updateActorSettings();

	QDomNode ltNode = iRIC::getChildNode(node, "LookupTable");
	if (ltNode.isNull()) {
		Post2dWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*>(parent()->parent()->parent());
		m_lookupTableContainer = *(gtItem->cellLookupTable(m_setting.target));
	} else {
		m_lookupTableContainer.loadFromProjectMainFile(ltNode);
	}
}

void Post2dWindowCellScalarGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.scalarBarSetting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	m_setting.save(writer);

	// write lookuptable
	writer.writeStartElement("LookupTable");
	m_lookupTableContainer.saveToProjectMainFile(writer);
	writer.writeEndElement();
}

void Post2dWindowCellScalarGroupDataItem::setupActors()
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

	m_contourActor = vtkLODActor::New();
	// Draw 5000 points from grid vertices.
	m_contourActor->SetNumberOfCloudPoints(5000);
	// Make the point size a little big.
	m_contourActor->GetProperty()->SetPointSize(2);
	m_contourActor->GetProperty()->SetLighting(false);
	renderer()->AddActor(m_contourActor);

	m_contourMapper = vtkDataSetMapper::New();
	m_contourMapper->SetScalarVisibility(true);
	m_contourActor->SetMapper(m_contourMapper);

	m_fringeActor = vtkLODActor::New();
	m_fringeActor->SetNumberOfCloudPoints(5000);
	m_fringeActor->GetProperty()->SetPointSize(2);
	m_fringeActor->GetProperty()->SetLighting(false);
	renderer()->AddActor(m_fringeActor);

	m_fringeMapper = vtkDataSetMapper::New();
	m_fringeMapper->SetScalarVisibility(true);
	m_fringeActor->SetMapper(m_fringeMapper);

	vtkRenderer* ren = renderer();
	Q_ASSERT(ren != nullptr);
	vtkRenderWindow* rwin = ren->GetRenderWindow();
	Q_ASSERT(rwin != nullptr);
	vtkRenderWindowInteractor* iren = renderer()->GetRenderWindow()->GetInteractor();
	Q_ASSERT(iren != nullptr);

	m_scalarBarWidget = vtkScalarBarWidget::New();
	m_scalarBarWidget->SetScalarBarActor(vtkCustomScalarBarActor::New());
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

void Post2dWindowCellScalarGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void Post2dWindowCellScalarGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_zDepthRange = range;

	m_contourActor->SetPosition(0, 0, range.min());
	m_fringeActor->SetPosition(0, 0, range.min());
	m_isolineActor->SetPosition(0, 0, range.min());
}

void Post2dWindowCellScalarGroupDataItem::handleStandardItemChange()
{
	if (m_isCommandExecuting) { return; }
	Post2dWindowCellScalarGroupTopDataItem* topitem = dynamic_cast<Post2dWindowCellScalarGroupTopDataItem*>(parent());
	if (m_standardItem->checkState() == Qt::Checked) {
		Q_ASSERT(m_standardItemCopy->checkState() == Qt::Unchecked);
		if (! topitem->nextScalarBarSetting(m_setting.scalarBarSetting)) {
			m_isCommandExecuting = true;
			m_standardItem->setCheckState(Qt::Unchecked);
			Q_ASSERT(m_standardItemCopy->checkState() == Qt::Unchecked);
			m_isCommandExecuting = false;
			return;
		}
	}
	else {
		Q_ASSERT(m_standardItemCopy->checkState() == Qt::Checked);
	}
	m_setting.scalarBarSetting.saveToRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	updateActorSettings();
	GraphicsWindowDataItem::handleStandardItemChange();
}

void Post2dWindowCellScalarGroupDataItem::update()
{
	updateActorSettings();
}

void Post2dWindowCellScalarGroupDataItem::setupIsolineSetting(vtkPolyData* polyData)
{
	// transform cell data into point data
	vtkSmartPointer<vtkCellDataToPointData> cellToPoint = vtkCellDataToPointData::New();
	cellToPoint->SetInputData(polyData);
	cellToPoint->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, iRIC::toStr(m_setting.target).c_str());

	double range[2];
	m_lookupTableContainer.getValueRange(&range[0], &range[1]);
	m_isolineFilter->SetInputConnection(cellToPoint->GetOutputPort(0));
	m_isolineFilter->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, iRIC::toStr(m_setting.target).c_str());
	m_isolineFilter->GenerateValues(m_setting.numberOfDivisions + 1, range);
	m_isolineMapper->SetLookupTable(m_lookupTableContainer.vtkObj());
	m_isolineMapper->SelectColorArray(iRIC::toStr(m_setting.target).c_str());
	m_actorCollection->AddItem(m_isolineActor);
}

vtkPolyData* Post2dWindowCellScalarGroupDataItem::setupLowerClippedPolygon(vtkPolyData* inputData, double value)
{
	vtkClipPolyData* clipper = vtkClipPolyData::New();
	clipper->SetValue(value);
	clipper->SetInputData(inputData);
	clipper->InsideOutOff();

	vtkCleanPolyData* cleaner = vtkCleanPolyData::New();
	cleaner->SetInputConnection(clipper->GetOutputPort());
	cleaner->Update();

	vtkPolyData* data = cleaner->GetOutput();
	data->Register(0);
	clipper->Delete();
	cleaner->Delete();
	return data;
}

vtkPolyData* Post2dWindowCellScalarGroupDataItem::setupHigherClippedPolygon(vtkPolyData* inputData, double value)
{
	vtkClipPolyData* clipper = vtkClipPolyData::New();
	clipper->SetValue(value);
	clipper->SetInputData(inputData);
	clipper->InsideOutOn();

	vtkCleanPolyData* cleaner = vtkCleanPolyData::New();
	cleaner->SetInputConnection(clipper->GetOutputPort());
	cleaner->Update();

	vtkPolyData* data = cleaner->GetOutput();
	data->Register(0);
	clipper->Delete();
	cleaner->Delete();
	return data;
}

void Post2dWindowCellScalarGroupDataItem::setupColorContourSetting(vtkPolyData* polyData)
{
	double range[2];
	m_lookupTableContainer.getValueRange(&range[0], &range[1]);

	vtkPolyData* ccPolyData = createColorContourPolyData(polyData);
	m_contourMapper->SetInputData(ccPolyData);
	ccPolyData->UnRegister(0);

	m_contourMapper->SetScalarRange(range[0], range[1]);
	m_contourMapper->SetScalarModeToUseCellData();
	m_contourActor->GetProperty()->SetInterpolationToFlat();
	m_contourActor->GetProperty()->SetOpacity(m_setting.opacity);
	m_contourMapper->SetLookupTable(m_lookupTableContainer.vtkObj());
	m_contourMapper->UseLookupTableScalarRangeOn();
	m_actorCollection->AddItem(m_contourActor);
}

void Post2dWindowCellScalarGroupDataItem::setupColorFringeSetting(vtkPolyData* polyData)
{
	m_fringeMapper->SetInputData(polyData);
	m_fringeMapper->SetScalarModeToUseCellFieldData();
	m_fringeMapper->SelectColorArray(iRIC::toStr(m_setting.target).c_str());
	m_fringeMapper->SetLookupTable(m_lookupTableContainer.vtkObj());
	m_fringeMapper->UseLookupTableScalarRangeOn();
	m_fringeActor->GetProperty()->SetOpacity(m_setting.opacity);
	m_actorCollection->AddItem(m_fringeActor);
}

void Post2dWindowCellScalarGroupDataItem::setupScalarBarSetting()
{
	Post2dWindowCellScalarGroupTopDataItem* topitem = dynamic_cast<Post2dWindowCellScalarGroupTopDataItem*>(parent());
	vtkScalarBarActor* a = m_scalarBarWidget->GetScalarBarActor();
	a->SetTitle(iRIC::toStr(topitem->m_colorbarTitleMap.value(iRIC::toStr(m_setting.target))).c_str());
	a->SetLookupTable(m_lookupTableContainer.vtkObj());
	a->SetNumberOfLabels(m_setting.scalarBarSetting.numberOfLabels);
	m_setting.scalarBarSetting.titleTextSetting.applySetting(a->GetTitleTextProperty());
	m_setting.scalarBarSetting.labelTextSetting.applySetting(a->GetLabelTextProperty());

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

void Post2dWindowCellScalarGroupDataItem::innerUpdateZScale(double scale)
{
	m_contourActor->SetScale(1, scale, 1);
	m_isolineActor->SetScale(1, scale, 1);
	m_fringeActor->SetScale(1, scale, 1);
}

QDialog* Post2dWindowCellScalarGroupDataItem::propertyDialog(QWidget* p)
{
	Post2dWindowContourSettingDialog* dialog = new Post2dWindowContourSettingDialog(p);
	Post2dWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*>(parent()->parent()->parent());
	*(gtItem->cellLookupTable(m_setting.target)) = m_lookupTableContainer;
	dialog->setGridTypeDataItem(gtItem);
	Post2dWindowZoneDataItem* zItem = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent());
	if (zItem->dataContainer() == nullptr || zItem->dataContainer()->data() == nullptr) {
		delete dialog;
		return nullptr;
	}
	dialog->setZoneData(zItem->dataContainer(), CellCenter);
	dialog->disablePhysicalValueComboBox();
	if (! zItem->dataContainer()->IBCCellExists()) {
		dialog->disableActive();
	}
	m_setting.scalarBarSetting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	dialog->setSetting(m_setting);
	Post2dWindowCellScalarGroupTopDataItem* topitem = dynamic_cast<Post2dWindowCellScalarGroupTopDataItem*>(parent());
	dialog->setColorBarTitleMap(topitem->m_colorbarTitleMap);

	return dialog;
}

class Post2dWindowCellScalarGroupDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const Post2dWindowContourSetting& s, const LookupTableContainer& ltc, const QString& colorbarTitle, Post2dWindowCellScalarGroupDataItem* item, QUndoCommand* parent = nullptr) :
		QUndoCommand {Post2dWindowCellScalarGroupDataItem::tr("Update Contour Setting"), parent},
		m_newSetting {s},
		m_newLookupTable {ltc},
		m_newScalarBarTitle {colorbarTitle},
		m_oldSetting {item->m_setting},
		m_oldLookupTable {item->m_lookupTableContainer},
		m_item {item},
		m_topItem {dynamic_cast<Post2dWindowCellScalarGroupTopDataItem*>(item->parent())}
	{
		m_oldScalarBarTitle = m_topItem->m_colorbarTitleMap[item->target()];
	}
	void undo() {
		m_item->m_setting = m_oldSetting;
		m_topItem->m_colorbarTitleMap[m_newSetting.target] = m_oldScalarBarTitle;
		applySettings(m_oldSetting.target, m_oldLookupTable);
		m_item->updateActorSettings();
	}
	void redo() {
		m_item->m_setting = m_newSetting;
		m_topItem->m_colorbarTitleMap[m_newSetting.target] = m_newScalarBarTitle;
		applySettings(m_newSetting.target, m_newLookupTable);
		m_item->updateActorSettings();
	}
private:
	void applySettings(const std::string& sol, const LookupTableContainer& c) {
		m_item->setTarget(sol);
		m_item->m_lookupTableContainer = c;
		m_item->m_setting.scalarBarSetting.saveToRepresentation(m_item->m_scalarBarWidget->GetScalarBarRepresentation());
		m_item->m_setting.scalarBarSetting.titleTextSetting.applySetting(m_item->m_scalarBarWidget->GetScalarBarActor()->GetTitleTextProperty());
		m_item->m_setting.scalarBarSetting.labelTextSetting.applySetting(m_item->m_scalarBarWidget->GetScalarBarActor()->GetLabelTextProperty());
	}

	Post2dWindowContourSetting m_newSetting;
	LookupTableContainer m_newLookupTable;
	QString m_newScalarBarTitle;

	Post2dWindowContourSetting m_oldSetting;
	LookupTableContainer m_oldLookupTable;
	QString m_oldScalarBarTitle;

	Post2dWindowCellScalarGroupDataItem* m_item;
	Post2dWindowCellScalarGroupTopDataItem* m_topItem;
};

void Post2dWindowCellScalarGroupDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	Post2dWindowContourSettingDialog* dialog = dynamic_cast<Post2dWindowContourSettingDialog*>(propDialog);
	pushRenderCommand(new SetSettingCommand(dialog->setting(), dialog->lookupTable(), dialog->scalarBarTitle(), this), this, true);
}

vtkPolyData* Post2dWindowCellScalarGroupDataItem::createRangeClippedPolyData(vtkPolyData* polyData)
{
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	if (m_setting.regionMode == StructuredGridRegion::rmFull) {
		polyData->Register(0);
		return polyData;
	} else if (m_setting.regionMode == StructuredGridRegion::rmActive) {
		vtkSmartPointer<iricActiveCellFilter> filter = vtkSmartPointer<iricActiveCellFilter>::New();
		filter->SetInputData(polyData);
		filter->SetValue(PostZoneDataContainer::IBCLimit);
		filter->CellClippingOn();
		polyData->GetCellData()->SetActiveScalars(iRIC::toStr(PostZoneDataContainer::IBC).c_str());
		filter->Update();
		vtkPolyData* activeData = filter->GetOutput();
		activeData->Register(0);
		return activeData;
	} else if (m_setting.regionMode == StructuredGridRegion::rmCustom) {
		vtkSmartPointer<vtkStructuredGridGeometryFilter> geoFilter = vtkSmartPointer<vtkStructuredGridGeometryFilter>::New();
		geoFilter->SetInputData(cont->data());
		StructuredGridRegion::Range2d r = m_setting.range;
		geoFilter->SetExtent(r.iMin, r.iMax, r.jMin, r.jMax, 0, 0);
		geoFilter->Update();
		vtkPolyData* clippedData = geoFilter->GetOutput();
		clippedData->Register(0);
		return clippedData;
	}
	Q_ASSERT(false);
	return nullptr;
}

vtkPolyData* Post2dWindowCellScalarGroupDataItem::createValueClippedPolyData(vtkPolyData* polyData)
{
	vtkSmartPointer<vtkPolyData> upperClipped;
	vtkSmartPointer<vtkPolyData> lowerClipped;

	double min, max;
	m_lookupTableContainer.getValueRange(&min, &max);
	if (m_setting.fillLower) {
		lowerClipped = polyData;
	} else {
		vtkSmartPointer<iricActiveCellFilter> lowerClipper = vtkSmartPointer<iricActiveCellFilter>::New();
		lowerClipper->SetValue(min);
		lowerClipper->CellClippingOn();
		lowerClipper->SetInputData(polyData);
		lowerClipper->InsideOutOff();
		polyData->GetCellData()->SetActiveScalars(iRIC::toStr(m_setting.target).c_str());
		lowerClipper->Update();
		lowerClipped = lowerClipper->GetOutput();
		polyData->GetCellData()->SetActiveScalars("");
	}
	if (m_setting.fillUpper) {
		upperClipped = lowerClipped;
	} else {
		vtkSmartPointer<iricActiveCellFilter> upperClipper = vtkSmartPointer<iricActiveCellFilter>::New();
		upperClipper->SetValue(max);
		upperClipper->CellClippingOn();
		upperClipper->SetInputData(lowerClipped);
		upperClipper->InsideOutOn();
		lowerClipped->GetCellData()->SetActiveScalars(iRIC::toStr(m_setting.target).c_str());
		upperClipper->Update();
		upperClipped = upperClipper->GetOutput();
		lowerClipped->GetCellData()->SetActiveScalars("");
	}
	upperClipped->Register(0);
	return upperClipped;
}

vtkPolyData* Post2dWindowCellScalarGroupDataItem::createColorContourPolyData(vtkPolyData* polyData)
{
	double range[2];
	m_lookupTableContainer.getValueRange(&range[0], &range[1]);

	polyData->GetPointData()->SetActiveScalars(iRIC::toStr(m_setting.target).c_str());

	vtkSmartPointer<vtkAppendPolyData> appendFilledContours = vtkSmartPointer<vtkAppendPolyData>::New();
	double delta = (range[1] - range[0]) / static_cast<double>(m_setting.numberOfDivisions);

	vtkPolyData* inputPolyData = polyData;
	inputPolyData->Register(0);

	for (int i = 0; i < m_setting.numberOfDivisions; i++) {
		double lowValue = range[0] + static_cast<double>(i) * delta;
		double highValue = range[0] + static_cast<double>(i + 1) * delta;
		if (i == 0){
			lowValue = - HUGE_VAL;
		}
		if (i == m_setting.numberOfDivisions - 1){
			highValue = HUGE_VAL;
		}
		vtkPolyData* lowClipped = setupLowerClippedPolygon(inputPolyData, lowValue);
		inputPolyData->UnRegister(0);

		vtkPolyData* bothClipped = setupHigherClippedPolygon(lowClipped, highValue);

		vtkSmartPointer<vtkDoubleArray> cd = vtkSmartPointer<vtkDoubleArray>::New();
		cd->SetNumberOfComponents(1);
		cd->SetNumberOfTuples(bothClipped->GetNumberOfCells());
		cd->FillComponent(0, range[0] + (range[1] - range[0]) * (i / (m_setting.numberOfDivisions - 1.0)));
		bothClipped->GetCellData()->SetScalars(cd);
		appendFilledContours->AddInputData(bothClipped);
		bothClipped->UnRegister(0);

		inputPolyData = lowClipped;
	}
	inputPolyData->UnRegister(0);

	vtkSmartPointer<vtkCleanPolyData> filledContours = vtkSmartPointer<vtkCleanPolyData>::New();
	filledContours->SetInputConnection(appendFilledContours->GetOutputPort());
	filledContours->Update();

	vtkPolyData* ret = filledContours->GetOutput();
	ret->Register(0);
	polyData->GetPointData()->SetActiveScalars("");

	return ret;
}

bool Post2dWindowCellScalarGroupDataItem::hasTransparentPart()
{
	if (standardItem()->checkState() == Qt::Unchecked) {return false;}
	if (m_setting.target == "") {return false;}
	if (m_setting.contour == ContourSettingWidget::Isolines) {return false;}
	if (m_setting.opacity == 100) {return false;}
	return true;
}

void Post2dWindowCellScalarGroupDataItem::updateVisibility(bool visible)
{
	bool v = (m_standardItem->checkState() == Qt::Checked) && visible;
	m_scalarBarWidget->SetEnabled(m_setting.scalarBarSetting.visible.value() && v && (m_setting.target != ""));
	Post2dWindowDataItem::updateVisibility(visible);
}

void Post2dWindowCellScalarGroupDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	m_scalarBarWidget->SetRepositionable(1);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->initCellResultAttributeBrowser();
}

void Post2dWindowCellScalarGroupDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	m_scalarBarWidget->SetRepositionable(0);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->clearCellResultAttributeBrowser();
}

void Post2dWindowCellScalarGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->updateCellResultAttributeBrowser(event->pos(), v);
}

void Post2dWindowCellScalarGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void Post2dWindowCellScalarGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
	if (event->button() == Qt::LeftButton) {
		dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->fixCellResultAttributeBrowser(event->pos(), v);
	}
}

void Post2dWindowCellScalarGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->showAttributeBrowserActionForCellResult();
	menu->addAction(abAction);
}

bool Post2dWindowCellScalarGroupDataItem::checkKmlExportCondition()
{
	// check the condition.
	if (m_setting.contour != ContourSettingWidget::ContourFigure) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("To export KML for street view, display with Contour Figure."));
		return false;
	}
	if (m_lookupTableContainer.autoRange()) {
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

bool Post2dWindowCellScalarGroupDataItem::exportKMLHeader(QXmlStreamWriter& writer)
{
	writer.writeStartElement("Document");
	writer.writeTextElement("name", "iRIC Calculation Result");
	writer.writeTextElement("open", "1");

	// output styles.
	auto& stc = m_lookupTableContainer;
	for (int i = 0; i < m_setting.numberOfDivisions; ++i) {
		double val = stc.manualMin() + i * (stc.manualMax() - stc.manualMin()) / (m_setting.numberOfDivisions - 1);
		double rgb[3];
		stc.vtkObj()->GetColor(val, rgb);
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

bool Post2dWindowCellScalarGroupDataItem::exportKMLFooter(QXmlStreamWriter& writer)
{
	writer.writeEndElement();
	return true;
}

bool Post2dWindowCellScalarGroupDataItem::exportKMLForTimestep(QXmlStreamWriter& writer, int /*index*/, double time)
{
	CoordinateSystem* cs = projectData()->mainfile()->coordinateSystem();
	auto& stc = m_lookupTableContainer;

	double div = (stc.manualMax() - stc.manualMin()) / m_setting.numberOfDivisions;

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
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	vtkPointSet* ps = cont->data();
	vtkDataArray* da = ps->GetPointData()->GetArray(iRIC::toStr(m_setting.target).c_str());
	vtkStructuredGrid* stGrid = dynamic_cast<vtkStructuredGrid*>(ps);
	bool isStructured = (stGrid != nullptr);
	auto off = offset();

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
			v[0] += off.x();
			v[1] += off.y();
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

		if (averageDepth < stc.manualMin()) {continue;}

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

//bool Post2dWindowCellScalarGroupDataItem::exportContourFigureToShape(const QString& filename, double time)
//{
//	return m_shapeExporter->exportContourFigure(filename, time);
//}

void Post2dWindowCellScalarGroupDataItem::undoCommands(QDialog* propDialog, QUndoCommand* parent)
{
	Post2dWindowContourSettingDialog* dialog = dynamic_cast<Post2dWindowContourSettingDialog*>(propDialog);

	Q_ASSERT(parent != nullptr); // the following won't get deleted if parent is null

	new GraphicsWindowDrawOnUndo(this, parent);
	new SetSettingCommand(dialog->setting(), dialog->lookupTable(), dialog->scalarBarTitle(), this, parent);
	new GraphicsWindowDrawOnRedo(this, parent);
}
