#include "posttitledataitem.h"
#include "posttitleeditdialog.h"
#include "private/posttitledataitem_setsettingcommand.h"

#include <guibase/colortool.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>

#include <QDomElement>
#include <QIcon>
#include <QStandardItem>
#include <QUndoCommand>
#include <QXmlStreamWriter>

#include <vtkActor2DCollection.h>
#include <vtkRenderer.h>
#include <vtkTextProperty.h>

PostTitleDataItem::PostTitleDataItem(GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem(tr("Title"), QIcon(":/libs/guibase/images/iconPaper.png"), parent)
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
	setupActors();
}

PostTitleDataItem::~PostTitleDataItem()
{}

void PostTitleDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	updateActorSettings();
}

void PostTitleDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

void PostTitleDataItem::setupActors()
{
	m_titleActor = vtkSmartPointer<vtkTextActor>::New();
	m_titleActor->VisibilityOff();

	renderer()->AddActor2D(m_titleActor);
}

QDialog* PostTitleDataItem::propertyDialog(QWidget* parent)
{
	PostTitleEditDialog* dialog = new PostTitleEditDialog(parent);
	dialog->setEnabled(isEnabled());
	dialog->setSetting(m_setting);
	return dialog;
}

void PostTitleDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	PostTitleEditDialog* dialog = dynamic_cast<PostTitleEditDialog*>(propDialog);
	pushRenderCommand(new SetSettingCommand(dialog->isEnabled(), dialog->setting(), this), this, true);
}

void PostTitleDataItem::updateActorSettings()
{
	actor2DCollection()->RemoveAllItems();
	// To avoid VTK warning
	if (m_setting.title.value().isEmpty()) {
		m_titleActor->VisibilityOff();
		updateVisibilityWithoutRendering();
		return;
	}
	actor2DCollection()->AddItem(m_titleActor);
	m_titleActor->SetInput(iRIC::toStr(m_setting.title).c_str());
	m_titleActor->SetTextScaleModeToNone();
	m_titleActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	m_titleActor->SetPosition(0.5, 0.99);

	vtkTextProperty* prop = m_titleActor->GetTextProperty();
	double c[3];
	iRIC::QColorToVTKColor(m_setting.color, c);
	prop->SetColor(c);
	prop->SetFontFamilyToArial();
	prop->SetFontSize(m_setting.fontSize);
	prop->SetJustificationToCentered();
	prop->SetVerticalJustificationToTop();
	updateVisibilityWithoutRendering();
}
