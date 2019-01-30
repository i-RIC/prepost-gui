#include "georeferenceview_imageinfo_selectionhelper.h"
#include "georeferenceview_imageinfo_initselectionhelper.h"
#include "georeferenceview_imageinfo_voidselectionhelper.h"
#include "georeferenceview_imageinfo_waitingselectionhelper.h"
#include "../private/georeferenceview_imageinfo.h"
#include "../addiblegcptablemodel.h"
#include "../georeferenceview.h"
#include "../gcptablerow.h"
#include "georeferenceviewselectionhelper.h"

#include <QMouseEvent>

#include <functional>
#include <algorithm>

namespace {
std::vector<std::vector<GcpTableRow>::size_type> findPointsInPolygon(const std::vector<GcpTableRow>& gcpTable, const QPolygonF& polygon)
{
	std::vector<std::vector<GcpTableRow>::size_type> indices;

	for (auto i = 0; i < gcpTable.size(); ++i) {
		const auto& row = gcpTable.at(i);
		if (polygon.containsPoint(QPointF {row.sourceX, row.sourceY}, Qt::OddEvenFill)) {
			indices.push_back(i);
		}
	}
	return std::move(indices);
}

QPointF qPointF(const std::vector<GcpTableRow>& gcpTable, const std::vector<std::vector<GcpTableRow>::size_type>&, std::vector<GcpTableRow>::size_type index)
{
	const auto& row = gcpTable.at(index);
	return QPointF {row.sourceX, row.sourceY};
}

QRectF boundingRect(const std::vector<QPointF>& points)
{
	if (points.empty()) {
		return QRectF {};
	}

	auto minmaxX = std::minmax_element(points.begin(), points.end(), [](const QPointF& a, const QPointF& b) {return a.x() < b.x();});
	auto minmaxY = std::minmax_element(points.begin(), points.end(), [](const QPointF& a, const QPointF& b) {return a.y() < b.y();});

	return QRectF {minmaxX.first->x(), minmaxY.first->y(), minmaxX.second->x() - minmaxX.first->x(), minmaxY.second->y() - minmaxY.first->y()};
}

template <typename POINT, typename POINTS>
std::vector<typename POINTS::size_type> findPointsInPolygon(std::function<POINT(typename POINTS::size_type i)>psAt, typename POINTS::size_type psSize, const QPolygonF& polygon)
{
	// Brute force

	std::vector<std::vector<double>::size_type> ret;

	for (auto j = 0; j < psSize; ++j) {
		auto p = psAt(j);

		std::vector<QPointF>::size_type count = 0;
		double previousEval;
		bool isInside = true;
		bool first = true;
		for (auto i = 0; i < polygon.size(); ++i) {
			auto p1 = polygon.at(i);
			auto p2 = polygon.at((i + 1) % polygon.size());
			auto dx = p2.x() - p1.x();
			auto dy = p2.y() - p1.y();

			if (dx != 0 || dy != 0) {++count;}

			double eval = dx * (p.y() - p1.y()) - dy * (p.x() - p1.x());
			if (! first) {
				if (previousEval * eval <= 0) {
					isInside = false;
					break;
				}
			} else {
				first = false;
			}
			previousEval = eval;
		}

		if (count < 3) {isInside = false;} // polygon is not valid (no inner region).

		if (isInside) {ret.push_back(j);}
	}

	return ret;
}

template <typename VIEW>
void setRectPolygon(QPolygonF* polygon, const QPoint& p1, const QPoint& p2, VIEW* view)
{
	QPointF pf1 = view->rconv(p1);
	QPointF pf2 = view->rconv(p2);
	QPointF pf3 = view->rconv(QPoint(p1.x(), p2.y()));
	QPointF pf4 = view->rconv(QPoint(p2.x(), p1.y()));

	polygon->clear();
	polygon->append(QPointF {pf1.x(), pf1.y()});
	polygon->append(QPointF {pf3.x(), pf3.y()});
	polygon->append(QPointF {pf2.x(), pf2.y()});
	polygon->append(QPointF {pf4.x(), pf4.y()});
}

// Returns INDEX of the point in the `points' which is closed to the `point'.
// INDEX is an identifier of the point (typically, index or pointer).
// findPointsInPolygon should return all INDEXs of the points (in the `points') within the spedified polygon.
// qPointF should return QPointF whose x and y are those of INDEX-th element of the specified points.
// POINTS should have the member function empty(). It may also have x() and y(), which may be used in findPointsInPolygon and qPointF.
template <typename INDEX, typename POINTS, typename VIEW>
INDEX nearestPoint(std::function<std::vector<INDEX>(const POINTS&, const QPolygonF&)> findPointsInPolygon, std::function<QPointF(const POINTS&, const std::vector<INDEX>&, typename std::vector<INDEX>::size_type)> qPointF, const std::string message, const POINTS& points, const QPoint& point, VIEW* view)
{
	std::function<std::vector<QPointF>(const POINTS&, const std::vector<INDEX>&, typename std::vector<INDEX>::size_type)> qPointFs = [&](const POINTS& points, const std::vector<INDEX>& indices, typename std::vector<INDEX>::size_type index) {return std::vector<QPointF>{1, qPointF(points, indices, index)};};
	return ::nearestPoint<INDEX, POINTS, VIEW>(findPointsInPolygon, qPointFs, message, points, point, view);
}

// Returns INDEX of the point in the `points' which is closed to the `point'.
// The same as above, with exception ofqPointFs, which returns the vector of QPointF.
template <typename INDEX, typename POINTS, typename VIEW>
INDEX nearestPoint(std::function<std::vector<INDEX>(const POINTS&, const QPolygonF&)> findPointsInPolygon, std::function<std::vector<QPointF>(const POINTS&, const std::vector<INDEX>&, typename std::vector<INDEX>::size_type)> qPointFs, const std::string message, const POINTS& points, const QPoint& point, VIEW* view)
{
	if (points.empty()) {
		// throw common::exception::RuntimeException(message);
	}

	QPolygonF polygon;
	int d = 2;
	bool found = false;
	while (true) {
		::setRectPolygon<VIEW>(&polygon, point - QPoint {d, d}, point + QPoint {d, d}, view);
		auto pointIndices = findPointsInPolygon(points, polygon);

		if (pointIndices.empty()) {
			d *=2;
			continue;
		} else if (! found) {
			d *= 1.4142136;
			++d;
			found = true;
			continue;
		}

		std::vector<INDEX>::size_type minIndex = 0;
		double minSdist = DBL_MAX;
		for (auto index = 0; index < pointIndices.size(); ++index) {
			auto candidatePoints = qPointFs(points, pointIndices, index);
			for (auto& candidatePoint : candidatePoints) {
				QPointF p1 = view->conv(candidatePoint);
				double dx = p1.x() - point.x();
				double dy = p1.y() - point.y();
				double sdist = dx * dx + dy * dy;
				if (minSdist > sdist) {
					minSdist = sdist;
					minIndex = index;
				}
			}
		}

		return pointIndices.at(minIndex);
	}
}

template <typename VIEW>
typename bool isPointInRect(const QRectF& rect, const QPointF point, double xMargin, double yMargin, bool hasMultiplePoints, VIEW* view) {
	// QRectF with height == 0 and width == 0 is invalid.
	// Therefore, bbox for single point is always invalid.
	// if (! rect.isValid()) {
	// 	return false;
	// }

	auto p = view->rconv(point);
	std::vector<QPointF> ps;
	ps.push_back(QPointF {p.x(), p.y()});

	auto dxy = view->rconv(QPointF {point.x() + xMargin, point.y() + yMargin}) - p;
	QPointF dx {dxy.x(), 0.};
	QPointF dy {0., dxy.y()};

	if (! hasMultiplePoints) {
		// Lax judgement.
		QPointF p {rect.topLeft()};
		QPolygonF boundingBox {{p + dx, p + dy, p - dx, p - dy}};
		return boundingBox.containsPoint(ps.at(0), Qt::OddEvenFill);
	}

	bool rectEnoughWidth {rect.width() > 2 * dx.x()};
	bool rectEnoughHeight {rect.height() > 2 * dy.y()};
	if (rectEnoughWidth && rectEnoughHeight) {
		return rect.contains(ps.at(0));
	} else {
		// Lax judgement.
		if (rectEnoughWidth) {
			dx.setX(rect.width() / 2.);
		}
		if (rectEnoughHeight) {
			dy.setY(rect.height() / 2.);
		}
		QPointF p = {rect.center()};
		QPolygonF boundingBox {{p + dx, p + dy, p - dx, p - dy}};
		return boundingBox.containsPoint(ps.at(0), Qt::OddEvenFill);
	}
}
}

