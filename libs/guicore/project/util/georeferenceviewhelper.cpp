#include "georeferenceviewhelper.h"
#include "../georeferenceview.h"

GeoreferenceViewHelper::GeoreferenceViewHelper(GeoreferenceView* view) :
	ViewHelperT<GeoreferenceView> {view}
{}

GeoreferenceViewHelper::GeoreferenceViewHelper(GeoreferenceView* view, const QPen& pen) :
	ViewHelperT<GeoreferenceView> {view, pen}
{}

GeoreferenceViewHelper::GeoreferenceViewHelper(GeoreferenceView* view, const QPen& pen, const QBrush& brush) :
	ViewHelperT<GeoreferenceView> {view, pen, brush}
{}
