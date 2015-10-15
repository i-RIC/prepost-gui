#include "preprocessorgriddataitemfactory.h"

#include "../datamodel/preprocessorstructured2dgriddataitem.h"
#include "../datamodel/preprocessornormal15dgridwithcrosssectiondataitem.h"
#include "../datamodel/preprocessorunstructured2dgriddataitem.h"

#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/grid/structured15dgridwithcrosssection.h>
#include <guicore/pre/grid/unstructured2dgrid.h>

PreProcessorGridDataItem* PreProcessorGridDataItemFactory::factory(Grid* grid, PreProcessorDataItem* parent)
{
	if (dynamic_cast<Structured2DGrid*> (grid) != 0) {
		return new PreProcessorStructured2dGridDataItem(parent);
	} else if (dynamic_cast<Structured15DGridWithCrossSection*> (grid) != 0) {
		return new PreProcessorNormal15DGridWithCrossSectionDataItem(parent);
	} else if (dynamic_cast<Unstructured2DGrid*> (grid) != 0) {
		return new PreProcessorUnstructured2dGridDataItem(parent);
	}
	return 0;
}

PreProcessorGridDataItemFactory::PreProcessorGridDataItemFactory()
{}
