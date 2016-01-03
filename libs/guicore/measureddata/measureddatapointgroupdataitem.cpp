#include "../datamodel/vtkgraphicsview.h"
#include "../named/namedgraphicswindowdataitemtool.h"
#include "../misc/targeted/targeteditemsettargetcommandtool.h"
#include "../pre/grid/grid.h"
#include "../project/measured/measureddata.h"
#include "../scalarstocolors/lookuptablecontainer.h"
#include "../scalarstocolors/lookuptablecontainer.h"
#include "measureddatafiledataitem.h"
#include "measureddatapointdataitem.h"
#include "measureddatapointgroupdataitem.h"
#include "measureddatapointsettingdialog.h"
#include "private/measureddatapointgroupdataitem_setsettingcommand.h"

#include <guibase/graphicsmisc.h>
#include <guibase/scalarbarsetting.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

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
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkGeometryFilter.h>
#include <vtkLODActor.h>
#include <vtkPointData.h>
#include <vtkPointSet.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkStringArray.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkTextProperty.h>

MeasuredDataPointGroupDataItem::MeasuredDataPointGroupDataItem(GraphicsWindowDataItem* p) :
	GraphicsWindowDataItem {tr("Scalar"), QIcon(":/libs/guibase/images/iconFolder.png"), p}
{
	m_isDeletable = false;

	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();

	setupActors();

	MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(p)->measuredData();

	for (int i = 0; i < md->scalarNames().size(); ++i) {
		std::string name = md->scalarNames().at(i);
		MeasuredDataPointDataItem* item = new MeasuredDataPointDataItem(name, name.c_str(), this);
		m_childItems.append(item);
		m_colorbarTitleMap.insert({name, name.c_str()});

		LookupTableContainer* cont = new LookupTableContainer(this);

		vtkDataArray* da = md->polyData()->GetPointData()->GetArray(name.c_str());
		double range[2];
		da->GetRange(range);
		cont->setValueRange(range[0], range[1]);
		m_lookupTables.insert({name, cont});
	}
	if (md->scalarNames().size() > 0) {
		auto name = md->scalarNames().at(0);
		setTarget(name);
	}
}

MeasuredDataPointGroupDataItem::~MeasuredDataPointGroupDataItem()
{
	vtkRenderer* r = renderer();
	r->RemoveActor(m_pointsActor);
	r->RemoveActor(m_blackPointsActor);
	r->RemoveActor(m_isolineActor);
	r->RemoveActor(m_contourActor);
	r->RemoveActor(m_fringeActor);
	m_scalarBarWidget->SetInteractor(nullptr);

	m_pointsActor->Delete();
	m_blackPointsActor->Delete();
	m_isolineActor->Delete();
	m_contourActor->Delete();
	m_fringeActor->Delete();
}

void MeasuredDataPointGroupDataItem::updateActorSettings()
{
	// make all the items invisible
	m_pointsActor->VisibilityOff();
	m_blackPointsActor->VisibilityOff();
	m_isolineActor->VisibilityOff();
	m_contourActor->VisibilityOff();
	m_fringeActor->VisibilityOff();
	m_scalarBarWidget->SetEnabled(0);
	m_actorCollection->RemoveAllItems();
	m_actor2DCollection->RemoveAllItems();

	MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(parent())->measuredData();

	if (md == nullptr || md->polyData() == nullptr) {return;}
	vtkPointSet* ps = md->polyData();
	if (m_setting.target == "") {return;}
	// update current active scalar
	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0) {return;}
	createValueClippedPolyData();
	switch (ContourSettingWidget::Contour(m_setting.contour)) {
	case ContourSettingWidget::Points:
		setupPointSetting();
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

	assignActorZValues(m_zDepthRange);
	updateVisibilityWithoutRendering();
}

void MeasuredDataPointGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);

	m_setting.scalarBarSetting.saveToRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	updateActorSettings();
}

void MeasuredDataPointGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.scalarBarSetting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	m_setting.save(writer);
}

