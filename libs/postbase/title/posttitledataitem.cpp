#include "posttitledataitem.h"
#include "posttitleeditdialog.h"

#include <guibase/colortool.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>

#include <QDomElement>
#include <QIcon>
#include <QStandardItem>
#include <QUndoCommand>

#include <vtkActor2DCollection.h>
#include <vtkRenderer.h>
#include <vtkTextProperty.h>

PostTitleDataItem::PostTitleDataItem(GraphicsWindowDataItem* parent)
	: GraphicsWindowDataItem(tr("Title"), QIcon(":/libs/guibase/images/iconPaper.png"),parent)
{
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();
	m_isDeletable = false;

	setupActors();
}

PostTitleDataItem::~PostTitleDataItem()
{

}

void PostTitleDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	m_setting.title = elem.attribute("title");
	m_setting.fontSize = elem.attribute("fontSize").toInt();
	m_setting.color = loadColorAttribute("color", elem, Qt::black);
	updateActorSettings();
}

void PostTitleDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("title", m_setting.title);
	writer.writeAttribute("fontSize", QString::number(m_setting.fontSize));
	writeColorAttribute("color", m_setting.color, writer);
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

class PostCommonTitleEditCommand : public QUndoCommand
{
public:
	PostCommonTitleEditCommand(bool enable, const PostTitleSetting& setting, PostTitleDataItem* item)
		: QUndoCommand(PostTitleDataItem::tr("Edit title setting")) {
		m_newEnabled = enable;
		m_newSetting = setting;

		m_oldEnabled = item->isEnabled();
		m_oldSetting = item->m_setting;

		m_dataItem = item;
	}
	void redo() {
		m_dataItem->setIsCommandExecuting(true);
		m_dataItem->setEnabled(m_newEnabled);
		m_dataItem->m_setting = m_newSetting;
		m_dataItem->updateActorSettings();
		m_dataItem->setIsCommandExecuting(false);
	}
	void undo() {
		m_dataItem->setIsCommandExecuting(true);
		m_dataItem->setEnabled(m_oldEnabled);
		m_dataItem->m_setting = m_oldSetting;
		m_dataItem->updateActorSettings();
		m_dataItem->setIsCommandExecuting(false);
	}
private:
	bool m_newEnabled;
	PostTitleSetting m_newSetting;

	bool m_oldEnabled;
	PostTitleSetting m_oldSetting;

	PostTitleDataItem* m_dataItem;
};

void PostTitleDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	PostTitleEditDialog* dialog = dynamic_cast<PostTitleEditDialog*>(propDialog);
	iRICUndoStack::instance().push(new PostCommonTitleEditCommand(dialog->isEnabled(), dialog->setting(), this));
}

void PostTitleDataItem::updateActorSettings()
{
	actor2DCollection()->RemoveAllItems();
	// To avoid VTK warning
	if (m_setting.title.isEmpty()) {
		m_titleActor->VisibilityOff();
		updateVisibility();
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
	updateVisibility();
}

