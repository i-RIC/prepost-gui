#ifndef GEOREFERENCEVIEW_IMAGEINFO_VOIDSELECTIONHELPER_H
#define GEOREFERENCEVIEW_IMAGEINFO_VOIDSELECTIONHELPER_H

#include "georeferenceview_imageinfo_selectionhelper.h"

class GeoreferenceView::ImageInfo::VoidSelectionHelper : public SelectionHelper
{
public:
	VoidSelectionHelper(SelectionHelper&& selectionHelper);
};

#endif // GEOREFERENCEVIEW_IMAGEINFO_VOIDSELECTIONHELPER_H
