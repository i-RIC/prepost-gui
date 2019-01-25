#ifndef GEOREFERENCEVIEWSELECTIONHELPERT_H
#define GEOREFERENCEVIEWSELECTIONHELPERT_H

#include "georeferenceviewselectionhelper.h"

template <typename ITEM>
class GeoreferenceViewSelectionHelperT : public GeoreferenceViewSelectionHelper
{
public:
	GeoreferenceViewSelectionHelperT();
	virtual ~GeoreferenceViewSelectionHelperT();
};

#include "georeferenceviewselectionhelpert_detail.h"

#endif // GEOREFERENCEVIEWSELECTIONHELPERT_H
