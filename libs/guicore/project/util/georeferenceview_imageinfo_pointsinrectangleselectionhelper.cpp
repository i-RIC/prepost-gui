#include "georeferenceview_imageinfo_pointsinrectangleselectionhelper.h"
#include "georeferenceview_imageinfo_waitingselectionhelper.h"
#include "../private/georeferenceview_imageinfo.h"
#include "../georeferenceview.h"
#include "gcptablegeoreferenceviewhelper.h"
#include "pointsgeoreferenceviewhelper.h"
#include "polygongeoreferenceviewhelper.h"

#include <QKeyEvent>
#include <QMouseEvent>

namespace {
const static bool enableRepetitiveEditing = false;
const static int minimumNumSelections = 0;
}

GeoreferenceView::ImageInfo::PointsInRectangleSelectionHelper::PointsInRectangleSelectionHelper(SelectionHelper&& selectionHelper) :
	GeoreferenceView::ImageInfo::SelectionHelper {std::move(selectionHelper)}
{
	viewHelpers().push_back(std::unique_ptr<GeoreferenceViewHelper> {new GcpTableGeoreferenceViewHelper {info()->gcpTable(), info()->modelView(), QPen {Qt::magenta}, QBrush {Qt::magenta, Qt::SolidPattern}}});
	viewHelpers().push_back(std::unique_ptr<GeoreferenceViewHelper> {(new PointsGeoreferenceViewHelper {&m_selectedPoints, info()->modelView(), QPen {Qt::black}, QBrush {Qt::magenta, Qt::SolidPattern}})->setRadius(RADIUS + 1)});
	viewHelpers().push_back(std::unique_ptr<GeoreferenceViewHelper> {new PolygonGeoreferenceViewHelper {&m_selectAreaPolygon, info()->modelView()}});
}

GeoreferenceView::ImageInfo::SelectionHelper* GeoreferenceView::ImageInfo::PointsInRectangleSelectionHelper::keyPressEvent(QKeyEvent* event, GeoreferenceView* view)
{
	if (event->key() == Qt::Key_Return) {
		// if (m_pickedPoints->size() < minimumNumSelections) {return;}

		// Finish polygon selection.
		auto selectedIndex = findPointsInPolygon(m_selectAreaPolygon);
		for (auto i : selectedIndex) {
			auto it = m_selectedIndex.find(i);
			if (it == m_selectedIndex.end()) {
				m_selectedIndex.insert(i);
			} else {
				m_selectedIndex.erase(it);
			}
		}

		updateSelectedPoints();

		// finishSelection(view);
		if (! enableRepetitiveEditing) {
			view->update();
			return new WaitingSelectionHelper {std::move(*this)};
		}
	}
	return this;
}

GeoreferenceView::ImageInfo::SelectionHelper* GeoreferenceView::ImageInfo::PointsInRectangleSelectionHelper::mouseDoubleClickEvent(QMouseEvent*, GeoreferenceView*)
{
	return this;
}

GeoreferenceView::ImageInfo::SelectionHelper* GeoreferenceView::ImageInfo::PointsInRectangleSelectionHelper::mouseMoveEvent(QMouseEvent* event, GeoreferenceView* view)
{
	const QPoint currentPoint = event->pos();

	setRectPolygon(&m_selectAreaPolygon, m_pressPoint, currentPoint, view);

	view->update();
	return this;
}

GeoreferenceView::ImageInfo::SelectionHelper* GeoreferenceView::ImageInfo::PointsInRectangleSelectionHelper::mouseReleaseEvent(QMouseEvent* event, GeoreferenceView* view)
{
	// Finish selecting points by rectangle.

	if (! (event->modifiers() & Qt::ShiftModifier)) {
		m_selectedIndex.clear();
	}

	if (m_selectAreaPolygon.empty()) {
		auto rect = m_pressPoint - event->pos();
		if (std::abs(rect.x()) < RADIUS && std::abs(rect.y()) < RADIUS) {
			setRectPolygon(&m_selectAreaPolygon, m_pressPoint - QPoint(RADIUS, RADIUS), m_pressPoint + QPoint(RADIUS, RADIUS), view);
		}
	}

	// Finish polygon selection.
	auto selectedIndex = findPointsInPolygon(m_selectAreaPolygon);
	for (auto i : selectedIndex) {
		auto it = m_selectedIndex.find(i);
		if (it == m_selectedIndex.end()) {
			m_selectedIndex.insert(i);
		} else {
			m_selectedIndex.erase(it);
		}
	}

	updateSelectedPoints();

	// finishSelection(view);
	if (! enableRepetitiveEditing) {
		view->update();
		return new WaitingSelectionHelper {std::move(*this)};
	}

	return this;
}
