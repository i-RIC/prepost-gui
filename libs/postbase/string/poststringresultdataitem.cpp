#include "poststringresultdataitem.h"
#include "poststringresultsettingeditwidget.h"
#include "public/poststringresultsettingcontainer_imagebuilder.h"

#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/datamodel/graphicswindowdatamodel.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/post/postzonedataitem.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QString>

PostStringResultDataItem::PostStringResultDataItem(GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem(tr("Label"), QIcon(":/libs/guibase/images/iconPaper.svg"), parent)
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
	setupActors();
}

PostStringResultDataItem::~PostStringResultDataItem()
{
	renderer()->RemoveActor2D(m_actor);
	m_actor->Delete();
}

void PostStringResultDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

void PostStringResultDataItem::setupActors()
{
	m_actor = vtkActor2D::New();

	m_setting.imageSetting.setActor(m_actor);
	m_setting.imageSetting.controller()->setItem(this);

	renderer()->AddActor2D(m_actor);
}

void PostStringResultDataItem::update()
{
	updateActorSetting();
}

void PostStringResultDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_setting.imageSetting.controller()->handleMouseMoveEvent(event, v);
}

void PostStringResultDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_setting.imageSetting.controller()->handleMousePressEvent(event, v);
}

void PostStringResultDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_setting.imageSetting.controller()->handleMouseReleaseEvent(event, v);
}

QDialog* PostStringResultDataItem::propertyDialog(QWidget* w)
{
	auto zItem = zoneDataItem();
	auto c = zItem->v4DataContainer();
	if (c == nullptr) {return nullptr;}

	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, w);
	dialog->setWindowTitle("Label Setting");

	auto widget = new PostStringResultSettingEditWidget(dialog);
	widget->setZoneDataContainer(c);
	widget->setSetting(&m_setting);

	connect(dialog, &ModifyCommandDialog::importClicked, widget, &PostStringResultSettingEditWidget::importSetting);
	connect(dialog, &ModifyCommandDialog::exportClicked, widget, &PostStringResultSettingEditWidget::exportSetting);

	dialog->setWidget(widget);
	dialog->showImportButton();
	dialog->showExportButton();
	dialog->resize(700, 600);

	return dialog;
}

void PostStringResultDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	m_setting.imageSetting.controller()->handleResize(event, v);
}

void PostStringResultDataItem::updateActorSetting()
{
	m_setting.imageBuilder->setZoneDataContainer(zoneDataItem()->v4DataContainer());

	auto v = dataModel()->graphicsView();
	m_setting.imageSetting.apply(v);
}

void PostStringResultDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	auto srNode = iRIC::getChildNode(node, "StringResult");
	if (! srNode.isNull()) {
		m_setting.load(srNode);
	}
	update();
}

void PostStringResultDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeStartElement("StringResult");
	m_setting.save(writer);
	writer.writeEndElement(); // StringResult
}
