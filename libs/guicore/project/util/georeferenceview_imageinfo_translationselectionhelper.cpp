#include "georeferenceview_imageinfo_translationselectionhelper.h"
#include "georeferenceview_imageinfo_waitingselectionhelper.h"
#include "gcptablegeoreferenceviewhelper.h"
#include "pointsgeoreferenceviewhelper.h"
#include "rectgeoreferenceviewhelper.h"
#include "../private/georeferenceview_imageinfo.h"
#include "../addiblegcptablemodel.h"
#include "../georeferenceview.h"
#include "../gcptablerow.h"

#include <QMouseEvent>

GeoreferenceView::ImageInfo::TranslationSelectionHelper::TranslationSelectionHelper(SelectionHelper&& selectionHelper) :
	GeoreferenceView::ImageInfo::SelectionHelper {std::move(selectionHelper)},
	m_translatingPoints {},
	m_boundingRect {}
{
	viewHelpers().push_back(std::unique_ptr<GeoreferenceViewHelper> {new GcpTableGeoreferenceViewHelper {info()->gcpTable(), info()->modelView(), QPen {Qt::magenta}, QBrush {Qt::magenta, Qt::SolidPattern}}});
	viewHelpers().push_back(std::unique_ptr<GeoreferenceViewHelper> {(new PointsGeoreferenceViewHelper {&m_selectedPoints, info()->modelView(), QPen {Qt::black}, QBrush {Qt::magenta, Qt::SolidPattern}})->setRadius(RADIUS + 1)});
	viewHelpers().push_back(std::unique_ptr<GeoreferenceViewHelper> {(new PointsGeoreferenceViewHelper {&m_translatingPoints, info()->modelView(), QPen {Qt::black}})->setRadius(RADIUS + 1)});
	viewHelpers().push_back(std::unique_ptr<GeoreferenceViewHelper> {new RectGeoreferenceViewHelper {&m_boundingRect, info()->modelView(), QPen {Qt::DotLine}}});

	m_translatingPoints = m_selectedPoints;
}

GeoreferenceView::ImageInfo::SelectionHelper* GeoreferenceView::ImageInfo::TranslationSelectionHelper::mouseMoveEvent(QMouseEvent* event, GeoreferenceView* view)
{
	const QPoint currentPoint = event->pos();

	m_translatingPoints.clear();

	QPointF pf = view->rconv(currentPoint) - view->rconv(m_pressPoint);
	for (auto selectedPoint : m_selectedPoints) {
		m_translatingPoints.push_back(selectedPoint + pf);
	}

	if (! m_selectedPoints.empty()) {
		updateBoundingRect(&m_boundingRect);
	}

	view->update();
	return this;
}

GeoreferenceView::ImageInfo::SelectionHelper* GeoreferenceView::ImageInfo::TranslationSelectionHelper::mouseReleaseEvent(QMouseEvent*, GeoreferenceView* view)
{
	finishSelection(view);
	return new WaitingSelectionHelper{ std::move(*this) };
}

void GeoreferenceView::ImageInfo::TranslationSelectionHelper::doFinishSelection(GeoreferenceView*)
{
	auto vec = std::vector<std::vector<GcpTableRow>::size_type> {m_selectedIndex.begin(), m_selectedIndex.end()}; // m_selectedIndex's order is fixed within call.
	info()->gcpTableModel()->replaceSourceData(vec, m_translatingPoints);
	updateSelectedPoints();
}