const int GeoreferenceView::ImageInfo::SelectionHelper::RADIUS = 3;

GeoreferenceView::ImageInfo::SelectionHelper::SelectionHelper(GeoreferenceView::ImageInfo* info) :
	m_info {info},
	m_pressPoint {},
	m_selectedIndex {},
	m_selectedPoints {}
{}

GeoreferenceView::ImageInfo::SelectionHelper::SelectionHelper(SelectionHelper&& selectionHelper) :
	m_info {selectionHelper.m_info},
	m_pressPoint {selectionHelper.m_pressPoint},
	m_selectedIndex (selectionHelper.m_selectedIndex.size()),
	m_selectedPoints (selectionHelper.m_selectedPoints.size())
{
	m_selectedIndex = std::move(selectionHelper.m_selectedIndex);
	m_selectedPoints = std::move(selectionHelper.m_selectedPoints);
}

GeoreferenceView::ImageInfo::SelectionHelper::~SelectionHelper()
{}

std::vector<QPointF>::size_type GeoreferenceView::ImageInfo::SelectionHelper::selectedPointsSize()
{
	return m_selectedPoints.size();
}

void GeoreferenceView::ImageInfo::SelectionHelper::deleteSelectedPoints()
{
	m_selectedPoints.clear();

	std::vector<std::vector<GcpTableRow>::size_type> sortedIndex {m_selectedIndex.begin(), m_selectedIndex.end()};
	std::sort(sortedIndex.begin(), sortedIndex.end(), std::greater<std::vector<GcpTableRow>::size_type>());

	for (auto i : sortedIndex) {
		info()->gcpTableModel()->removeRows(i, 1);
	}

	auto view = info()->modelView();

	m_selectedIndex.clear();
	updateSelectedPoints();
	// finishSelection(view);
	view->update();
}