void MeasuredDataPointGroupDataItem::setupActors()
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

	m_contourMapper = vtkPolyDataMapper::New();
	m_contourMapper->SetScalarVisibility(true);
	m_contourActor->SetMapper(m_contourMapper);

	m_valueClippedPolyData = vtkSmartPointer<vtkPolyData>::New();
	m_colorContourPolyData = vtkSmartPointer<vtkPolyData>::New();

	m_fringeActor = vtkLODActor::New();
	m_fringeActor->SetNumberOfCloudPoints(5000);
	m_fringeActor->GetProperty()->SetPointSize(2);
	m_fringeActor->GetProperty()->SetLighting(false);
	renderer()->AddActor(m_fringeActor);

	m_fringeMapper = vtkPolyDataMapper::New();
	m_fringeMapper->SetScalarVisibility(true);
	m_fringeActor->SetMapper(m_fringeMapper);

	m_pointsActor = vtkActor::New();
	m_pointsActor->GetProperty()->SetPointSize(2);
	m_pointsActor->GetProperty()->SetLighting(false);
	m_pointsActor->GetProperty()->SetRepresentationToPoints();
	renderer()->AddActor(m_pointsActor);

	m_pointsMapper = vtkPolyDataMapper::New();
	m_pointsMapper->SetScalarVisibility(true);
	m_pointsActor->SetMapper(m_pointsMapper);

	m_blackPointsActor = vtkActor::New();
	m_blackPointsActor->GetProperty()->SetPointSize(2);
	m_blackPointsActor->GetProperty()->SetLighting(false);
	m_blackPointsActor->GetProperty()->SetRepresentationToPoints();
	m_blackPointsActor->GetProperty()->SetColor(0, 0, 0);
	renderer()->AddActor(m_blackPointsActor);

	m_blackPointsMapper = vtkPolyDataMapper::New();
	m_blackPointsMapper->SetScalarVisibility(false);
	m_blackPointsActor->SetMapper(m_blackPointsMapper);

	vtkRenderWindowInteractor* iren = renderer()->GetRenderWindow()->GetInteractor();

	m_scalarBarWidget = vtkSmartPointer<vtkScalarBarWidget>::New();
	iRIC::setupScalarBarProperty(m_scalarBarWidget->GetScalarBarActor());
	m_scalarBarWidget->SetEnabled(0);
	m_scalarBarWidget->SetInteractor(iren);

	m_isolineActor->VisibilityOff();
	m_contourActor->VisibilityOff();
	m_fringeActor->VisibilityOff();
	m_scalarBarWidget->SetEnabled(0);

	m_setting.scalarBarSetting.saveToRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	updateActorSettings();
}

void MeasuredDataPointGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void MeasuredDataPointGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_pointsActor->SetPosition(0, 0, range.max());
	m_blackPointsActor->SetPosition(0, 0, range.min());
	m_contourActor->SetPosition(0, 0, range.min());
	m_fringeActor->SetPosition(0, 0, range.min());
	m_isolineActor->SetPosition(0, 0, range.min());
}

void MeasuredDataPointGroupDataItem::update()
{
	updateActorSettings();
}

void MeasuredDataPointGroupDataItem::setupIsolineSetting()
{
	LookupTableContainer* stc = lookupTable(m_setting.target);
	if (stc == nullptr) {return;}
	double range[2];
	stc->getValueRange(&range[0], &range[1]);
	m_isolineFilter->SetInputData(m_valueClippedPolyData);
	m_isolineFilter->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, iRIC::toStr(m_setting.target).c_str());
	m_isolineFilter->GenerateValues(m_setting.numberOfDivisions + 1, range);
	m_isolineMapper->SetLookupTable(stc->vtkObj());
	m_isolineMapper->SelectColorArray(iRIC::toStr(m_setting.target).c_str());
	m_actorCollection->AddItem(m_isolineActor);
}

void MeasuredDataPointGroupDataItem::setupColorContourSetting()
{
	LookupTableContainer* stc = lookupTable(m_setting.target);
	if (stc == nullptr) {return;}
	double range[2];
	stc->getValueRange(&range[0], &range[1]);

	m_valueClippedPolyData->GetPointData()->SetActiveScalars(iRIC::toStr(m_setting.target).c_str());
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

void MeasuredDataPointGroupDataItem::setupPointSetting()
{
	LookupTableContainer* stc = lookupTable(m_setting.target);
	if (stc == nullptr) {return;}
	m_pointsActor->GetProperty()->SetPointSize(m_setting.pointSize);
	m_pointsMapper->SetInputData(m_valueClippedPolyData);
	m_pointsMapper->SetScalarModeToUsePointFieldData();
	m_pointsMapper->SelectColorArray(iRIC::toStr(m_setting.target).c_str());
	m_pointsMapper->SetLookupTable(stc->vtkObj());
	m_pointsMapper->UseLookupTableScalarRangeOn();
	m_actorCollection->AddItem(m_pointsActor);

	m_blackPointsActor->GetProperty()->SetPointSize(m_setting.pointSize + 2);
	m_pointsMapper->SetInputData(m_valueClippedPolyData);
	m_actorCollection->AddItem(m_blackPointsActor);
}

void MeasuredDataPointGroupDataItem::setupColorFringeSetting()
{
	LookupTableContainer* stc = lookupTable(m_setting.target);
	if (stc == nullptr) {return;}
	m_fringeMapper->SetInputData(m_valueClippedPolyData);
	m_fringeMapper->SetScalarModeToUsePointFieldData();
	m_fringeMapper->SelectColorArray(iRIC::toStr(m_setting.target).c_str());
	m_fringeMapper->SetLookupTable(stc->vtkObj());
	m_fringeMapper->UseLookupTableScalarRangeOn();
	m_fringeActor->GetProperty()->SetOpacity(m_setting.opacity);
	m_actorCollection->AddItem(m_fringeActor);
}

void MeasuredDataPointGroupDataItem::setupScalarBarSetting()
{
	LookupTableContainer* stc = lookupTable(m_setting.target);
	if (stc == nullptr) {return;}
	vtkScalarBarActor* a = m_scalarBarWidget->GetScalarBarActor();
	a->SetTitle(iRIC::toStr(m_colorbarTitleMap[iRIC::toStr(m_setting.target)]).c_str());
	a->SetLookupTable(stc->vtkObj());
	a->SetNumberOfLabels(m_setting.scalarBarSetting.numberOfLabels);
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

QDialog* MeasuredDataPointGroupDataItem::propertyDialog(QWidget* p)
{
	MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(parent())->measuredData();
	if (md == nullptr || md->pointData() == nullptr) {
		return nullptr;
	}
	if (md->scalarNames().size() == 0) {
		return nullptr;
	}
	m_setting.scalarBarSetting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	MeasuredDataPointSettingDialog* dialog = new MeasuredDataPointSettingDialog(p);

	dialog->setData(md);
	if (md->noPolyData()) {
		dialog->forceSelectPointsOnly();
	}

	dialog->setSetting(m_setting);
	dialog->setLookupTables(m_lookupTables);
	dialog->setScalarBarTitleMap(m_colorbarTitleMap);

	return dialog;
}

void MeasuredDataPointGroupDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	MeasuredDataPointSettingDialog* dialog = dynamic_cast<MeasuredDataPointSettingDialog*>(propDialog);
	pushRenderCommand(new SetSettingCommand(dialog->setting(), dialog->lookupTable(), dialog->scalarBarTitle(), this), this, true);
}

void MeasuredDataPointGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Contour Physical Value Change"));
	pushRenderCommand(cmd, this, true);
}

