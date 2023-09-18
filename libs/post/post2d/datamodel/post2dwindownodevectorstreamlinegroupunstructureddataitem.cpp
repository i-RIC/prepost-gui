#include "../post2dwindowgraphicsview.h"
#include "post2dwindownodevectorstreamlinegroupunstructureddataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindownodevectorstreamlinegroupunstructureddataitem_impl.h"
#include "private/post2dwindownodevectorstreamlinegroupunstructureddataitem_setting.h"
#include "private/post2dwindownodevectorstreamlinegroupunstructureddataitem_settingeditwidget.h"

#include <guibase/objectbrowserview.h>
#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/base/iricmainwindowi.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/grid/v4grid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QMainWindow>
#include <QMouseEvent>
#include <QSettings>
#include <QStandardItem>

#include <vtkProperty.h>
#include <vtkRenderer.h>

Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem(Post2dWindowDataItem* parent) :
	Post2dWindowNodeVectorStreamlineGroupDataItem(parent),
	impl {new Impl {}}
{
	setupDefaultValues();
	setupPreviewActor();
}

Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::~Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem()
{
	renderer()->RemoveActor(impl->m_previewActor.pointsActor());

	clearActors();
}

QDialog* Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::propertyDialog(QWidget* p)
{
	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);

	connect(this, &Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::point1Changed, widget, &SettingEditWidget::handleMousePoint1Change);
	connect(this, &Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::point2Changed, widget, &SettingEditWidget::handleMousePoint2Change);

	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Streamline Display Setting"));
	dialog->resize(450, 400);

	return dialog;
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::setupDefaultValues()
{
	QSettings settings;

	impl->m_setting.startPositions.clear();

	Setting::StartPosition pos;
	pos.pointsSet = false;
	pos.numberOfPoints = 10;
	pos.color = settings.value("graphics/streamline_color", QColor(Qt::black)).value<QColor>();
	pos.width = settings.value("graphics/streamline_width", 1).toInt();

	impl->m_setting.startPositions.push_back(pos);
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::updatePreview(const QPointF p1, const QPointF p2, int numPoints)
{
	auto col = actorCollection();

	col->RemoveItem(impl->m_previewActor.pointsActor());
	impl->m_previewActor.setLine(generatePoints(p1, p2, numPoints));
	col->AddItem(impl->m_previewActor.pointsActor());

	updateVisibilityWithoutRendering();
}

std::vector<QPointF> Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::generatePoints(const QPointF p1, const QPointF p2, int numPoints)
{
	QPointF diffVec = p2 - p1;

	std::vector<QPointF> ret;
	for (int i = 0; i < numPoints; ++i) {
		double param = i / static_cast<double> (numPoints - 1);
		QPointF v = p1 + param * diffVec;

		ret.push_back(v);
	}
	return ret;
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::disablePreviewActor()
{
	impl->m_previewActor.pointsActor()->VisibilityOff();
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::setupPreviewActor()
{
	auto actor = impl->m_previewActor.pointsActor();
	actor->VisibilityOff();
	renderer()->AddActor(actor);

	auto v = dataModel()->graphicsView();
	auto prop = actor->GetProperty();
	prop->SetPointSize(3 * v->devicePixelRatioF());
	prop->LightingOff();
	prop->SetColor(0, 0, 0);
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::setupActors()
{
	clearActors();

	auto cont = zoneDataItem()->v4DataContainer();
	auto grid = cont->gridData()->grid()->vtkData()->data();

	auto regionData = impl->m_setting.region.buildNodeFilteredData(grid);
	auto r = renderer();
	auto col = actorCollection();

	const auto& positions = impl->m_setting.startPositions;
	for (const auto& pos : positions) {
		auto points = vtkSmartPointer<vtkPoints>::New();
		points->SetDataTypeToDouble();

		auto plist = generatePoints(pos.point1, pos.point2, pos.numberOfPoints);
		for (const auto& p : plist) {
			points->InsertNextPoint(p.x(), p.y(), 0);
		}
		auto polyData = vtkSmartPointer<vtkPolyData>::New();
		polyData->SetPoints(points);
		auto cells = vtkSmartPointer<vtkCellArray>::New();
		for (int i = 0; i < plist.size(); ++i) {
			vtkIdType id = i;
			cells->InsertNextCell(1, &id);
		}
		polyData->SetVerts(cells);

		auto tracer = vtkSmartPointer<vtkStreamTracer>::New();
		setupStreamTracer(tracer);
		tracer->SetSourceData(polyData);
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

		actor->Delete();
	}
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	Post2dWindowNodeVectorStreamlineGroupDataItem::doLoadFromProjectMainFile(node);
	impl->m_setting.load(node);
	updateActorSetting();
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	Post2dWindowNodeVectorStreamlineGroupDataItem::doSaveToProjectMainFile(writer);
	impl->m_setting.save(writer);
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::hidePreviewSetting()
{
	actor2DCollection()->RemoveItem(impl->m_previewActor.pointsActor());
	impl->m_previewActor.pointsActor()->VisibilityOff();
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto view = dynamic_cast<VTK2DGraphicsView*> (v);
	QPointF pos = view->viewportToWorld(event->pos());

	emit point1Changed(pos);
	emit point2Changed(pos);

	impl->m_dragging = true;
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	Post2dWindowNodeVectorStreamlineGroupDataItem::mouseReleaseEvent(event, v);
	auto view = dynamic_cast<VTK2DGraphicsView*> (v);
	QPointF pos = view->viewportToWorld(event->pos());

	emit point2Changed(pos);

	impl->m_dragging = false;
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	Post2dWindowNodeVectorStreamlineGroupDataItem::mouseMoveEvent(event, v);
	if (! impl->m_dragging) {return;}

	auto view = dynamic_cast<VTK2DGraphicsView*> (v);
	QPointF pos = view->viewportToWorld(event->pos());

	emit point2Changed(pos);
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::assignActorZValues(const ZDepthRange& range)
{
	impl->m_previewActor.pointsActor()->SetPosition(0, 0, range.max());
	Post2dWindowNodeVectorStreamlineGroupDataItem::assignActorZValues(range);
}
