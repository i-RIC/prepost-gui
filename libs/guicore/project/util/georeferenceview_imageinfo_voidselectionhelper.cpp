#include "georeferenceview_imageinfo_voidselectionhelper.h"
#include "../private/georeferenceview_imageinfo.h"
#include "../georeferenceview.h"

GeoreferenceView::ImageInfo::VoidSelectionHelper::VoidSelectionHelper(SelectionHelper&& selectionHelper) :
	GeoreferenceView::ImageInfo::SelectionHelper {std::move(selectionHelper)}
{}
