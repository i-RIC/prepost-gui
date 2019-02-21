#ifndef GEOREFERENCEVIEW_IMAGEINFO_POINTSINRECTANGLESELECTIONHELPER_H
#define GEOREFERENCEVIEW_IMAGEINFO_POINTSINRECTANGLESELECTIONHELPER_H

#include "georeferenceview_imageinfo_selectionhelper.h"

#include <QPoint>

class GeoreferenceView::ImageInfo::PointsInRectangleSelectionHelper : public SelectionHelper
{
public:
	PointsInRectangleSelectionHelper(SelectionHelper&& selectionHelper);

	SelectionHelper* keyPressEvent(QKeyEvent* event, GeoreferenceView* view) override;
	SelectionHelper* mouseDoubleClickEvent(QMouseEvent* event, GeoreferenceView* view) override;
	SelectionHelper* mouseMoveEvent(QMouseEvent* event, GeoreferenceView* view) override;
	SelectionHelper* mouseReleaseEvent(QMouseEvent* event, GeoreferenceView* view) override;

private:
	QPolygonF m_selectAreaPolygon;
};

#endif // GEOREFERENCEVIEW_IMAGEINFO_POINTSINRECTANGLESELECTIONHELPER_H
