#include "posttimedataitem.h"
#include "private/posttimedataitem_editwidget.h"

#include <guibase/timeformat/timeformatutil.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <misc/qscreenutil.h>
#include <misc/stringtool.h>

#include <QDateTime>
#include <QDomNode>
#include <QIcon>
#include <QSettings>
#include <QStandardItem>
#include <QTimeZone>
#include <QXmlStreamWriter>

#include <vtkActor2DCollection.h>
#include <vtkRenderer.h>
#include <vtkTextProperty.h>

PostTimeDataItem::Setting::Setting() :
	CompositeContainer ({&useProjectSetting, &timeFormat, &customTimeFormat, &showTimeZone, &fontSetting}),
	useProjectSetting {"useProjectSetting", true},
	timeFormat {"format", TimeFormat::elapsed_SS_sec},
	customTimeFormat {"customFormat", ""},
	showTimeZone {"showTimeZone", false},
	fontSetting {}
{
	fontSetting.fontSize = FONTSIZE;
}

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

void PostTimeDataItem::Setting::load(const QDomNode& node)
{
	for (XmlAttributeContainer* c : containers()) {
		auto timeFormatContainer = dynamic_cast<EnumContainerT<TimeFormat>*> (c);
		if (timeFormatContainer != nullptr) {
			auto elem = node.toElement();
			*timeFormatContainer = TimeFormatUtil::fromString(elem.attribute("timeFormat"));
		} else {
			c->load(node);
		}
	}
}

void PostTimeDataItem::Setting::save(QXmlStreamWriter& writer) const
{
	for (XmlAttributeContainer* c : containers()) {
		auto timeFormatContainer = dynamic_cast<EnumContainerT<TimeFormat>*> (c);
		if (timeFormatContainer != nullptr) {
			writer.writeAttribute("timeFormat", TimeFormatUtil::toString(timeFormatContainer->value()));
		} else {
			c->save(writer);
		}
	}
}

PostTimeDataItem::PostTimeDataItem(GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem {tr("Time"), QIcon(":/libs/guibase/images/iconPaper.svg"), parent}
{
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();
	m_isDeletable = false;

	setupActors();
	updateActorSetting();
}

PostTimeDataItem::~PostTimeDataItem()
{
	renderer()->RemoveActor2D(m_timeActor);
}

void PostTimeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	updateActorSetting();
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

void PostTimeDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* PostTimeDataItem::propertyDialog(QWidget* parent)
{
	auto mainFile = projectData()->mainfile();
	auto zeroDateTime = mainFile->zeroDateTime();
	if (m_setting.useProjectSetting) {
		m_setting.timeFormat = mainFile->timeFormat();
		m_setting.customTimeFormat = mainFile->customTimeFormat();
		m_setting.showTimeZone = mainFile->showTimeZone();
	}

	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, parent);
	auto widget = new EditWidget(this, dialog);
	widget->setActualTimeAvailable(! zeroDateTime.isNull());

	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Time Setting"));
	dialog->resize(QScreenUtil::sizeWithinScreen(500, 650));

	return dialog;
}

void PostTimeDataItem::update()
{
	updateActorSetting();
}

void PostTimeDataItem::doUpdateActorSetting()
{
	auto mainFile = projectData()->mainfile();

	QDateTime zeroDateTime = mainFile->zeroDateTime();
	auto format = mainFile->timeFormat();
	auto customFormat = mainFile->customTimeFormat();
	auto showTimeZone = mainFile->showTimeZone();

	double time = postSolutionInfo()->currentTimeStep();

	if (! m_setting.useProjectSetting) {
		format = m_setting.timeFormat.value();
		customFormat = m_setting.customTimeFormat.value();
		showTimeZone = m_setting.showTimeZone;
	}
	auto formattedStr = TimeFormatUtil::formattedString(zeroDateTime, time, format, customFormat);
	if (showTimeZone) {
		formattedStr += " (" + mainFile->timeZone().displayName(QTimeZone::StandardTime) + ")";
	}

	QString timeStr = QString("Time: %1").arg(formattedStr);

	m_timeActor->SetInput(iRIC::toStr(timeStr).c_str());
	m_timeActor->SetTextScaleModeToNone();
	m_timeActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	m_timeActor->SetPosition(0.4, 0.05);

	vtkTextProperty* prop = m_timeActor->GetTextProperty();
	m_setting.fontSetting.applySetting(prop);
	prop->SetJustificationToLeft();
	prop->SetVerticalJustificationToBottom();
}
