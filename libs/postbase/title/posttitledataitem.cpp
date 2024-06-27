#include "posttitledataitem.h"
#include "private/posttitledataitem_editwidget.h"

#include <guibase/colortool.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>

#include <QIcon>

#include <vtkActor2DCollection.h>
#include <vtkRenderer.h>

PostTitleDataItem::PostTitleDataItem(GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem(tr("Title"), QIcon(":/libs/guibase/images/iconPaper.svg"), parent)
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
	setupActors();
}

PostTitleDataItem::~PostTitleDataItem()
{
	renderer()->RemoveActor2D(m_titleActor);
}

void PostTitleDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	updateActorSetting();
}

void PostTitleDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

void PostTitleDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

void PostTitleDataItem::setupActors()
{
	m_titleActor = vtkSmartPointer<vtkTextActor>::New();
	m_titleActor->VisibilityOff();

	renderer()->AddActor2D(m_titleActor);
}

QDialog* PostTitleDataItem::propertyDialog(QWidget* parent)
{
	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, parent);
	auto widget = new EditWidget(this, dialog);

	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Title Setting"));

	return dialog;
}

void PostTitleDataItem::updateActorSetting()
{
	actor2DCollection()->RemoveAllItems();

	// To avoid VTK warning
	if (m_setting.title.value().isEmpty()) {
		m_titleActor->VisibilityOff();
		return;
	}

	actor2DCollection()->AddItem(m_titleActor);

	m_titleActor->SetInput(iRIC::toStr(m_setting.title).c_str());
	m_titleActor->SetTextScaleModeToNone();
	m_titleActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	m_titleActor->SetPosition(0.5, 0.99);

	vtkTextProperty* prop = m_titleActor->GetTextProperty();
	m_setting.fontSetting.applySetting(prop);
	prop->SetJustificationToCentered();
	prop->SetVerticalJustificationToTop();

	updateVisibilityWithoutRendering();
}
