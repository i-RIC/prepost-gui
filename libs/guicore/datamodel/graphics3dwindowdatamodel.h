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

public slots:
	/// Reset the rotation of pre-processor main view.
	void resetRotation();
	/// Rotate 90 degree in pre-processor main view.
	void rotate90();
	/// Go to XY plane
	void toXYPlane();
	/// Go to YZ plane
	void toYZPlane();
	/// Go to ZX plane
	void toZXPlane();
};

#endif // GRAPHICS3DWINDOWDATAMODEL_H
