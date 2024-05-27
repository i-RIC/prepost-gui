#include "../../../guibase/objectbrowserview.h"
#include "../post3dwindowdatamodel.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindownodescalargroupdataitem.h"
#include "post3dwindowzonedataitem.h"
#include "private/post3dwindownodescalargroupdataitem_impl.h"
#include "private/post3dwindownodescalargroupdataitem_settingeditwidget.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/grid/v4structured3dgrid.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>

Post3dWindowNodeScalarGroupDataItem::Impl::Impl() :
	m_isoSurfaceActor {vtkActor::New()},
	m_isoSurfaceMapper {vtkPolyDataMapper::New()}
{}

Post3dWindowNodeScalarGroupDataItem::Impl::~Impl()
{
	m_isoSurfaceActor->Delete();
	m_isoSurfaceMapper->Delete();
}

Post3dWindowNodeScalarGroupDataItem::Post3dWindowNodeScalarGroupDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem {tr("Isosurface"), QIcon(":/libs/guibase/images/iconPaper.svg"), p},
	impl {new Impl {}}
{
	setupStandardItem(Checked, NotReorderable, Deletable);

	setDefaultValues();
	setupActors();
}

Post3dWindowNodeScalarGroupDataItem::Post3dWindowNodeScalarGroupDataItem(const std::string& target, Post3dWindowDataItem* parent) :
	Post3dWindowNodeScalarGroupDataItem(parent)
{
	impl->m_setting.target = target.c_str();
}

Post3dWindowNodeScalarGroupDataItem::~Post3dWindowNodeScalarGroupDataItem()
{
	renderer()->RemoveActor(impl->m_isoSurfaceActor);
}

void Post3dWindowNodeScalarGroupDataItem::setDefaultValues()
{
	auto zItem = topDataItem()->zoneDataItem();
	auto cont = zItem->v4DataContainer();
	if (cont == nullptr || cont->gridData() == nullptr)	{
		return;
	}
	auto g = dynamic_cast<v4Structured3dGrid*> (zItem->v4DataContainer()->gridData()->grid())->vtkConcreteData()->concreteData();
	int dims[3];
	g->GetDimensions(dims);

	impl->m_setting.range.iMin = 0;
	impl->m_setting.range.jMin = 0;
	impl->m_setting.range.kMin = 0;
	impl->m_setting.range.iMax = dims[0] - 1;
	impl->m_setting.range.jMax = dims[1] - 1;
	impl->m_setting.range.kMax = dims[2] - 1;
}

void Post3dWindowNodeScalarGroupDataItem::updateActorSetting()
{
	impl->m_isoSurfaceActor->VisibilityOff();
	m_actorCollection->RemoveAllItems();

	auto cont = topDataItem()->zoneDataItem()->v4DataContainer();
	if (cont == nullptr) {return;}

	auto ps = cont->gridData()->grid()->vtkData()->data();
	if (ps == nullptr) {return;}
	if (impl->m_setting.target == "") {return;}

	auto caption = cont->gridType()->output(impl->m_setting.target)->caption();

	m_standardItem->setText(caption);
	m_standardItemCopy->setText(caption);

	// update current active scalar
	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0) {return;}

	// input data

	// extract interest volume
	auto voi = vtkSmartPointer<vtkExtractGrid>::New();
	voi->SetInputData(ps);
	voi->SetVOI(
		impl->m_setting.range.iMin, impl->m_setting.range.iMax,
		impl->m_setting.range.jMin, impl->m_setting.range.jMax,
		impl->m_setting.range.kMin, impl->m_setting.range.kMax);
	voi->Update();

	// Create the isosurface
	vtkSmartPointer<vtkContourFilter> contourFilter = vtkSmartPointer<vtkContourFilter>::New();
	contourFilter->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, iRIC::toStr(impl->m_setting.target).c_str());
	contourFilter->SetInputConnection(voi->GetOutputPort());
	contourFilter->GenerateValues(1, impl->m_setting.isoValue, impl->m_setting.isoValue);

	// Map the data to graphical primitives
	impl->m_isoSurfaceMapper->SetInputConnection(contourFilter->GetOutputPort());

	impl->m_isoSurfaceActor->GetProperty()->SetColor(impl->m_setting.color);
	impl->m_isoSurfaceActor->GetProperty()->SetOpacity(impl->m_setting.opacity);

	m_actorCollection->AddItem(impl->m_isoSurfaceActor);

	updateVisibilityWithoutRendering();
}

void Post3dWindowNodeScalarGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.load(node);

	updateActorSetting();
}

void Post3dWindowNodeScalarGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_setting.save(writer);
}

void Post3dWindowNodeScalarGroupDataItem::setupActors()
{
	// Mapper for graphical primitives
	impl->m_isoSurfaceMapper->ScalarVisibilityOff();

	// Create an actor for the Isosurface
	impl->m_isoSurfaceActor->SetMapper(impl->m_isoSurfaceMapper);

	renderer()->AddActor(impl->m_isoSurfaceActor);
}

void Post3dWindowNodeScalarGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void Post3dWindowNodeScalarGroupDataItem::update()
{
	updateActorSetting();
}

Post3dWindowNodeScalarGroupTopDataItem* Post3dWindowNodeScalarGroupDataItem::topDataItem() const
{
	return dynamic_cast<Post3dWindowNodeScalarGroupTopDataItem*>(parent());
}

QDialog* Post3dWindowNodeScalarGroupDataItem::propertyDialog(QWidget* p)
{
	auto zItem = topDataItem()->zoneDataItem();
	auto cont = zItem->v4DataContainer();
	if (cont == nullptr || cont->gridData() == nullptr) {return nullptr;}

	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);

	auto gtItem = zItem->gridTypeDataItem();
	widget->setGridTypeDataItem(gtItem);
	widget->setZoneData(cont);

	widget->setSetting(impl->m_setting);

	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Isosurface Setting"));

	return dialog;
}

void Post3dWindowNodeScalarGroupDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

void Post3dWindowNodeScalarGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Isosurface physical value change"));
	pushRenderCommand(cmd, this, true);
}

std::string Post3dWindowNodeScalarGroupDataItem::target() const
{
	return impl->m_setting.target;
}

void Post3dWindowNodeScalarGroupDataItem::setTarget(const std::string& target)
{
	impl->m_setting.target = target.c_str();
	updateActorSetting();
}

void Post3dWindowNodeScalarGroupDataItem::innerUpdateZScale(double scale)
{
	impl->m_isoSurfaceActor->SetScale(1, 1, scale);
}
