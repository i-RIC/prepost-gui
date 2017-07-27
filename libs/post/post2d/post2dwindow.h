#ifndef POST2DWINDOW_H
#define POST2DWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include <QVector2D>
#include <guicore/post/postprocessorwindow.h>
#include <guicore/base/additionalmenuwindowi.h>
#include <guicore/base/windowwithobjectbrowserinterface.h>
#include <guicore/base/windowwithpropertybrowser.h>
#include <guicore/base/windowwithtmsi.h>
#include <guicore/bgcolor/backgroundcoloreditvtkinterface.h>
#include <postbase/cfshapeexportwindowi.h>
#include <postbase/particleexportwindowi.h>
#include <postbase/svkmlexportwindowi.h>

class QAction;
class QToolBar;
class Post2dObjectBrowser;
class Post2dWindowDataModel;
class Post2dWindowProjectDataItem;
class Post2dWindowActionManager;
class Post2dWindowGraphicsView;

/// This class represents the two-dimensional post-processing window.
class Post2dWindow :
	public PostProcessorWindow,
	public AdditionalMenuWindowI,
	public WindowWithObjectBrowserInterface,
	public WindowWithPropertyBrowser,
	public WindowWithTmsI,
	public CfShapeExportWindowI,
	public ParticleExportWindowI,
	public SVKmlExportWindowI,
	public BackgroundColorEditVtkInterface
{
	Q_OBJECT

public:
	Post2dWindow(QWidget* parent, int index, Post2dWindowProjectDataItem* pdi);
	~Post2dWindow();

	/// Informed that CGNS file is switched.
	void handleCgnsSwitch() {}
	/// switch to the new index.
	void changeIndex(uint /*newindex*/) {}
	QPixmap snapshot() override;
	vtkRenderWindow* getVtkRenderWindow() const override;

	QList<QMenu*> getAdditionalMenus() const override;
	QToolBar* getAdditionalToolBar() const override;

	ObjectBrowser* objectBrowser() const override;
	int index() const {return m_index;}

	QList<QString> particleDrawingZones() override;
	QList<QString> contourFigureDrawingZones() override;
	QList<QString> contourDrawingZones() override;
	bool hasTransparentPart();

	bool exportContourFigureToShape(const QString& filePrefix, int index, double time, const QString& zonename) override;
	bool exportParticles(const QString& filePrefix, int fileIndex, double time, const QString& zonename) override;

	virtual bool checkShapeExportCondition(const QString &zonename) const;

	bool checkKmlExportCondition(const QString &zonename) const override;
	bool exportKMLHeader(QXmlStreamWriter& writer, const QString& zonename) override;
	bool exportKMLFooter(QXmlStreamWriter& writer, const QString& zonename) override;
	bool exportKMLForTimestep(QXmlStreamWriter& writer, int index, double time, const QString& zonename) override;

	void applyOffset(double x_diff, double y_diff) override;

	void updateTmsList() override;

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
	void editZScale();

signals:
	void worldPositionChangedForStatusBar(const QVector2D& pos);

private:
	void init();

	VTKGraphicsView* viewForBackgroundColor() const override;
	void setupDefaultGeometry(int index) override;

	Post2dObjectBrowser* m_objectBrowser;
	Post2dWindowDataModel* m_dataModel;
	Post2dWindowActionManager* m_actionManager;
	QByteArray m_initialState;
	mutable QString m_exportScalarName;

public:
	friend class Post2dWindowProjectDataItem;
	friend class Post2dWindowActionManager;
	friend class Post2dWindowDataModel;
};

#endif // POST2DWINDOW_H
