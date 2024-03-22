#include "post2dwindowparticleimagedataitem.h"
#include "post2dwindowparticleimagetopdataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowparticleimagedataitem_impl.h"
#include "private/post2dwindowparticleimagedataitem_settingeditwidget.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/grid/v4grid.h>

#include <vtkPolyDataMapper.h>
#include <vtkTexture.h>
#include <vtkTextureMapToPlane.h>

Post2dWindowParticleImageDataItem::Post2dWindowParticleImageDataItem(const std::string& groupName, const QString& caption, Post2dWindowDataItem* parent) :
	Post2dWindowDataItem(caption, QIcon(":/libs/guibase/images/iconPaper.svg"), parent),
	m_groupName {groupName},
	impl {new Impl {}}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
}

Post2dWindowParticleImageDataItem::~Post2dWindowParticleImageDataItem()
{
	delete impl;
}

const std::string& Post2dWindowParticleImageDataItem::name() const
{
	return m_groupName;
}

void Post2dWindowParticleImageDataItem::update()
{
	updateActorSetting();
}

QDialog* Post2dWindowParticleImageDataItem::propertyDialog(QWidget* p)
{
	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Particles (image) setting"));
	dialog->resize(600, 500);

	return dialog;
}

void Post2dWindowParticleImageDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

Post2dWindowParticleImageTopDataItem* Post2dWindowParticleImageDataItem::topDataItem() const
{
	return dynamic_cast<Post2dWindowParticleImageTopDataItem*> (parent());
}

Post2dWindowZoneDataItem* Post2dWindowParticleImageDataItem::zoneDataItem() const
{
	return topDataItem()->zoneDataItem();
}

void Post2dWindowParticleImageDataItem::updateActorSetting()
{
	auto texture = impl->buildTexture();
	auto plane = impl->buildPlane();

	auto mapToPlane = vtkSmartPointer<vtkTextureMapToPlane>::New();
	mapToPlane->SetInputData(plane);
	mapToPlane->AutomaticPlaneGenerationOn();
	plane->Delete();

	auto cont = zoneDataItem()->v4DataContainer();
	auto particles = cont->particleGroupImage(m_groupName);
	if (particles == nullptr) {return;}

	auto pd = particles->grid()->vtkData()->data();
	auto points = pd->GetPoints();
	auto vals = vtkDoubleArray::SafeDownCast(pd->GetPointData()->GetArray("size"));
	auto angles = vtkDoubleArray::SafeDownCast(pd->GetPointData()->GetArray("angle"));


	for (auto a : impl->m_actors) {
		renderer()->RemoveActor(a);
		a->Delete();
	}
	impl->m_actors.clear();
	m_actorCollection->RemoveAllItems();

	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
		double v[3];
		points->GetPoint(i, v);
		double val = vals->GetValue(i);
		double angle = angles->GetValue(i);
		QPointF pos(v[0], v[1]);

		auto mapper = vtkPolyDataMapper::New();
		mapper->SetInputConnection(mapToPlane->GetOutputPort());

		auto actor = vtkActor::New();
		actor->SetMapper(mapper);
		actor->SetTexture(texture);

		auto bottomLeftCorner = impl->buildBottomLeftCorner(pos, val, angle);
		actor->SetPosition(bottomLeftCorner.x(), bottomLeftCorner.y(), 0);
		auto scale = impl->calcScale(val);
		actor->SetScale(scale, scale, 1);
		actor->SetOrientation(0, 0, angle);
		actor->GetProperty()->SetOpacity(impl->m_setting.opacity);

		renderer()->AddActor(actor);
		m_actorCollection->AddItem(actor);
		impl->m_actors.push_back(actor);
	}

	texture->Delete();
}

void Post2dWindowParticleImageDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.load(node);

	updateActorSetting();
}

void Post2dWindowParticleImageDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_setting.save(writer);
}
