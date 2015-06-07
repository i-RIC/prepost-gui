#ifndef PREPROCESSORDATAMODELINTERFACE_H
#define PREPROCESSORDATAMODELINTERFACE_H

#include "../../guicore_global.h"
#include "../../datamodel/graphics2dwindowdatamodel.h"

class MouseBoundingBox;
class PreProcessorWindowInterface;
class PreProcessorRawDataTopDataItemInterface;
class PreProcessorGridAndGridCreatingConditionDataItemInterface;
class PreProcessorGraphicsViewInterface;

class GUICOREDLL_EXPORT PreProcessorDataModelInterface : public Graphics2DWindowDataModel
{

public:
	PreProcessorDataModelInterface(PreProcessorWindowInterface* w, ProjectDataItem* parent);
	virtual MouseBoundingBox* mouseBoundingBox() = 0;
	virtual PreProcessorRawDataTopDataItemInterface* rawDataTopDataItem(const QString& type) const = 0;
	virtual PreProcessorGridAndGridCreatingConditionDataItemInterface* getGridAndGridCreatingConditionDataItem(const QString& typeName, const QString& zoneName) const = 0;
	virtual PreProcessorGraphicsViewInterface* graphicsView() const = 0;
	virtual void applyOffset(double x, double y) = 0;
};

#endif // PREPROCESSORDATAMODEL_H
