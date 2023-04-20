#include "post3dwindowcontourdataitem.h"
#include "post3dwindowcontourgroupdataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>

#include <QDomElement>
#include <QDomNode>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkActor.h>
#include <vtkRenderer.h>

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
	updateActorSettings();
}

void Post3dWindowContourDataItem::update()
{
	updateActorSettings();
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

	updateActorSettings();
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

void Post3dWindowContourDataItem::updateActorSettings()
{
	auto data = groupDataItem()->data();
	if (data == nullptr) {return;}
	auto grid = vtkStructuredGrid::SafeDownCast(data->data()->data());
	auto face = m_setting.extractFace(grid);

	face->GetPointData()->SetActiveScalars(groupDataItem()->target().c_str());
	auto mapper = groupDataItem()->m_colorMapSetting.buildPointDataMapper(face);
	m_actor->SetMapper(mapper);
	mapper->Delete();

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
