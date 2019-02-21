#include "georeferenceview_imageinfo_destinationoperationwaitingselectionhelper.h"
#include "gcptablegeoreferenceviewhelper.h"
#include "../private/georeferenceview_imageinfo.h"
#include "../georeferenceview.h"

GeoreferenceView::ImageInfo::DestinationOperationWaitingSelectionHelper::DestinationOperationWaitingSelectionHelper(SelectionHelper&& selectionHelper) :
	GeoreferenceView::ImageInfo::SelectionHelper {std::move(selectionHelper)}
{
	viewHelpers().push_back(std::unique_ptr<GeoreferenceViewHelper> {new GcpTableGeoreferenceViewHelper {info()->gcpTable(), info()->modelView(), QPen {Qt::magenta}, QBrush {Qt::magenta, Qt::SolidPattern}}});
}
