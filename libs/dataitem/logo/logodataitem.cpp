#include "logosettingeditwidget.h"
#include "logodataitem.h"
#include "private/logodataitem_impl.h"

#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/datamodel/graphicswindowdatamodel.h>

LogoDataItem::LogoDataItem(GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem(tr("Logo"), QIcon(":/libs/guibase/images/iconPaper.svg"), parent),
	impl {new Impl {}}
{
	m_isDeletable = false;

	renderer()->AddActor2D(impl->m_actor);
	impl->m_setting.imageSetting.setActor(impl->m_actor);
	impl->m_setting.imageSetting.controller()->setItem(this);

	m_actor2DCollection->AddItem(impl->m_actor);

	updateActorSetting();
}

LogoDataItem::~LogoDataItem()
{
	renderer()->RemoveActor2D(impl->m_actor);
	delete impl;
}

void LogoDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	impl->m_setting.imageSetting.controller()->handleMouseMoveEvent(event, v);
}

void LogoDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	impl->m_setting.imageSetting.controller()->handleMousePressEvent(event, v);
}

void LogoDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	impl->m_setting.imageSetting.controller()->handleMouseReleaseEvent(event, v);
}

QDialog* LogoDataItem::propertyDialog(QWidget* parent)
{
	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, parent);
	auto widget = new LogoSettingEditWidget(dialog);
	widget->setSetting(&impl->m_setting);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Logo Setting"));
	dialog->resize(100, 100);

	return dialog;
}

void LogoDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

void LogoDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	impl->m_setting.imageSetting.controller()->handleResize(event, v);
}

void LogoDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.load(node);
}

void LogoDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_setting.save(writer);
}

void LogoDataItem::updateActorSetting()
{
	updateVisibilityWithoutRendering();

	auto view = dataModel()->graphicsView();
	impl->m_setting.imageSetting.apply(view);
}
