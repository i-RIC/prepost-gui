#include "imagesettingcontainer_movecommand.h"

#include <guicore/datamodel/vtkgraphicsview.h>
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
		m_newSetting.verticalMargin += dy / static_cast<double> (v->height());
	} else if (pos == Position::Bottom || pos == Position::BottomLeft || pos == Position::BottomRight) {
		m_newSetting.verticalMargin -= dy / static_cast<double> (v->height());
	}
	if (pos == Position::Left || pos == Position::TopLeft || pos == Position::BottomLeft) {
		m_newSetting.horizontalMargin += dx / static_cast<double> (v->width());
	} else if (pos == Position::Right || pos == Position::TopRight || pos == Position::BottomRight) {
		m_newSetting.horizontalMargin -= dx / static_cast<double> (v->width());
	}
	if (m_newSetting.negativePositionForbidden()) {
		if (m_newSetting.verticalMargin < 0) {m_newSetting.verticalMargin = 0;}
		if (m_newSetting.horizontalMargin < 0) {m_newSetting.horizontalMargin = 0;}
	}

	m_newSetting.optimizePosition(v);
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
