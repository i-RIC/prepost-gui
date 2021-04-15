#include "../../../guibase/objectbrowserview.h"
#include "../post3dwindowdatamodel.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowisosurfacesettingdialog.h"
#include "post3dwindownodescalargroupdataitem.h"
#include "post3dwindowzonedataitem.h"
#include "private/post3dwindownodescalargroupdataitem_setsettingcommand.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QList>
#include <QMenu>
#include <QStandardItem>
#include <QUndoCommand>
#include <QXmlStreamWriter>

#include <vtkActor.h>
#include <vtkActorCollection.h>
#include <vtkAppendPolyData.h>
#include <vtkCellData.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkContourFilter.h>
#include <vtkDelaunay2D.h>
#include <vtkDoubleArray.h>
#include <vtkExtractGrid.h>
#include <vtkLODActor.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkTextProperty.h>

Post3dWindowNodeScalarGroupDataItem::Post3dWindowNodeScalarGroupDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem {tr("Isosurface"), QIcon(":/libs/guibase/images/iconPaper.png"), p},
	m_fullRange {true},
	m_isoValue {0.0},
	m_color {Qt::white},
	m_opacity {100}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	setDefaultValues();
	setupActors();

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();
}

Post3dWindowNodeScalarGroupDataItem::~Post3dWindowNodeScalarGroupDataItem()
{
	vtkRenderer* r = renderer();
	r->RemoveActor(m_isoSurfaceActor);
}

void Post3dWindowNodeScalarGroupDataItem::setDefaultValues()
{
	validateRange();
}

void Post3dWindowNodeScalarGroupDataItem::updateActorSettings()
{
	m_isoSurfaceActor->VisibilityOff();
	m_actorCollection->RemoveAllItems();

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	if (cont == nullptr) {return;}
	vtkPointSet* ps = cont->data();
	if (ps == nullptr) {return;}
	if (m_target == "") {return;}

	m_standardItem->setText(m_target.c_str());
	m_standardItemCopy->setText(m_target.c_str());

	// update current active scalar
	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0) {return;}

	setupIsosurfaceSetting();
	updateColorSetting();

	updateVisibilityWithoutRendering();
}

void Post3dWindowNodeScalarGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	setTarget(iRIC::toStr(elem.attribute("solution")));
	m_fullRange = iRIC::getBooleanAttribute(node, "fullRange", true);
	m_range.iMin = iRIC::getIntAttribute(node, "iMin");
	m_range.iMax = iRIC::getIntAttribute(node, "iMax");
	m_range.jMin = iRIC::getIntAttribute(node, "jMin");
	m_range.jMax = iRIC::getIntAttribute(node, "jMax");
	m_range.kMin = iRIC::getIntAttribute(node, "kMin");
	m_range.kMax = iRIC::getIntAttribute(node, "kMax");
	m_isoValue = iRIC::getDoubleAttribute(node, "value");
	m_color = iRIC::getColorAttribute(node, "color", Qt::white);
	m_opacity = iRIC::getIntAttribute(node, "opacityPercent", 100);
	validateRange();
	updateActorSettings();
}

void Post3dWindowNodeScalarGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("solution", m_target.c_str());
	iRIC::setBooleanAttribute(writer, "fullRange", m_fullRange);
	iRIC::setIntAttribute(writer, "iMin", m_range.iMin);
	iRIC::setIntAttribute(writer, "iMax", m_range.iMax);
	iRIC::setIntAttribute(writer, "jMin", m_range.jMin);
	iRIC::setIntAttribute(writer, "jMax", m_range.jMax);
	iRIC::setIntAttribute(writer, "kMin", m_range.kMin);
	iRIC::setIntAttribute(writer, "kMax", m_range.kMax);
	iRIC::setDoubleAttribute(writer, "value", m_isoValue);
	iRIC::setColorAttribute(writer, "color", m_color);
	iRIC::setIntAttribute(writer, "opacityPercent", m_opacity);
}

void Post3dWindowNodeScalarGroupDataItem::setupActors()
{
	// Mapper for graphical primitives
	m_isoSurfaceMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_isoSurfaceMapper->ScalarVisibilityOff();

	// Create an actor for the Isosurface
	m_isoSurfaceActor = vtkSmartPointer<vtkActor>::New();
	m_isoSurfaceActor->SetMapper(m_isoSurfaceMapper);

	// Visualize
	renderer()->AddActor(m_isoSurfaceActor);

	updateActorSettings();
}

void Post3dWindowNodeScalarGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void Post3dWindowNodeScalarGroupDataItem::assignActorZValues(const ZDepthRange& /*range*/)
{

}

void Post3dWindowNodeScalarGroupDataItem::update()
{
	updateActorSettings();
}

