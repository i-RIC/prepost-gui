#ifndef GEOREFERENCEVIEW_IMAGEINFO_WAITINGSELECTIONHELPER_H
#define GEOREFERENCEVIEW_IMAGEINFO_WAITINGSELECTIONHELPER_H

#include "georeferenceview_imageinfo_selectionhelper.h"

class GeoreferenceView::ImageInfo::WaitingSelectionHelper : public SelectionHelper
{
public:
	WaitingSelectionHelper(SelectionHelper&& selectionHelper);

	SelectionHelper* keyPressEvent(QKeyEvent* event, GeoreferenceView* view) override;
	SelectionHelper* mouseMoveEvent(QMouseEvent* event, GeoreferenceView* view) override;
	SelectionHelper* mousePressEvent(QMouseEvent* event, GeoreferenceView* view) override;
	SelectionHelper* mouseReleaseEvent(QMouseEvent* event, GeoreferenceView* view) override;

	void selectPoints(const std::unordered_set<std::vector<GcpTableRow>::size_type>& indices) override;

private:
	bool m_isMousePressed;
	QRectF m_boundingRect;
};

#endif // GEOREFERENCEVIEW_IMAGEINFO_WAITINGSELECTIONHELPER_H
