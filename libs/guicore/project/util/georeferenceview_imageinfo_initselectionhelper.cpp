#include "georeferenceview_imageinfo_initselectionhelper.h"
#include "georeferenceview_imageinfo_waitingselectionhelper.h"
#include "../private/georeferenceview_imageinfo.h"
#include "../georeferenceview.h"

// dummy selectionHelper to delay loading of WaitingSelectionHelper
GeoreferenceView::ImageInfo::InitSelectionHelper::InitSelectionHelper(SelectionHelper&& selectionHelper) :
	GeoreferenceView::ImageInfo::SelectionHelper {std::move(selectionHelper)}
{}

GeoreferenceView::ImageInfo::SelectionHelper* GeoreferenceView::ImageInfo::InitSelectionHelper::mouseMoveEvent(QMouseEvent*, GeoreferenceView*)
{
	return new WaitingSelectionHelper {std::move(*this)};
}
