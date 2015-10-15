#ifndef POST2DWINDOW_H
#define POST2DWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include <QVector2D>
#include <guicore/post/postprocessorwindow.h>
#include <guicore/base/additionalmenuwindowinterface.h>
#include <guicore/base/windowwithobjectbrowserinterface.h>
#include <guicore/base/windowwithpropertybrowser.h>
#include <postbase/cfshapeexportwindowinterface.h>
#include <postbase/particleexportwindowinterface.h>
#include <postbase/svkmlexportwindowinterface.h>

class QAction;
class QToolBar;
class Post2dObjectBrowser;
class Post2dWindowDataModel;
class Post2dWindowProjectDataItem;
class Post2dWindowActionManager;
class Post2dWindowGraphicsView;
class Post2dWindowEditBackgroundColorCommand;

/// This class represents the two-dimensional post-processing window.
class Post2dWindow :
	public PostProcessorWindow,
	public AdditionalMenuWindowInterface,
	public WindowWithObjectBrowserInterface,
	public WindowWithPropertyBrowser,
	public CfShapeExportWindowInterface,
	public ParticleExportWindowInterface,
	public SVKmlExportWindowInterface
{
	Q_OBJECT

public:
	/// Constructor
	Post2dWindow(QWidget* parent, int index, Post2dWindowProjectDataItem* pdi);
	/// Destructor
	~Post2dWindow();
	/// Informed that CGNS file is switched.
	void handleCgnsSwitch() {}
	/// switch to the new index.
	void changeIndex(uint /*newindex*/) {}
	QPixmap snapshot() override;
	vtkRenderWindow* getVtkRenderWindow() const override;
	QList<QMenu*> getAdditionalMenus() const override;
	ObjectBrowser* objectBrowser() const override;
	int index() const {return m_index;}
	QList<QString> particleDrawingZones() override;
	QList<QString> contourFigureDrawingZones() override;
	bool exportContourFigureToShape(const QString& filePrefix, int index, double time, const QString& zonename) override;
	bool exportParticles(const QString& filePrefix, int fileIndex, double time, const QString& zonename) override;
	bool checkKmlExportCondition(const QString &zonename) const override;
	bool exportKMLHeader(QXmlStreamWriter& writer, const QString& zonename) override;
	bool exportKMLFooter(QXmlStreamWriter& writer, const QString& zonename) override;
	bool exportKMLForTimestep(QXmlStreamWriter& writer, int index, double time, const QString& zonename) override;
	QList<QString> contourDrawingZones() override;
	bool hasTransparentPart();

public slots:
	void cameraFit();
	void cameraResetRotation();
	void cameraRotate90();
	void cameraZoomIn();
	void cameraZoomOut();
	void cameraMoveLeft();
	void cameraMoveRight();
	void cameraMoveUp();
	void cameraMoveDown();
	void editBackgroundColor();

signals:
	void worldPositionChangedForStatusBar(const QVector2D& pos);

private:
	void init();
	/// Background color
	const QColor backgroundColor() const;
	/// Set background color;
	void setBackgroundColor(QColor& c);
	void setupDefaultGeometry(int index) override;
	Post2dObjectBrowser* m_objectBrowser;
	Post2dWindowDataModel* m_dataModel;
	Post2dWindowActionManager* m_actionManager;
	QByteArray m_initialState;

public:
	friend class Post2dWindowProjectDataItem;
	friend class Post2dWindowActionManager;
	friend class Post2dWindowDataModel;
	friend class Post2dWindowEditBackgroundColorCommand;
};

#endif // POST2DWINDOW_H
