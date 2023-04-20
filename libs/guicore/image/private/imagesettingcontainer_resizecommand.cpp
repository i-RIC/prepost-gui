#include "imagesettingcontainer_resizecommand.h"

#include <guicore/datamodel/vtkgraphicsview.h>
#include <misc/qundocommandhelper.h>

ImageSettingContainer::ResizeCommand::ResizeCommand(bool keyDown, const QPoint& from, const QPoint& to, ImageSettingContainer* setting, VTKGraphicsView* v) :
	UpdateCommand {setting, v},
	m_keyDown {keyDown}
{
	int dx = to.x() - from.x();
	int dy = to.y() - from.y();

	auto resizePos = m_setting->controller()->resizePosition();
	auto pos = m_setting->position.value();

	// vertical
	if (resizePos == Position::Top || resizePos == Position::TopLeft || resizePos == Position::TopRight) {
		m_newSetting.height -= dy;

		if (pos == Position::Top || pos == Position::TopLeft || pos == Position::TopRight) {
			m_newSetting.verticalMargin += dy / static_cast<double> (v->height());
		}
	} else if (resizePos == Position::Bottom || resizePos == Position::BottomLeft || resizePos == Position::BottomRight) {
		m_newSetting.height += dy;

		if (pos == Position::Bottom || pos == Position::BottomLeft || pos == Position::BottomRight) {
			m_newSetting.verticalMargin -= dy / static_cast<double> (v->height());
		}
	}

	// horizontal
	if (resizePos == Position::Left || resizePos == Position::TopLeft || resizePos == Position::BottomLeft) {
		m_newSetting.width -= dx;

		if (pos == Position::Left || pos == Position::TopLeft || pos == Position::BottomLeft) {
			m_newSetting.horizontalMargin += dx / static_cast<double> (v->width());
		}
	} else if (resizePos == Position::Right || resizePos == Position::TopRight || resizePos == Position::BottomRight) {
		m_newSetting.width += dx;

		if (pos == Position::Right || pos == Position::TopRight || pos == Position::BottomRight) {
			m_newSetting.horizontalMargin -= dx / static_cast<double> (v->width());
		}
	}

	m_newSetting.optimizePosition(v);
}

int ImageSettingContainer::ResizeCommand::id() const
{
	return iRIC::generateCommandId("ImagesettingContainerResize");
}

bool ImageSettingContainer::ResizeCommand::mergeWith(const QUndoCommand* other)
{
	const auto comm = dynamic_cast<const ResizeCommand*> (other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_setting != m_setting) {return false;}

	m_newSetting = comm->m_newSetting;
	return true;
}
