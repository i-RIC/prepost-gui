#include "../post2dwindowgraphicsview.h"
#include "post2dwindownodevectorparticlegroupstructureddataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindownodevectorparticlegroupstructureddataitem_impl.h"
#include "private/post2dwindownodevectorparticlegroupstructureddataitem_settingeditwidget.h"

#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>

#include <QSettings>

Post2dWindowNodeVectorParticleGroupStructuredDataItem::Post2dWindowNodeVectorParticleGroupStructuredDataItem(Post2dWindowDataItem* parent) :
	Post2dWindowNodeVectorParticleGroupDataItem {parent},
	impl {new Impl {}}
{
	setDefaultValues();
}

Post2dWindowNodeVectorParticleGroupStructuredDataItem::~Post2dWindowNodeVectorParticleGroupStructuredDataItem()
{}

void Post2dWindowNodeVectorParticleGroupStructuredDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

int Post2dWindowNodeVectorParticleGroupStructuredDataItem::numberOfActors()
{
	return static_cast<int> (impl->m_setting.startPositions.size());
}

vtkActor* Post2dWindowNodeVectorParticleGroupStructuredDataItem::setupActor(int i)
{
	auto view = dataModel()->graphicsView();
	const auto& pos = impl->m_setting.startPositions.at(i);

	auto actor = vtkActor::New();
	auto prop = actor->GetProperty();

	prop->SetColor(pos.color);
	prop->SetPointSize(pos.pointSize * view->devicePixelRatioF());

	return actor;
}

vtkPolyData* Post2dWindowNodeVectorParticleGroupStructuredDataItem::newParticles(int i)
{
	auto zoneContainer = zoneDataItem()->v4DataContainer();
	auto sGrid = dynamic_cast<v4Structured2dGrid*>(zoneContainer->gridData()->grid());

	const auto& pos = impl->m_setting.startPositions.at(i);

	auto grid2 = pos.range.buildNodeFilteredData(sGrid->vtkConcreteData()->concreteData());
	auto grid3 = pos.skipOrSubdivide.buildSampledData(grid2, StructuredGridSkipOrSubdivideSettingContainer::Dimension::Two);
	grid2->Delete();
	auto filter = vtkSmartPointer<vtkGeometryFilter>::New();
	filter->SetInputData(grid3);
	filter->Update();

	auto ret = filter->GetOutput();
	ret->Register(nullptr);
	grid3->Delete();

	return ret;
}

vtkDataSet* Post2dWindowNodeVectorParticleGroupStructuredDataItem::getRegion()
{
	auto cont = zoneDataItem()->v4DataContainer();
	auto sGrid = dynamic_cast<v4Structured2dGrid*>(cont->gridData()->grid());

	return impl->m_setting.region.buildNodeFilteredData(sGrid->vtkConcreteData()->concreteData());
}

QDialog* Post2dWindowNodeVectorParticleGroupStructuredDataItem::propertyDialog(QWidget* p)
{
	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Particles Display Setting"));

	return dialog;
}

void Post2dWindowNodeVectorParticleGroupStructuredDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.startPositions.clear();
	impl->m_setting.load(node);

	Post2dWindowNodeVectorParticleGroupDataItem::doLoadFromProjectMainFile(node);
}

void Post2dWindowNodeVectorParticleGroupStructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	Post2dWindowNodeVectorParticleGroupDataItem::doSaveToProjectMainFile(writer);

	impl->m_setting.save(writer);
}

void Post2dWindowNodeVectorParticleGroupStructuredDataItem::setDefaultValues()
{
	QSettings settings;

	impl->m_setting.startPositions.clear();

	auto cont = zoneDataItem()->v4DataContainer();
	auto sGrid = dynamic_cast<v4Structured2dGrid*>(cont->gridData()->grid());
	int dim[3];
	sGrid->vtkConcreteData()->concreteData()->GetDimensions(dim);

	Setting::StartPosition pos;
	pos.range.iMin = 0;
	pos.range.iMax = 0;
	pos.range.jMin = 0;
	pos.range.jMax = dim[1] - 1;
	pos.color = settings.value("graphics/particle_color", QColor(Qt::black)).value<QColor>();
	pos.pointSize = settings.value("graphics/particle_size", ParticleSettingContainer::DEFAULT_SIZE).toInt();

	impl->m_setting.startPositions.push_back(pos);
}
