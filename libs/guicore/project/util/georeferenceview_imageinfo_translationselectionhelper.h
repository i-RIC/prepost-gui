#ifndef GEOREFERENCEVIEW_IMAGEINFO_TRANSLATIONSELECTIONHELPER_H
#define GEOREFERENCEVIEW_IMAGEINFO_TRANSLATIONSELECTIONHELPER_H

#include "georeferenceview_imageinfo_selectionhelper.h"

#include <QPoint>

class GeoreferenceView::ImageInfo::TranslationSelectionHelper : public SelectionHelper
{
public:
	TranslationSelectionHelper(SelectionHelper&& selectionHelper);

	SelectionHelper* mouseMoveEvent(QMouseEvent* event, GeoreferenceView* view) override;
	SelectionHelper* mouseReleaseEvent(QMouseEvent* event, GeoreferenceView* view) override;

protected:
	void doFinishSelection(GeoreferenceView* view) override;

private:
	std::vector<QPointF> m_translatingPoints;
	QRectF m_boundingRect;
};

#endif // GEOREFERENCEVIEW_IMAGEINFO_TRANSLATIONSELECTIONHELPER_H
