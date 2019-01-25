#ifndef GEOREFERENCEVIEW_IMAGEINFO_INITSELECTIONHELPER_H
#define GEOREFERENCEVIEW_IMAGEINFO_INITSELECTIONHELPER_H

#include "georeferenceview_imageinfo_selectionhelper.h"

class GeoreferenceView::ImageInfo::InitSelectionHelper : public SelectionHelper
{
public:
	InitSelectionHelper(SelectionHelper&& selectionHelper);

	SelectionHelper* mouseMoveEvent(QMouseEvent* event, GeoreferenceView* view) override;
};

#endif // GEOREFERENCEVIEW_IMAGEINFO_INITSELECTIONHELPER_H
