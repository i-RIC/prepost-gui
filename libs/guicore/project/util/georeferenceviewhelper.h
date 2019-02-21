#ifndef GEOREFERENCEVIEWHELPER_H
#define GEOREFERENCEVIEWHELPER_H

#include "../georeferenceview.h"
#include "viewhelpert.h"

class GeoreferenceViewHelper : public ViewHelperT<GeoreferenceView>
{
public:
	GeoreferenceViewHelper(GeoreferenceView* view);
	GeoreferenceViewHelper(GeoreferenceView* view, const QPen& pen);
	GeoreferenceViewHelper(GeoreferenceView* view, const QPen& pen, const QBrush& brush);
};

#endif // GEOREFERENCEVIEWHELPER_H