void Post3dWindowNodeScalarGroupDataItem::setupIsosurfaceSetting()
{
	// input data
	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	vtkPointSet* ps = cont->data();

	// extract interest volume
	vtkSmartPointer<vtkExtractGrid> voi = vtkSmartPointer<vtkExtractGrid>::New();
	voi->SetInputData(ps);
	voi->SetVOI(
		m_range.iMin, m_range.iMax,
		m_range.jMin, m_range.jMax,
		m_range.kMin, m_range.kMax);
	voi->Update();

	// Create the isosurface
	vtkSmartPointer<vtkContourFilter> contourFilter = vtkSmartPointer<vtkContourFilter>::New();
	contourFilter->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, m_target.c_str());
	contourFilter->SetInputConnection(voi->GetOutputPort());
	contourFilter->GenerateValues(1, m_isoValue, m_isoValue);

	// Map the data to graphical primitives
	m_isoSurfaceMapper->SetInputConnection(contourFilter->GetOutputPort());
	updateColorSetting();

	m_actorCollection->AddItem(m_isoSurfaceActor);
}

void Post3dWindowNodeScalarGroupDataItem::updateVisibility()
{
	bool ancientVisible = isAncientChecked();
	updateVisibility(ancientVisible);

	renderGraphicsView();
}

void Post3dWindowNodeScalarGroupDataItem::updateVisibility(bool visible)
{
	GraphicsWindowDataItem::updateVisibility(visible);
}

QDialog* Post3dWindowNodeScalarGroupDataItem::propertyDialog(QWidget* p)
{
	Post3dWindowIsosurfaceSettingDialog* dialog = new Post3dWindowIsosurfaceSettingDialog(p);
	Post3dWindowGridTypeDataItem* gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*>(parent()->parent()->parent());
	dialog->setGridTypeDataItem(gtItem);
	Post3dWindowZoneDataItem* zItem = dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent());

	if (zItem->dataContainer() == nullptr || zItem->dataContainer()->data() == nullptr) {return nullptr;}

	dialog->setEnabled(true);
	dialog->setZoneData(zItem->dataContainer());
	dialog->setTarget(m_target);

	// it's made enabled ALWAYS.
	//	dialog->setEnabled(isEnabled());
	dialog->setFullRange(m_fullRange);
	dialog->setRange(m_range);

	dialog->setIsoValue(m_isoValue);
	dialog->setColor(m_color);
	dialog->setOpacity(m_opacity);

	return dialog;
}

void Post3dWindowNodeScalarGroupDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	Post3dWindowIsosurfaceSettingDialog* dialog = dynamic_cast<Post3dWindowIsosurfaceSettingDialog*>(propDialog);
	iRICUndoStack::instance().push(
		new SetSettingCommand(
			dialog->enabled(), dialog->target(),
			dialog->fullRange(), dialog->range(),
			dialog->isoValue(), dialog->color(), dialog->opacity(), this));
}

void Post3dWindowNodeScalarGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Isosurface physical value change"));
	pushRenderCommand(cmd, this, true);
}

std::string Post3dWindowNodeScalarGroupDataItem::target() const
{
	return m_target;
}

void Post3dWindowNodeScalarGroupDataItem::setTarget(const std::string& target)
{
	m_target = target;
	updateActorSettings();
}

void Post3dWindowNodeScalarGroupDataItem::innerUpdateZScale(double scale)
{
	m_isoSurfaceActor->SetScale(1, 1, scale);
}

void Post3dWindowNodeScalarGroupDataItem::updateColorSetting()
{
	m_isoSurfaceActor->GetProperty()->SetColor(m_color.red()/255., m_color.green()/255., m_color.blue()/255.);
	m_isoSurfaceActor->GetProperty()->SetOpacity(m_opacity / 100.0);
}

void Post3dWindowNodeScalarGroupDataItem::validateRange()
{
	Post3dWindowZoneDataItem* zItem = dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent());
	if (zItem->dataContainer() == nullptr || zItem->dataContainer()->data() == nullptr)	{
		return;
	}
	vtkStructuredGrid* g = dynamic_cast<vtkStructuredGrid*>(zItem->dataContainer()->data());
	int dims[3];
	g->GetDimensions(dims);
	if (m_fullRange) {
		m_range.iMin = 0;  m_range.iMax = dims[0] - 1;
		m_range.jMin = 0;  m_range.jMax = dims[1] - 1;
		m_range.kMin = 0;  m_range.kMax = dims[2] - 1;
	}
	else {
		if (m_range.iMin < 0) m_range.iMin = 0;
		if (m_range.jMin < 0) m_range.jMin = 0;
		if (m_range.kMin < 0) m_range.kMin = 0;
		if (m_range.iMax < 0) m_range.iMax = dims[0] - 1;
		if (m_range.jMax < 0) m_range.jMax = dims[1] - 1;
		if (m_range.kMax < 0) m_range.kMax = dims[2] - 1;
	}
}

void Post3dWindowNodeScalarGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(dataModel()->objectBrowserView()->undoableDeleteAction());
}
