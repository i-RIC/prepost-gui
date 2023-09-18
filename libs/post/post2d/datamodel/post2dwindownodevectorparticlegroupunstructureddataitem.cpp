#include "../post2dwindowgraphicsview.h"
#include "post2dwindownodevectorparticlegroupunstructureddataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindownodevectorparticlegroupunstructureddataitem_impl.h"
#include "private/post2dwindownodevectorparticlegroupunstructureddataitem_settingeditwidget.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/grid/v4grid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>

Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Post2dWindowNodeVectorParticleGroupUnstructuredDataItem(Post2dWindowDataItem* parent) :
	Post2dWindowNodeVectorParticleGroupDataItem {parent},
	impl {new Impl {}}
{
	setupDefaultValues();
	setupPreviewActor();
}

Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::~Post2dWindowNodeVectorParticleGroupUnstructuredDataItem()
{}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::assignActorZValues(const ZDepthRange& range)
{
	impl->m_previewActor.pointsActor()->SetPosition(0, 0, range.max());
	Post2dWindowNodeVectorParticleGroupDataItem::assignActorZValues(range);
}

int Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::numberOfActors()
{
	return static_cast<int> (impl->m_setting.startPositions.size());
}

vtkActor* Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::setupActor(int i)
{
	auto view = dataModel()->graphicsView();
	const auto& pos = impl->m_setting.startPositions.at(i);

	auto actor = vtkActor::New();
	auto prop = actor->GetProperty();

	prop->SetColor(pos.color);
	prop->SetPointSize(pos.pointSize * view->devicePixelRatioF());

	return actor;
}

vtkPolyData* Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::newParticles(int i)
{
	const auto& pos = impl->m_setting.startPositions.at(i);

	auto points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();

	auto plist = generatePoints(pos.point1, pos.point2, pos.numberOfPoints);
	for (const auto& p : plist) {
		points->InsertNextPoint(p.x(), p.y(), 0);
	}

	return setupPolyDataFromPoints(points);
}

vtkDataSet* Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::getRegion()
{
	auto cont = zoneDataItem()->v4DataContainer();
	auto grid = cont->gridData()->grid()->vtkData()->data();

	return impl->m_setting.region.buildNodeFilteredData(grid);
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::setupPreviewActor()
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

QDialog* Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::propertyDialog(QWidget* parent)
{
	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, parent);
	auto widget = new SettingEditWidget(this, dialog);

	connect(this, &Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::point1Changed, widget, &SettingEditWidget::handleMousePoint1Change);
	connect(this, &Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::point2Changed, widget, &SettingEditWidget::handleMousePoint2Change);

	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Particles Display Setting"));

	return dialog;
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.startPositions.clear();
	impl->m_setting.load(node);

	Post2dWindowNodeVectorParticleGroupDataItem::doLoadFromProjectMainFile(node);
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	Post2dWindowNodeVectorParticleGroupDataItem::doSaveToProjectMainFile(writer);
	impl->m_setting.save(writer);
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto view = dynamic_cast<VTK2DGraphicsView*> (v);
	QPointF pos = view->viewportToWorld(event->pos());

	emit point1Changed(pos);
	emit point2Changed(pos);

	impl->m_dragging = true;
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	Post2dWindowNodeVectorParticleGroupDataItem::mouseReleaseEvent(event, v);

	auto view = dynamic_cast<VTK2DGraphicsView*> (v);
	QPointF pos = view->viewportToWorld(event->pos());

	emit point2Changed(pos);

	impl->m_dragging = false;
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	Post2dWindowNodeVectorParticleGroupDataItem::mouseMoveEvent(event, v);

	if (! impl->m_dragging) {return;}

	auto view = dynamic_cast<VTK2DGraphicsView*> (v);
	QPointF pos = view->viewportToWorld(event->pos());

	emit point2Changed(pos);
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::hidePreviewSetting()
{
	actor2DCollection()->RemoveItem(impl->m_previewActor.pointsActor());
	impl->m_previewActor.pointsActor()->VisibilityOff();
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::setupDefaultValues()
{
	QSettings settings;

	impl->m_setting.startPositions.clear();

	Setting::StartPosition pos;
	pos.pointsSet = false;
	pos.numberOfPoints = 10;
	pos.color = settings.value("graphics/particle_color", QColor(Qt::black)).value<QColor>();
	pos.pointSize = settings.value("graphics/particle_size", ParticleSettingContainer::DEFAULT_SIZE).toInt();

	impl->m_setting.startPositions.push_back(pos);
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::updatePreview(const QPointF p1, const QPointF p2, int numPoints)
{
	auto col = actorCollection();

	col->RemoveItem(impl->m_previewActor.pointsActor());
	impl->m_previewActor.setLine(generatePoints(p1, p2, numPoints));
	col->AddItem(impl->m_previewActor.pointsActor());

	updateVisibilityWithoutRendering();
}

std::vector<QPointF> Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::generatePoints(const QPointF p1, const QPointF p2, int numPoints)
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

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::disablePreviewActor()
{
	impl->m_previewActor.pointsActor()->VisibilityOff();
}
