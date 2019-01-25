#ifndef GEOREFERENCEVIEWSELECTIONHELPER_H
#define GEOREFERENCEVIEWSELECTIONHELPER_H

#include "selectionhelpert.h"

class GeoreferenceView;
class GeoreferenceViewHelper;


class GeoreferenceViewSelectionHelper : public SelectionHelperT<GeoreferenceView, GeoreferenceViewHelper>
{
public:
	GeoreferenceViewSelectionHelper();
	virtual ~GeoreferenceViewSelectionHelper();
};

#endif // GEOREFERENCEVIEWSELECTIONHELPER_H
