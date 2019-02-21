#include "georeferenceview_imageinfo_waitingselectionhelper.h"
#include "georeferenceview_imageinfo_destinationoperationwaitingselectionhelper.h"
#include "georeferenceview_imageinfo_pointsinrectangleselectionhelper.h"
#include "georeferenceview_imageinfo_translationselectionhelper.h"
#include "gcptablegeoreferenceviewhelper.h"
#include "pointsgeoreferenceviewhelper.h"
#include "rectgeoreferenceviewhelper.h"
#include "../private/georeferenceview_imageinfo.h"
#include "../addiblegcptablemodel.h"
#include "../georeferenceview.h"
#include "../gcptablerow.h"

#include <QMouseEvent>

GeoreferenceView::ImageInfo::WaitingSelectionHelper::WaitingSelectionHelper(SelectionHelper&& selectionHelper) :
	GeoreferenceView::ImageInfo::SelectionHelper {std::move(selectionHelper)},
	m_isMousePressed {false},
	m_boundingRect {}
{
	viewHelpers().push_back(std::unique_ptr<GeoreferenceViewHelper> {new GcpTableGeoreferenceViewHelper {info()->gcpTable(), info()->modelView(), QPen {Qt::magenta}, QBrush {Qt::magenta, Qt::SolidPattern}}});
	viewHelpers().push_back(std::unique_ptr<GeoreferenceViewHelper> {(new PointsGeoreferenceViewHelper {&m_selectedPoints, info()->modelView(), QPen {Qt::black}, QBrush {Qt::magenta, Qt::SolidPattern}})->setRadius(RADIUS + 1)});
	viewHelpers().push_back(std::unique_ptr<GeoreferenceViewHelper> {new RectGeoreferenceViewHelper {&m_boundingRect, info()->modelView(), QPen {Qt::DotLine}}});

	updateBoundingRect(&m_boundingRect);

	info()->updateTableSelection(m_selectedIndex);

	emit info()->selectedIndexChanged(m_selectedIndex);
}

GeoreferenceView::ImageInfo::SelectionHelper* GeoreferenceView::ImageInfo::WaitingSelectionHelper::keyPressEvent(QKeyEvent* event, GeoreferenceView* view)
{
	if (event->key() == Qt::Key_Escape) {
		m_selectedIndex.clear();
		updateSelectedPoints();
		updateBoundingRect(&m_boundingRect);
		view->update();
	}
	return this;
}

GeoreferenceView::ImageInfo::SelectionHelper* GeoreferenceView::ImageInfo::WaitingSelectionHelper::mouseMoveEvent(QMouseEvent*, GeoreferenceView*)
{
	if (m_isMousePressed) {
		return new PointsInRectangleSelectionHelper {std::move(*this)};
	}
	return this;
}

GeoreferenceView::ImageInfo::SelectionHelper* GeoreferenceView::ImageInfo::WaitingSelectionHelper::mousePressEvent(QMouseEvent* event, GeoreferenceView* view)
{
	if (event->button() == Qt::LeftButton) {
		m_pressPoint = event->pos();
		m_isMousePressed = true;

		bool isPressPointInRect = isPointInRect(m_boundingRect, m_pressPoint, RADIUS, RADIUS, m_selectedIndex.size() > 1, view);

		if (isPressPointInRect) {
			return new TranslationSelectionHelper {std::move(*this)};
		}

		auto gcpTable = info()->gcpTable();

		if (gcpTable->size() == 0) {
			return this;
		}

		if (m_selectedIndex.empty()) {
			auto nearestIndex = nearestPoint(gcpTable, m_pressPoint, view);

			const auto& row = gcpTable->at(nearestIndex);

			auto p = view->conv(QPointF {row.sourceX, row.sourceY});
			if (std::abs(p.x() - m_pressPoint.x()) < RADIUS && std::abs(p.y() - m_pressPoint.y()) < RADIUS) {
				m_selectedIndex.insert(nearestIndex);
				updateSelectedPoints();
				updateBoundingRect(&m_boundingRect);
				return new TranslationSelectionHelper {std::move(*this)};
			}
		}
	}
	return this;
}

GeoreferenceView::ImageInfo::SelectionHelper* GeoreferenceView::ImageInfo::WaitingSelectionHelper::mouseReleaseEvent(QMouseEvent* event, GeoreferenceView* view)
{
	if (event->button() == Qt::LeftButton) {
		auto pointToAdd = view->rconv(m_pressPoint);

		GcpTableRow row {};
		row.sourceX = pointToAdd.rx();
		row.sourceY = pointToAdd.ry();

		info()->gcpTableModel()->addRow(row);

		m_selectedIndex.clear();
		updateSelectedPoints();
		updateBoundingRect(&m_boundingRect);

		info()->updateTableSelection(m_selectedIndex);

		m_isMousePressed = false;

		view->update();
		emit info()->gcpDefined();
		return new DestinationOperationWaitingSelectionHelper {std::move(*this)};
	}

	return this;
}

void GeoreferenceView::ImageInfo::WaitingSelectionHelper::selectPoints(const std::unordered_set<std::vector<GcpTableRow>::size_type>& indices)
{
	auto view = info()->modelView();

	m_selectedIndex.clear();
	auto size = info()->gcpTable()->size();
	for (auto index : indices) {
		if (index < size) {
			m_selectedIndex.insert(index);
		}
	}

	updateSelectedPoints();
	updateBoundingRect(&m_boundingRect);
	view->update();
}