std::string MeasuredDataPointGroupDataItem::target() const
{
	return iRIC::toStr(m_setting.target);
}

void MeasuredDataPointGroupDataItem::setTarget(const std::string &target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	m_setting.target = target.c_str();
	updateActorSettings();
}

void MeasuredDataPointGroupDataItem::createValueClippedPolyData()
{
	vtkSmartPointer<vtkPointSet> upperClipped;
	vtkSmartPointer<vtkPointSet> lowerClipped;

	MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(parent())->measuredData();
	vtkPointSet* pd = nullptr;
	if (md->noPolyData() || m_setting.contour == ContourSettingWidget::Points) {
		pd = md->pointData();
	} else {
		pd = md->polyData();
	}
	pd->GetPointData()->SetActiveScalars(iRIC::toStr(m_setting.target).c_str());

	LookupTableContainer* stc = lookupTable(m_setting.target);
	if (stc == nullptr) {return;}
	double min, max;
	stc->getValueRange(&min, &max);
	if (m_setting.fillLower) {
		lowerClipped = pd;
	} else {
		vtkSmartPointer<vtkClipPolyData> lowerClipper = vtkSmartPointer<vtkClipPolyData>::New();
		lowerClipper->SetValue(min);
		lowerClipper->SetInputData(pd);
		lowerClipper->InsideOutOff();
		lowerClipper->Update();
		lowerClipped = lowerClipper->GetOutput();
	}
	if (m_setting.fillUpper) {
		upperClipped = lowerClipped;
	} else {
		vtkSmartPointer<vtkClipPolyData> upperClipper = vtkSmartPointer<vtkClipPolyData>::New();
		upperClipper->SetValue(max);
		upperClipper->SetInputData(lowerClipped);
		upperClipper->InsideOutOn();
		upperClipper->Update();
		upperClipped = upperClipper->GetOutput();
	}
	m_valueClippedPolyData->DeepCopy(upperClipped);
	pd->GetPointData()->SetActiveScalars("");
}

bool MeasuredDataPointGroupDataItem::hasTransparentPart()
{
	if (standardItem()->checkState() == Qt::Unchecked) {return false;}
	if (m_setting.target == "") {return false;}
	if (m_setting.contour == ContourSettingWidget::Isolines || m_setting.contour == ContourSettingWidget::Points) {return false;}
	if (m_setting.opacity.value() == 100) {return false;}
	return true;
}

LookupTableContainer* MeasuredDataPointGroupDataItem::lookupTable(const std::string& target) const
{
	auto it = m_lookupTables.find(target);
	if (it == m_lookupTables.end()) {return nullptr;}

	return it->second;
}

void MeasuredDataPointGroupDataItem::updateVisibility(bool visible)
{
	bool v = (m_standardItem->checkState() == Qt::Checked) && visible;
	int scalarBarVisible = 0;
	if (v && m_setting.scalarBarSetting.visible) {
		scalarBarVisible = 1;
	}
	m_scalarBarWidget->SetEnabled(scalarBarVisible);
	GraphicsWindowDataItem::updateVisibility(visible);
}

void MeasuredDataPointGroupDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	m_scalarBarWidget->SetRepositionable(1);
}

void MeasuredDataPointGroupDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	m_scalarBarWidget->SetRepositionable(0);
}

void MeasuredDataPointGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
}

void MeasuredDataPointGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void MeasuredDataPointGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
}

void MeasuredDataPointGroupDataItem::doApplyOffset(double /*x*/, double /*y*/)
{
	this->updateActorSettings();
}
