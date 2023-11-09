#include "../post3dwindowgraphicsview.h"
#include "post3dwindownodevectorparticlegroupstructureddataitem.h"
#include "post3dwindowzonedataitem.h"
#include "private/post3dwindownodevectorparticlegroupstructureddataitem_impl.h"
#include "private/post3dwindownodevectorparticlegroupstructureddataitem_settingeditwidget.h"

#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/grid/v4structured3dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>

#include <QSettings>

Post3dWindowNodeVectorParticleGroupStructuredDataItem::Post3dWindowNodeVectorParticleGroupStructuredDataItem(Post3dWindowDataItem* parent) :
	Post3dWindowNodeVectorParticleGroupDataItem {parent},
	impl {new Impl {}}
{
	setDefaultValues();
}

Post3dWindowNodeVectorParticleGroupStructuredDataItem::~Post3dWindowNodeVectorParticleGroupStructuredDataItem()
{}

void Post3dWindowNodeVectorParticleGroupStructuredDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

int Post3dWindowNodeVectorParticleGroupStructuredDataItem::numberOfActors()
{
	return static_cast<int> (impl->m_setting.startPositions.size());
}

vtkActor* Post3dWindowNodeVectorParticleGroupStructuredDataItem::setupActor(int i)
{
	auto view = dataModel()->graphicsView();
	const auto& pos = impl->m_setting.startPositions.at(i);

	auto actor = vtkActor::New();
	auto prop = actor->GetProperty();

	prop->SetColor(pos.color);
	prop->SetPointSize(pos.pointSize * view->devicePixelRatioF());

	return actor;
}

vtkPolyData* Post3dWindowNodeVectorParticleGroupStructuredDataItem::newParticles(int i)
{
	auto cont = zoneDataItem()->v4DataContainer();
	auto grid = dynamic_cast<v4Structured3dGrid*> (cont->gridData()->grid())->vtkConcreteData()->concreteData();

	const auto& pos = impl->m_setting.startPositions.at(i);

	auto grid2 = pos.face.extractFace(grid);
	auto grid3 = pos.skipOrSubdivide.buildSampledData(grid2, StructuredGridSkipOrSubdivideSettingContainer::Dimension::Three);
	grid2->Delete();
	auto filter = vtkSmartPointer<vtkGeometryFilter>::New();
	filter->SetInputData(grid3);
	filter->Update();

	auto ret = filter->GetOutput();
	ret->Register(nullptr);
	grid3->Delete();

	return ret;
}

vtkDataSet* Post3dWindowNodeVectorParticleGroupStructuredDataItem::getRegion()
{
	auto cont = zoneDataItem()->v4DataContainer();
	auto grid = dynamic_cast<v4Structured3dGrid*> (cont->gridData()->grid())->vtkConcreteData()->concreteData();
	grid->Register(nullptr);
	return grid;
}

QDialog* Post3dWindowNodeVectorParticleGroupStructuredDataItem::propertyDialog(QWidget* p)
{
	if (zoneDataItem()->v4DataContainer() == nullptr) {return nullptr;}

	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Particles Display Setting"));

	return dialog;
}

void Post3dWindowNodeVectorParticleGroupStructuredDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.startPositions.clear();
	impl->m_setting.load(node);

	Post3dWindowNodeVectorParticleGroupDataItem::doLoadFromProjectMainFile(node);
}

void Post3dWindowNodeVectorParticleGroupStructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	Post3dWindowNodeVectorParticleGroupDataItem::doSaveToProjectMainFile(writer);

	impl->m_setting.save(writer);
}

void Post3dWindowNodeVectorParticleGroupStructuredDataItem::setDefaultValues()
{
	QSettings settings;

	impl->m_setting.startPositions.clear();

	auto cont = zoneDataItem()->v4DataContainer();
	auto grid = dynamic_cast<v4Structured3dGrid*> (cont->gridData()->grid())->vtkConcreteData()->concreteData();
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
	pos.color = settings.value("graphics/particle_color", QColor(Qt::black)).value<QColor>();
	pos.pointSize = settings.value("graphics/particle_size", ParticleSettingContainer::DEFAULT_SIZE).toInt();

	impl->m_setting.startPositions.push_back(pos);
}
