#include "imagesettingcontainer_movecommand.h"

#include <misc/qundocommandhelper.h>

#include <QPoint>

ImageSettingContainer::MoveCommand::MoveCommand(bool keyDown, const QPoint& from, const QPoint& to, ImageSettingContainer* setting, VTKGraphicsView* v) :
	UpdateCommand {setting, v},
	m_keyDown {keyDown}
{
	int dx = to.x() - from.x();
	int dy = to.y() - from.y();

	auto pos = m_setting->position.value();

	if (pos == Position::Top || pos == Position::TopLeft || pos == Position::TopRight) {
		m_newSetting.verticalMargin += dy;
	} else if (pos == Position::Bottom || pos == Position::BottomLeft || pos == Position::BottomRight) {
		m_newSetting.verticalMargin -= dy;
	}
	if (pos == Position::Left || pos == Position::TopLeft || pos == Position::BottomLeft) {
		m_newSetting.horizontalMargin += dx;
	} else if (pos == Position::Right || pos == Position::TopRight || pos == Position::BottomRight) {
		m_newSetting.horizontalMargin -= dx;
	}
}

int ImageSettingContainer::MoveCommand::id() const
{
	return iRIC::generateCommandId("ImagesettingContainerMove");
}

bool ImageSettingContainer::MoveCommand::mergeWith(const QUndoCommand* other)
{
	const auto comm = dynamic_cast<const MoveCommand*> (other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_setting != m_setting) {return false;}

	m_newSetting = comm->m_newSetting;
	return true;
}
