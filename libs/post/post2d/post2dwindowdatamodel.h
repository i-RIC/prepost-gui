#ifndef POST2DWINDOWDATAMODEL_H
#define POST2DWINDOWDATAMODEL_H

#include "post2d_global.h"
#include <guicore/datamodel/graphics2dwindowdatamodel.h>

class PostSolutionInfo;
class Post2dWindow;
class Post2dWindowRootDataItem;
class Post2dWindowGraphicsView;
class Post2dObjectBrowser;
class Post2dObjectBrowserView;
class Post2dWindowZoneDataItem;
class MeasuredDataFileDataItem;

/// This class handles ALL informations that are handled
/// inside 2d post-processing window.
class POST2D_EXPORT Post2dWindowDataModel : public Graphics2DWindowDataModel
{
	Q_OBJECT

public:
	static const int MOVEWIDTH = 1;
	/// Constructor
	Post2dWindowDataModel(Post2dWindow* w, ProjectDataItem* parent);
	~Post2dWindowDataModel();
	/// The view to display 2d post-processor main region
	Post2dWindowGraphicsView* graphicsView() const;
	PostSolutionInfo* postSolutionInfo();

public slots:
	void gridShapeSetting();
	void contourSetting();
	void vectorSetting();
	void streamlineSetting();
	void particleSetting();
	void cellFlagSetting();
	void titleSetting();
	void timeSetting();

	void measuredDataPointSetting();
	void measuredDataVectorSetting();

private:
	void createCameraConnections();
	void init();
	Post2dWindowZoneDataItem* getZoneDataItem();
	MeasuredDataFileDataItem* getMeasuredDataFileDataItem();

public:
	friend class Post2dWindowDataModelProjectDataItem;
	friend class Post2dWindowProjectDataItem;
	friend class Post2dWindowRootDataItem;
	friend class Post2dWindow;
};

#endif // PREPROCESSORDATAMODEL_H
