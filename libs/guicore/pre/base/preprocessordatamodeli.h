#ifndef PREPROCESSORDATAMODELI_H
#define PREPROCESSORDATAMODELI_H

#include "../../guicore_global.h"
#include "../../datamodel/graphics2dwindowdatamodel.h"

class MouseBoundingBox;
class PreProcessorWindowI;
class PreProcessorGeoDataTopDataItemI;
class PreProcessorGridAndGridCreatingConditionDataItemI;
class PreProcessorGridTypeDataItemI;
class PreProcessorGraphicsViewI;
class PreProcessorHydraulicDataTopDataItemI;

class GUICOREDLL_EXPORT PreProcessorDataModelI : public Graphics2DWindowDataModel
{

public:
	PreProcessorDataModelI(PreProcessorWindowI* w, ProjectDataItem* parent);
	virtual ~PreProcessorDataModelI() {}
	virtual MouseBoundingBox* mouseBoundingBox() = 0;
	virtual PreProcessorGridTypeDataItemI* gridTypeDataItem(const std::string& type) const = 0;
	virtual PreProcessorGeoDataTopDataItemI* geoDataTopDataItem() const = 0;
	virtual PreProcessorGeoDataTopDataItemI* geoDataTopDataItem(const std::string& type) const = 0;
	virtual PreProcessorHydraulicDataTopDataItemI* hydraulicDataTopDataItem(const std::string& type) const = 0;
	virtual PreProcessorGridAndGridCreatingConditionDataItemI* getGridAndGridCreatingConditionDataItem(const std::string& typeName, const std::string& zoneName) const = 0;
	virtual PreProcessorGraphicsViewI* graphicsView() const = 0;
	virtual void applyOffset(double x, double y) = 0;
};

#endif // PREPROCESSORDATAMODEL_H
