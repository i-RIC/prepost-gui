#ifndef GEOREFERENCEVIEW_IMAGEINFO_SELECTIONHELPER_H
#define GEOREFERENCEVIEW_IMAGEINFO_SELECTIONHELPER_H

#include "../georeferenceview.h"
#include "../private/georeferenceview_imageinfo.h"
#include "georeferenceviewselectionhelpert.h"

#include <unordered_set>
#include <vector>

class GcpTableRow;

class GeoreferenceView::ImageInfo::SelectionHelper : public GeoreferenceViewSelectionHelperT<GeoreferenceView::ImageInfo>
{
public:
	SelectionHelper(GeoreferenceView::ImageInfo* ctrl);
	SelectionHelper(SelectionHelper&& selectionHelper); // noexcept;

	virtual ~SelectionHelper();

	std::vector<QPointF>::size_type selectedPointsSize();

	void deleteSelectedPoints();
	virtual void selectPoints(const std::unordered_set<std::vector<GcpTableRow>::size_type>& indices);

	SelectionHelper* SelectionHelper::voidSelectionHelper();
	SelectionHelper* SelectionHelper::initSelectionHelper();
	SelectionHelper* SelectionHelper::waitingSelectionHelper();
	void updateSelectedPoints();

protected:
	GeoreferenceView::ImageInfo* info();

	void updateBoundingRect(QRectF* rect);

	std::vector<std::vector<GcpTableRow>::size_type> findPointsInPolygon(const QPolygonF& polygon);

	static std::vector<GcpTableRow>::size_type nearestPoint(std::vector<GcpTableRow>* gcpTable, const QPoint& point, GeoreferenceView* view);
	static void setRectPolygon(QPolygonF* polygon, const QPoint& p1, const QPoint& p2, GeoreferenceView* view);
	static bool isPointInRect(const QRectF& rect, const QPointF point, double xMargin, double yMargin, bool hasMultiplePoints, GeoreferenceView* view);

	const static int RADIUS;

	GeoreferenceView::ImageInfo* m_info;

	QPoint m_pressPoint;
	std::unordered_set<std::vector<GcpTableRow>::size_type> m_selectedIndex;
	std::vector<QPointF> m_selectedPoints;
};

#endif // GEOREFERENCEVIEW_IMAGEINFO_SELECTIONHELPER_H
