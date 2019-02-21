#include "georeferenceviewselectionhelper.h"
#include "../georeferenceview.h"
#include "georeferenceviewhelper.h"

GeoreferenceViewSelectionHelper::GeoreferenceViewSelectionHelper() :
	SelectionHelperT<GeoreferenceView, GeoreferenceViewHelper> {}
{}

GeoreferenceViewSelectionHelper::~GeoreferenceViewSelectionHelper()
{}
