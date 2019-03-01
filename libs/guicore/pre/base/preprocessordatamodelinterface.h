#ifndef PREPROCESSORDATAMODELINTERFACE_H
#define PREPROCESSORDATAMODELINTERFACE_H

#include "../../guicore_global.h"
#include "../../datamodel/graphics2dwindowdatamodel.h"

class MouseBoundingBox;
class PreProcessorWindowInterface;
class PreProcessorGeoDataTopDataItemInterface;
class PreProcessorGridAndGridCreatingConditionDataItemInterface;
class PreProcessorGraphicsViewInterface;
class PreProcessorHydraulicDataTopDataItemInterface;

class GUICOREDLL_EXPORT PreProcessorDataModelInterface : public Graphics2DWindowDataModel
{

public:
	PreProcessorDataModelInterface(PreProcessorWindowInterface* w, ProjectDataItem* parent);
	virtual ~PreProcessorDataModelInterface() {}
	virtual MouseBoundingBox* mouseBoundingBox() = 0;
	virtual PreProcessorGeoDataTopDataItemInterface* geoDataTopDataItem() const = 0;
	virtual PreProcessorGeoDataTopDataItemInterface* geoDataTopDataItem(const std::string& type) const = 0;
	virtual PreProcessorHydraulicDataTopDataItemInterface* hydraulicDataTopDataItem(const std::string& type) const = 0;
	virtual PreProcessorGridAndGridCreatingConditionDataItemInterface* getGridAndGridCreatingConditionDataItem(const std::string& typeName, const std::string& zoneName) const = 0;
	virtual PreProcessorGraphicsViewInterface* graphicsView() const = 0;
	virtual void applyOffset(double x, double y) = 0;
};

#endif // PREPROCESSORDATAMODEL_H