void GeoreferenceView::ImageInfo::SelectionHelper::selectPoints(const std::unordered_set<std::vector<GcpTableRow>::size_type>&)
{}

GeoreferenceView::ImageInfo::SelectionHelper* GeoreferenceView::ImageInfo::SelectionHelper::voidSelectionHelper()
{
	return new VoidSelectionHelper {std::move(*this)};
}

GeoreferenceView::ImageInfo::SelectionHelper* GeoreferenceView::ImageInfo::SelectionHelper::initSelectionHelper()
{
	return new InitSelectionHelper {std::move(*this)};
}

GeoreferenceView::ImageInfo::SelectionHelper* GeoreferenceView::ImageInfo::SelectionHelper::waitingSelectionHelper()
{
	return new WaitingSelectionHelper {std::move(*this)};
}

GeoreferenceView::ImageInfo* GeoreferenceView::ImageInfo::SelectionHelper::info()
{
	return m_info;
}

void GeoreferenceView::ImageInfo::SelectionHelper::updateSelectedPoints()
{
	auto gcpTable = info()->gcpTable();

	m_selectedPoints.clear();
	for (auto i : m_selectedIndex) {
		const auto& row = gcpTable->at(i);
		m_selectedPoints.push_back(QPointF {row.sourceX, row.sourceY});
	}
}

void GeoreferenceView::ImageInfo::SelectionHelper::updateBoundingRect(QRectF* rect)
{
	// if (m_selectedPoints.empty()) {return;}
	*rect = ::boundingRect(m_selectedPoints);
}

std::vector<std::vector<GcpTableRow>::size_type> GeoreferenceView::ImageInfo::SelectionHelper::findPointsInPolygon(const QPolygonF& polygon)
{
	auto gcpTable = info()->gcpTable();
	return ::findPointsInPolygon(*gcpTable, polygon);
}

std::vector<GcpTableRow>::size_type GeoreferenceView::ImageInfo::SelectionHelper::nearestPoint(std::vector<GcpTableRow>* gcpTable, const QPoint& point, GeoreferenceView* view)
{
	std::function<QPointF(const std::vector<GcpTableRow>&, const std::vector<std::vector<GcpTableRow>::size_type>&, std::vector<GcpTableRow>::size_type)> f = ::qPointF;
	std::function<std::vector<std::vector<GcpTableRow>::size_type>(const std::vector<GcpTableRow>&, const QPolygonF&)> f1 = ::findPointsInPolygon;
	return ::nearestPoint(f1, f, "Empty GcpTable.", *gcpTable, point, view);

	// std::function<QPointF(const std::vector<GcpTableRow>&, const std::vector<std::vector<GcpTableRow>::size_type>&, std::vector<GcpTableRow>::size_type)> f = ::qPointF;
	// // return ::nearestPoint<std::vector<GcpTableRow>::size_type, std::vector<GcpTableRow>, GeoreferenceView>(::findPointsInPolygon, f, "Empty GcpTable.", *gcpTable, point, view);
	// return ::nearestPoint(::findPointsInPolygon, f, "Empty GcpTable.", *gcpTable, point, view);
}

void GeoreferenceView::ImageInfo::SelectionHelper::setRectPolygon(QPolygonF* polygon, const QPoint& p1, const QPoint& p2, GeoreferenceView* view)
{
	::setRectPolygon<GeoreferenceView>(polygon, p1, p2, view);
}

bool GeoreferenceView::ImageInfo::SelectionHelper::isPointInRect(const QRectF& rect, const QPointF point, double xMargin, double yMargin, bool hasMultiplePoints, GeoreferenceView* view) {
	return ::isPointInRect<GeoreferenceView>(rect, point, xMargin, yMargin, hasMultiplePoints, view);
}
