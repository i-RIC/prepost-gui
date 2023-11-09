#include "preprocessorgriddataitemfactory.h"

#include "../datamodel/preprocessorstructured2dgriddataitem.h"
#include "../datamodel/preprocessornormal15dgridwithcrosssectiondataitem.h"
#include "../datamodel/preprocessorunstructured2dgriddataitem.h"

#include <guicore/grid/v4structured15dgridwithcrosssection.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/grid/v4unstructured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>

PreProcessorGridDataItem* PreProcessorGridDataItemFactory::factory(v4InputGrid* grid, PreProcessorDataItem* parent)
{
	auto grid2 = grid->grid();
	if (dynamic_cast<v4Structured2dGrid*> (grid2) != nullptr) {
		return new PreProcessorStructured2dGridDataItem(parent);
	} else if (dynamic_cast<v4Structured15dGridWithCrossSection*> (grid2) != nullptr) {
		return new PreProcessorNormal15DGridWithCrossSectionDataItem(parent);
	} else if (dynamic_cast<v4Unstructured2dGrid*> (grid2) != nullptr) {
		return new PreProcessorUnstructured2dGridDataItem(parent);
	}

	return nullptr;
}

PreProcessorGridDataItemFactory::PreProcessorGridDataItemFactory()
{}
