#include "imagesettingcontainer_controller.h"
#include "../private/imagesettingcontainer_movecommand.h"
#include "../private/imagesettingcontainer_resizecommand.h"

#include <guicore/datamodel/graphicswindowdataitem.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <misc/mathsupport.h>
#include <cmath>

ImageSettingContainer::Controller::Controller(ImageSettingContainer* setting) :
	m_mouseEventMode {MouseEventMode::Normal},
	m_previousPosition {},
	m_item {nullptr},
	m_selected {false},
	m_setting {setting}
{}

ImageSettingContainer::Controller::MouseEventMode ImageSettingContainer::Controller::mouseEventMode() const
{
	return m_mouseEventMode;
}

bool ImageSettingContainer::Controller::selected() const
{
	return m_selected;
}

GraphicsWindowDataItem* ImageSettingContainer::Controller::item() const
{
	return m_item;
}

void ImageSettingContainer::Controller::setItem(GraphicsWindowDataItem* item)
{
	m_item = item;
}

ImageSettingContainer::Position ImageSettingContainer::Controller::resizePosition() const
{
	return m_resizePosition;
}

void ImageSettingContainer::Controller::handleMouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v, bool noCursorUpdate)
{
	auto event2 = v->createReverseScaledEvent(*event);

	switch (m_mouseEventMode) {
	case MouseEventMode::Move:
		m_item->pushRenderCommand(new MoveCommand(false, m_previousPosition, event2.pos(), m_setting, v), m_item);
		break;
	case MouseEventMode::Resize:
		m_item->pushRenderCommand(new ResizeCommand(false, m_previousPosition, event2.pos(), m_setting, v), m_item);
		break;
	default:
		updateMouseEventMode(event, v);
		if (! noCursorUpdate) {
			updateMouseCursor(v);
		}
	}
	m_previousPosition = event2.pos();
}

void ImageSettingContainer::Controller::handleMousePressEvent(QMouseEvent* event, VTKGraphicsView* v, bool noCursorUpdate)
{
	auto event2 = v->createReverseScaledEvent(*event);

	switch (m_mouseEventMode) {
	case MouseEventMode::MovePrepare:
		m_mouseEventMode = MouseEventMode::Move;
		m_item->pushRenderCommand(new MoveCommand(true, event2.pos(), event2.pos(), m_setting, v), m_item);
		if (! noCursorUpdate) {
			updateMouseCursor(v);
		}
		break;
	case MouseEventMode::ResizePrepare:
		m_mouseEventMode = MouseEventMode::Resize;
		m_item->pushRenderCommand(new ResizeCommand(true, event2.pos(), event2.pos(), m_setting, v), m_item);
		break;
	default:
		break;
	}
	m_previousPosition = event2.pos();
}

void ImageSettingContainer::Controller::handleMouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v, bool noCursorUpdate)
{
	updateMouseEventMode(event, v);
	if (! noCursorUpdate) {
		updateMouseCursor(v);
	}
}

void ImageSettingContainer::Controller::handleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	m_setting->apply(event->size(), v);
}

void ImageSettingContainer::Controller::handleSelection(VTKGraphicsView* v)
{
	m_selected = true;
	m_setting->apply(v->size(), v);
}

void ImageSettingContainer::Controller::handleDeselection(VTKGraphicsView* v)
{
	m_selected = false;
	m_setting->apply(v->size(), v);
}

void ImageSettingContainer::Controller::updateMouseCursor(VTKGraphicsView* v, const std::vector<ImageSettingContainer::Controller*>& controllers)
{
	std::vector<MouseEventMode> modes;
	modes.push_back(MouseEventMode::Move);
	modes.push_back(MouseEventMode::Resize);
	modes.push_back(MouseEventMode::MovePrepare);
	modes.push_back(MouseEventMode::ResizePrepare);
	modes.push_back(MouseEventMode::Normal);

	for (auto m : modes) {
		for (auto c : controllers) {
			if (c->mouseEventMode() == m) {
				c->updateMouseCursor(v);
				return;
			}
		}
	}
}

void ImageSettingContainer::Controller::updateMouseEventMode(QMouseEvent* event, VTKGraphicsView* v)
{
	m_mouseEventMode = MouseEventMode::Normal;

	if (! m_item->isAncientChecked()) {return;}
	if (! m_item->isChecked()) {return;}

	auto r = m_setting->rect(v->size(), v);

	int radius = iRIC::nearRadius();
	bool nearLeft = std::abs(r.left() - event->x()) < radius;
	bool nearRight = std::abs(r.right() - event->x()) < radius;
	bool nearTop = std::abs(r.top() - event->y()) < radius;
	bool nearBottom = std::abs(r.bottom() - event->y()) < radius;
	bool betweenTopBottom = (event->y() >= r.top() && event->y() <= r.bottom());
	bool betweenLeftRight = (event->x() >= r.left() && event->x() <= r.right());

	if (nearTop && nearLeft) {
		m_mouseEventMode = MouseEventMode::ResizePrepare;
		m_resizePosition = Position::TopLeft;
	} else if (nearTop && nearRight) {
		m_mouseEventMode = MouseEventMode::ResizePrepare;
		m_resizePosition = Position::TopRight;
	} else if (nearBottom && nearLeft) {
		m_mouseEventMode = MouseEventMode::ResizePrepare;
		m_resizePosition = Position::BottomLeft;
	} else if (nearBottom && nearRight) {
		m_mouseEventMode = MouseEventMode::ResizePrepare;
		m_resizePosition = Position::BottomRight;
	} else if (betweenTopBottom && nearLeft) {
		m_mouseEventMode = MouseEventMode::ResizePrepare;
		m_resizePosition = Position::Left;
	} else if (betweenTopBottom && nearRight) {
		m_mouseEventMode = MouseEventMode::ResizePrepare;
		m_resizePosition = Position::Right;
	} else if (betweenLeftRight && nearTop) {
		m_mouseEventMode = MouseEventMode::ResizePrepare;
		m_resizePosition = Position::Top;
	} else if (betweenLeftRight && nearBottom) {
		m_mouseEventMode = MouseEventMode::ResizePrepare;
		m_resizePosition = Position::Bottom;
	} else if (betweenLeftRight && betweenTopBottom) {
		m_mouseEventMode = MouseEventMode::MovePrepare;
	}
}

void ImageSettingContainer::Controller::updateMouseCursor(VTKGraphicsView* v)
{
	switch (m_mouseEventMode) {
	case MouseEventMode::MovePrepare:
		v->setCursor(Qt::OpenHandCursor);
		break;

	case MouseEventMode::Move:
		v->setCursor(Qt::ClosedHandCursor);
		break;

	case MouseEventMode::ResizePrepare:
	case MouseEventMode::Resize:
		switch (m_resizePosition) {
		case Position::TopLeft:
		case Position::BottomRight:
			v->setCursor(Qt::SizeFDiagCursor);
			break;
		case Position::TopRight:
		case Position::BottomLeft:
			v->setCursor(Qt::SizeBDiagCursor);
			break;
		case Position::Left:
		case Position::Right:
			v->setCursor(Qt::SizeHorCursor);
			break;
		case Position::Top:
		case Position::Bottom:
			v->setCursor(Qt::SizeVerCursor);
			break;
		}
		break;

	default:
		v->setCursor(Qt::ArrowCursor);
	}
}
