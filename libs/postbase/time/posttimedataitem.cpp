#include "posttimedataitem.h"
#include "posttimeeditdialog.h"

#include <guibase/colortool.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>

#include <QDomElement>
#include <QIcon>
#include <QSettings>
#include <QStandardItem>
#include <QUndoCommand>
#include <QXmlStreamWriter>

#include <vtkActor2DCollection.h>
#include <vtkRenderer.h>
#include <vtkTextProperty.h>

PostTimeDataItem::PostTimeDataItem(GraphicsWindowDataItem* parent)
	: GraphicsWindowDataItem(tr("Time"), QIcon(":/libs/guibase/images/iconPaper.png"),parent)
{
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();
	m_isDeletable = false;

	setupActors();
	update();
}

PostTimeDataItem::~PostTimeDataItem()
{

}

void PostTimeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	m_setting.timeFormat = static_cast<TimeFormat::Format>(elem.attribute("format").toInt());
	m_setting.color = loadColorAttribute("color", elem, Qt::black);
	updateActorSettings();
}

void PostTimeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("format", QString::number(static_cast<int>(m_setting.timeFormat)));
	writeColorAttribute("color", m_setting.color, writer);
}

void PostTimeDataItem::setupActors()
{
	m_timeActor = vtkSmartPointer<vtkTextActor>::New();
	m_timeActor->VisibilityOn();

	renderer()->AddActor2D(m_timeActor);
	actor2DCollection()->AddItem(m_timeActor);
}

QDialog* PostTimeDataItem::propertyDialog(QWidget* parent)
{
	PostTimeEditDialog* dialog = new PostTimeEditDialog(parent);
	dialog->setSetting(m_setting);
	return dialog;
}

class PostCommonTimeEditCommand : public QUndoCommand
{
public:
	PostCommonTimeEditCommand(const PostTimeSetting& setting, PostTimeDataItem* item)
		: QUndoCommand(PostTimeDataItem::tr("Edit time setting")) {
		m_newEnabled = true;
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
	PostTimeSetting m_newSetting;

	bool m_oldEnabled;
	PostTimeSetting m_oldSetting;

	PostTimeDataItem* m_dataItem;
};

void PostTimeDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	PostTimeEditDialog* dialog = dynamic_cast<PostTimeEditDialog*>(propDialog);
	iRICUndoStack::instance().push(new PostCommonTimeEditCommand(dialog->setting(), this));
}

void PostTimeDataItem::updateActorSettings()
{
	update();
	updateVisibility();
}

void PostTimeDataItem::update()
{
	double time = postSolutionInfo()->currentTimeStep();
	QString timeStr = QString("Time: %1").arg(TimeFormat::formattedString(time, m_setting.timeFormat));

	m_timeActor->SetInput(iRIC::toStr(timeStr).c_str());
	m_timeActor->SetTextScaleModeToNone();
	m_timeActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	m_timeActor->SetPosition(0.4, 0.05);

	vtkTextProperty* prop = m_timeActor->GetTextProperty();
	double c[3];
	iRIC::QColorToVTKColor(m_setting.color, c);
	prop->SetColor(c);
	prop->SetFontFamilyToArial();
	prop->SetFontSize(FONTSIZE);
	prop->SetJustificationToLeft();
	prop->SetVerticalJustificationToBottom();
}
