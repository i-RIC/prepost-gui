#include "post3dwindowcellcontourdataitem.h"
#include "post3dwindowcellcontourgroupdataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>

#include <QDomElement>
#include <QDomNode>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkActor.h>
#include <vtkRenderer.h>

Post3dWindowCellContourDataItem::Post3dWindowCellContourDataItem(const QString& label, Post3dWindowDataItem* p) :
	Post3dWindowDataItem(label, QIcon(":/libs/guibase/images/iconPaper.svg"), p),
	m_actor {vtkActor::New()}
{
	setupStandardItem(Checked, NotReorderable, Deletable);

	renderer()->AddActor(m_actor);
	actorCollection()->AddItem(m_actor);
}

Post3dWindowCellContourDataItem::~Post3dWindowCellContourDataItem()
{
	renderer()->RemoveActor(m_actor);

	m_actor->Delete();
}

Post3dWindowCellRangeSettingContainer Post3dWindowCellContourDataItem::setting() const
{
	auto s = m_setting;
	s.enabled.setValue(standardItem()->checkState() == Qt::Checked);

	return s;
}

void Post3dWindowCellContourDataItem::setSetting(const Post3dWindowCellRangeSettingContainer& setting)
{
	m_setting = setting;
	m_isCommandExecuting = true;
	standardItem()->setCheckState(Qt::Checked);
	m_isCommandExecuting = false;
	updateActorSettings();
}

void Post3dWindowCellContourDataItem::update()
{
	updateActorSettings();
}

Post3dWindowCellContourGroupDataItem* Post3dWindowCellContourDataItem::groupDataItem() const
{
	return dynamic_cast<Post3dWindowCellContourGroupDataItem*> (parent());
}

void Post3dWindowCellContourDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	m_standardItem->setText(elem.attribute("caption", tr("Range")));

	m_setting.load(node);

	updateActorSettings();
}

void Post3dWindowCellContourDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("caption", m_standardItem->text());
	m_setting.save(writer);
}

void Post3dWindowCellContourDataItem::innerUpdateZScale(double scale)
{
	m_actor->SetScale(1, 1, scale);
}

void Post3dWindowCellContourDataItem::updateActorSettings()
{
	auto data = groupDataItem()->data();
	if (data == nullptr) {return;}
	auto grid = vtkStructuredGrid::SafeDownCast(data->data()->data());
	auto extracted = m_setting.extractRegion(grid);

	extracted->GetCellData()->SetActiveScalars(groupDataItem()->target().c_str());
	auto mapper = groupDataItem()->m_colorMapSetting.buildCellDataMapper(extracted, false);
	m_actor->SetMapper(mapper);
	mapper->Delete();

	updateVisibilityWithoutRendering();
}

void Post3dWindowCellContourDataItem::informSelection(VTKGraphicsView* v)
{
	groupDataItem()->informSelection(v);
}

void Post3dWindowCellContourDataItem::informDeselection(VTKGraphicsView* v)
{
	groupDataItem()->informDeselection(v);
}

void Post3dWindowCellContourDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mouseMoveEvent(event, v);
}

void Post3dWindowCellContourDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mousePressEvent(event, v);
}

void Post3dWindowCellContourDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mouseReleaseEvent(event, v);
}
