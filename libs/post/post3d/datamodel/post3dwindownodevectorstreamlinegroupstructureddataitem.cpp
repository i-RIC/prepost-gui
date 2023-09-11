#include "../post3dwindowgraphicsview.h"
#include "post3dwindownodevectorstreamlinegroupstructureddataitem.h"
#include "post3dwindowzonedataitem.h"
#include "private/post3dwindownodevectorstreamlinegroupstructureddataitem_impl.h"
#include "private/post3dwindownodevectorstreamlinegroupstructureddataitem_settingeditwidget.h"

#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <vtkGeometryFilter.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkStructuredGrid.h>

Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::Post3dWindowNodeVectorStreamlineGroupStructuredDataItem(Post3dWindowDataItem* parent) :
	Post3dWindowNodeVectorStreamlineGroupDataItem(parent),
	impl {new Impl {}}
{
	setupDefaultValues();
}

Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::~Post3dWindowNodeVectorStreamlineGroupStructuredDataItem()
{
	delete impl;
}

void Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

void Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::setupActors()
{
	clearActors();

	auto zoneContainer = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	auto grid = vtkStructuredGrid::SafeDownCast(zoneContainer->data()->data());

	auto r = renderer();
	auto col = actorCollection();

	const auto& positions = impl->m_setting.startPositions;
	for (const auto& pos : positions) {
		auto grid2 = pos.face.extractFace(grid);
		auto grid3 = pos.skipOrSubdivide.buildSampledData(grid2, StructuredGridSkipOrSubdivideSettingContainer::Dimension::Three);
		grid2->Delete();
		auto filter = vtkSmartPointer<vtkGeometryFilter>::New();
		filter->SetInputData(grid3);
		filter->Update();

		auto tracer = vtkSmartPointer<vtkStreamTracer>::New();
		setupStreamTracer(tracer);
		tracer->SetSourceData(filter->GetOutput());
		tracer->SetInputData(grid);
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

void Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::setupDefaultValues()
{
	QSettings settings;

	impl->m_setting.startPositions.clear();

	auto cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	auto grid = vtkStructuredGrid::SafeDownCast(cont->data()->data());
	int dim[3];
	grid->GetDimensions(dim);

	Setting::StartPosition pos;
	pos.face.direction = Post3dWindowFaceSettingContainer::Direction::dirI;
	pos.face.iMin = 0;
	pos.face.iMax = 0;
	pos.face.jMin = 0;
	pos.face.jMax = dim[1] - 1;
	pos.face.kMin = 0;
	pos.face.kMax = dim[2] - 1;
	pos.color = settings.value("graphics/streamline_color", QColor(Qt::black)).value<QColor>();
	pos.width = settings.value("graphics/streamline_width", 1).toInt();

	impl->m_setting.startPositions.push_back(pos);
}

QDialog* Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::propertyDialog(QWidget* p)
{
	if (zoneDataItem()->dataContainer() == nullptr) {return nullptr;}

	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Streamline Display Setting"));

	return dialog;
}

void Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.load(node);
	Post3dWindowNodeVectorStreamlineGroupDataItem::doLoadFromProjectMainFile(node);
}

void Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	Post3dWindowNodeVectorStreamlineGroupDataItem::doSaveToProjectMainFile(writer);
	impl->m_setting.save(writer);
}
