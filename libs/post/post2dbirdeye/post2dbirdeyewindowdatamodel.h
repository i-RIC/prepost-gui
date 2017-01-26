#ifndef POST2DBIRDEYEWINDOWDATAMODEL_H
#define POST2DBIRDEYEWINDOWDATAMODEL_H

#include <guicore/datamodel/graphics3dwindowdatamodel.h>

class PostSolutionInfo;
class Post2dBirdEyeWindow;
class Post2dBirdEyeWindowRootDataItem;
class Post2dBirdEyeWindowGraphicsView;
class Post2dBirdEyeObjectBrowser;
class Post2dBirdEyeObjectBrowserView;
class Post2dBirdEyeWindowZoneDataItem;

/// This class handles ALL informations that are handled
/// inside 2d post-processing window.
class Post2dBirdEyeWindowDataModel : public Graphics3DWindowDataModel
{
	Q_OBJECT

public:
	static const int MOVEWIDTH = 1;
	/// Constructor
	Post2dBirdEyeWindowDataModel(Post2dBirdEyeWindow* w, ProjectDataItem* parent);
	~Post2dBirdEyeWindowDataModel();
	/// The view to display 2d post-processor main region
	Post2dBirdEyeWindowGraphicsView* graphicsView() const;
	PostSolutionInfo* postSolutionInfo();
	void editZScale();

public slots:
	void addContour();
	void gridShapeSetting();
	void contourSetting();
	void titleSetting();
	void timeSetting();

private:
	void createCameraConnections();
	void init();
	Post2dBirdEyeWindowZoneDataItem* getZoneDataItem();

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	double m_zScale;

public:
	friend class Post2dBirdEyeWindowDataModelProjectDataItem;
	friend class Post2dBirdEyeWindowProjectDataItem;
	friend class Post2dBirdEyeWindowRootDataItem;
	friend class Post2dBirdEyeWindow;
};

#endif // POST2DBIRDEYEWINDOWDATAMODEL_H
