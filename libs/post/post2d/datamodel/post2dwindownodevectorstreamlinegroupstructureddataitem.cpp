#include "../post2dwindowgraphicsview.h"
#include "post2dwindownodevectorstreamlinegroupstructureddataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindownodevectorstreamlinegroupstructureddataitem_impl.h"
#include "private/post2dwindownodevectorstreamlinegroupstructureddataitem_settingeditwidget.h"

#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>
#include <misc/modifycommanddialog.h>

#include <vtkGeometryFilter.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredGrid.h>

Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Post2dWindowNodeVectorStreamlineGroupStructuredDataItem(Post2dWindowDataItem* parent) :
	Post2dWindowNodeVectorStreamlineGroupDataItem {parent},
	impl {new Impl {}}
{
	setupDefaultValues();
}

Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::~Post2dWindowNodeVectorStreamlineGroupStructuredDataItem()
{
	delete impl;
}

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::setupDefaultValues()
{
	QSettings settings;

	impl->m_setting.startPositions.clear();

	auto cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	auto grid = vtkStructuredGrid::SafeDownCast(cont->data()->data());
	int dim[3];
	grid->GetDimensions(dim);

	Setting::StartPosition pos;
	pos.range.iMin = 0;
	pos.range.iMax = 0;
	pos.range.jMin = 0;
	pos.range.jMax = dim[1] - 1;
	pos.color = settings.value("graphics/streamline_color", QColor(Qt::black)).value<QColor>();
	pos.width = settings.value("graphics/streamline_width", 1).toInt();

	impl->m_setting.startPositions.push_back(pos);
}

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::setupActors()
{
	clearActors();

	auto zoneContainer = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	auto grid = vtkStructuredGrid::SafeDownCast(zoneContainer->data()->data());

	auto regionData = impl->m_setting.region.buildNodeFilteredData(grid);
	auto r = renderer();
	auto col = actorCollection();

	const auto& positions = impl->m_setting.startPositions;
	for (const auto& pos : positions) {
		auto grid2 = pos.range.buildNodeFilteredData(grid);
		auto grid3 = pos.skipOrSubdivide.buildSampledData(grid2, StructuredGridSkipOrSubdivideSettingContainer::Dimension::Two);
		grid2->Delete();
		auto filter = vtkSmartPointer<vtkGeometryFilter>::New();
		filter->SetInputData(grid3);
		filter->Update();

		auto tracer = vtkSmartPointer<vtkStreamTracer>::New();
		setupStreamTracer(tracer);
		tracer->SetSourceData(filter->GetOutput());
		tracer->SetInputData(regionData);
		tracer->SetInputArrayToProcess(0, 0, 0, 0, iRIC::toStr(m_setting.target).c_str());

		auto mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputConnection(tracer->GetOutputPort());
		auto actor = vtkActor::New();
		actor->SetMapper(mapper);
		mapper->Delete();

		actor->GetProperty()->SetLineWidth(pos.width);
		actor->GetProperty()->SetColor(pos.color);

		r->AddActor(actor);
		col->AddItem(actor);
		m_streamlineActors.push_back(actor);

		grid3->Delete();
		actor->Delete();
	}
}

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::propertyDialog(QWidget* p)
{
	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Streamline Display Setting"));

	return dialog;
}

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.load(node);
	Post2dWindowNodeVectorStreamlineGroupDataItem::doLoadFromProjectMainFile(node);
}

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	Post2dWindowNodeVectorStreamlineGroupDataItem::doSaveToProjectMainFile(writer);
	impl->m_setting.save(writer);
}
