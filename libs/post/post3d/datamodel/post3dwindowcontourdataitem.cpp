#include "post3dwindowcontourdataitem.h"
#include "post3dwindowcontourgroupdataitem.h"
#include "private/post3dwindowcontourgroupdataitem_impl.h"

#include <guicore/grid/v4structured3dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>

Post3dWindowContourDataItem::Post3dWindowContourDataItem(const QString& label, Post3dWindowDataItem* p) :
	Post3dWindowDataItem(label, QIcon(":/libs/guibase/images/iconPaper.svg"), p),
	m_actor {vtkActor::New()}
{
	setupStandardItem(Checked, NotReorderable, Deletable);

	renderer()->AddActor(m_actor);
	actorCollection()->AddItem(m_actor);
}

Post3dWindowContourDataItem::~Post3dWindowContourDataItem()
{
	renderer()->RemoveActor(m_actor);

	m_actor->Delete();
}

Post3dWindowFaceSettingContainer Post3dWindowContourDataItem::setting() const
{
	auto s = m_setting;
	s.enabled.setValue(standardItem()->checkState() == Qt::Checked);

	return s;
}

void Post3dWindowContourDataItem::setSetting(const Post3dWindowFaceSettingContainer& setting)
{
	m_setting = setting;
	m_isCommandExecuting = true;
	standardItem()->setCheckState(Qt::Checked);
	m_isCommandExecuting = false;
	updateActorSetting();
}

void Post3dWindowContourDataItem::update()
{
	updateActorSetting();
}

void Post3dWindowContourDataItem::showPropertyDialog()
{
	groupDataItem()->showPropertyDialog();
}

Post3dWindowContourGroupDataItem* Post3dWindowContourDataItem::groupDataItem() const
{
	return dynamic_cast<Post3dWindowContourGroupDataItem*> (parent());
}

void Post3dWindowContourDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	m_standardItem->setText(elem.attribute("caption", tr("Face")));

	m_setting.load(node);

	updateActorSetting();
}

void Post3dWindowContourDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("caption", m_standardItem->text());
	m_setting.save(writer);
}

void Post3dWindowContourDataItem::innerUpdateZScale(double scale)
{
	m_actor->SetScale(1, 1, scale);
}

void Post3dWindowContourDataItem::updateActorSetting()
{
	m_actor->VisibilityOff();
	m_actorCollection->RemoveAllItems();

	auto cont = groupDataItem()->data();
	if (cont == nullptr) {return;}

	auto grid = dynamic_cast<v4Structured3dGrid*>(cont->gridData()->grid());
	auto face = m_setting.extractFace(grid->vtkConcreteData()->concreteData());

	face->GetPointData()->SetActiveScalars(groupDataItem()->target().c_str());

	auto impl = groupDataItem()->impl.get();

	impl->m_setting.contourSetting.setColorMapSetting(impl->m_setting.colorMapSetting);
	auto face2 = impl->m_setting.contourSetting.buildFilteredData(face);
	face->Delete();

	auto mapper = impl->m_setting.colorMapSetting->buildPointDataMapper(face2);
	face2->Delete();
	m_actor->SetMapper(mapper);
	mapper->Delete();

	m_actor->GetProperty()->SetLighting(impl->m_setting.lighting);
	m_actor->GetProperty()->SetOpacity(impl->m_setting.opacity);
	m_actor->GetProperty()->SetLineWidth(impl->m_setting.contourSetting.contourLineWidth);
	m_actorCollection->AddItem(m_actor);

	updateVisibilityWithoutRendering();
}

void Post3dWindowContourDataItem::informSelection(VTKGraphicsView* v)
{
	groupDataItem()->informSelection(v);
}

void Post3dWindowContourDataItem::informDeselection(VTKGraphicsView* v)
{
	groupDataItem()->informDeselection(v);
}

void Post3dWindowContourDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mouseMoveEvent(event, v);
}

void Post3dWindowContourDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mousePressEvent(event, v);
}

void Post3dWindowContourDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mouseReleaseEvent(event, v);
}

bool Post3dWindowContourDataItem::addToolBarButtons(QToolBar* toolBar)
{
	return groupDataItem()->addToolBarButtons(toolBar);
}

QDialog* Post3dWindowContourDataItem::propertyDialog(QWidget* p)
{
	return groupDataItem()->propertyDialog(p);
}

void Post3dWindowContourDataItem::updateVisibility(bool visible)
{
	GraphicsWindowDataItem::updateVisibility(visible);

	groupDataItem()->updateColorMapVisibility();
}
