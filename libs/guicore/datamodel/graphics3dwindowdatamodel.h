#ifndef GRAPHICS3DWINDOWDATAMODEL_H
#define GRAPHICS3DWINDOWDATAMODEL_H

#include "../guicore_global.h"

#include "graphicswindowdatamodel.h"

class GUICOREDLL_EXPORT Graphics3DWindowDataModel : public GraphicsWindowDataModel
{
	Q_OBJECT

public:
	Graphics3DWindowDataModel(QMainWindow* w, ProjectDataItem* parent)
		: GraphicsWindowDataModel(w, parent)
	{}
};

#endif // GRAPHICS3DWINDOWDATAMODEL_H
