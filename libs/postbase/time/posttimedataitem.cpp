#include "posttimedataitem.h"
#include "posttimeeditdialog.h"

#include <guicore/postcontainer/postsolutioninfo.h>
#include <misc/stringtool.h>

#include <QIcon>
#include <QSettings>
#include <QStandardItem>
#include <QUndoCommand>

#include <vtkActor2DCollection.h>
#include <vtkRenderer.h>
#include <vtkTextProperty.h>

PostTimeDataItem::Setting::Setting() :
	CompositeContainer ({&timeFormat, &color}),
	timeFormat {"format", TimeFormat::Format::Second},
	color {"color", Qt::black}
{}

PostTimeDataItem::Setting::Setting(const Setting& s) :
	Setting()
{
	CompositeContainer::copyValue(s);
}

PostTimeDataItem::Setting& PostTimeDataItem::Setting::operator=(const Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}

PostTimeDataItem::PostTimeDataItem(GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem {tr("Time"), QIcon(":/libs/guibase/images/iconPaper.png"), parent}
{
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();
	m_isDeletable = false;

	setupActors();
	updateActorSettings();
}

PostTimeDataItem::~PostTimeDataItem()
{
	renderer()->RemoveActor2D(m_timeActor);
}

void PostTimeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	updateActorSettings();
}

void PostTimeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
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

class PostTimeDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const PostTimeDataItem::Setting& setting, PostTimeDataItem* item) :
		QUndoCommand {PostTimeDataItem::tr("Edit time setting")},
		m_newSetting {setting},
		m_oldSetting {item->m_setting},
		m_oldEnabled {item->isEnabled()},
		m_item {item}
	{}
	void redo() override {
		applySetting(m_newSetting, true);
	}
	void undo() override {
		applySetting(m_oldSetting, m_oldEnabled);
	}

private:
	void applySetting(const PostTimeDataItem::Setting& s, bool enabled)
	{
		m_item->m_setting = s;
		m_item->setEnabled(enabled);
		m_item->updateActorSettings();
	}
	PostTimeDataItem::Setting m_newSetting;

	PostTimeDataItem::Setting m_oldSetting;
	bool m_oldEnabled;

	PostTimeDataItem* m_item;
};

void PostTimeDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	PostTimeEditDialog* dialog = dynamic_cast<PostTimeEditDialog*>(propDialog);
	pushRenderCommand(new SetSettingCommand(dialog->setting(), this), this, true);
}

void PostTimeDataItem::update()
{
	updateActorSettings();
}

void PostTimeDataItem::updateActorSettings()
{
	double time = postSolutionInfo()->currentTimeStep();
	QString timeStr = QString("Time: %1").arg(TimeFormat::formattedString(time, m_setting.timeFormat));

	m_timeActor->SetInput(iRIC::toStr(timeStr).c_str());
	m_timeActor->SetTextScaleModeToNone();
	m_timeActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	m_timeActor->SetPosition(0.4, 0.05);

	vtkTextProperty* prop = m_timeActor->GetTextProperty();
	prop->SetColor(m_setting.color);
	prop->SetFontFamilyToArial();
	prop->SetFontSize(FONTSIZE);
	prop->SetJustificationToLeft();
	prop->SetVerticalJustificationToBottom();
}
