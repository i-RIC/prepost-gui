#ifndef GRAPHICS2DWINDOWDATAMODEL_H
#define GRAPHICS2DWINDOWDATAMODEL_H

#include "../guicore_global.h"

#include "graphicswindowdatamodel.h"
#include <misc/zdepthrange.h>

class GUICOREDLL_EXPORT Graphics2DWindowDataModel : public GraphicsWindowDataModel
{
	Q_OBJECT

public:
	Graphics2DWindowDataModel(QMainWindow* w, ProjectDataItem* parent);

public slots:
	/// Reset the rotation of pre-processor main view.
	void resetRotation();
	/// Rotate 90 degree in pre-processor main view.
	void rotate90();

protected:
	/// Z depth range used for labels, colormap legends etc.
	ZDepthRange m_labelRange;
	/// Z depth range for mouse bounding box etc.
	ZDepthRange m_hoverRange;
	/// Z depth range for data.
	ZDepthRange m_dataRange;
};

#endif // GRAPHICS2DWINDOWDATAMODEL_H
