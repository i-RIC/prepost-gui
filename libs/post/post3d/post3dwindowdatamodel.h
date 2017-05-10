#ifndef POST3DWINDOWDATAMODEL_H
#define POST3DWINDOWDATAMODEL_H

#include <guicore/datamodel/graphics3dwindowdatamodel.h>

class PostSolutionInfo;
class Post3dWindow;
class Post3dWindowRootDataItem;
class Post3dWindowZoneDataItem;
class Post3dWindowGraphicsView;
class Post3dObjectBrowser;
class Post3dObjectBrowserView;

/// This class handles ALL informations that are handled
/// inside 3d post-processing window.
class Post3dWindowDataModel : public Graphics3DWindowDataModel
{
	Q_OBJECT

public:
	static const int MOVEWIDTH = 1;
	/// Constructor
	Post3dWindowDataModel(Post3dWindow* w, ProjectDataItem* parent);
	~Post3dWindowDataModel();
	/// The view to display 3d post-processor main region
	Post3dWindowGraphicsView* graphicsView() const;
	PostSolutionInfo* postSolutionInfo();
	void editZScale();

public slots:
	void addContour();
	void addIsosurface();
	void gridShapeSetting();
	void surfaceSetting();
	void contourSetting();
	void arrowSetting();
	void streamlineSetting();
	void particleSetting();
	void isosurfaceSetting();
	void titleSetting();
	void timeSetting();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void init();
	Post3dWindowZoneDataItem* getZoneDataItem();

private:
	double m_zScale;

public:
	friend class Post3dWindow;
	friend class Post3dWindowDataModelProjectDataItem;
	friend class Post3dWindowProjectDataItem;
	friend class Post3dWindowRootDataItem;
};

#endif // PREPROCESSORDATAMODEL_H
