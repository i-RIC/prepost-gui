#ifndef POST2DWINDOW_H
#define POST2DWINDOW_H

#include "post2dwindowdatamodel.h"

#include <QMainWindow>
#include <QByteArray>
#include <QVector2D>
#include <guicore/post/postprocessorwindow.h>
#include <guicore/base/additionalmenuwindowi.h>
#include <guicore/base/windowwithobjectbrowseri.h>
#include <guicore/base/windowwithpropertybrowser.h>
#include <guicore/base/windowwithtmsi.h>
#include <guicore/bgcolor/backgroundcoloreditvtki.h>
#include <guicore/datamodel/windowwithvtkgraphicsviewi.h>
#include <postbase/autoparticlewindowi.h>
#include <postbase/cfshapeexportwindowi.h>
#include <postbase/particleexportwindowi.h>
#include <postbase/svkmlexportwindowi.h>

#include <string>

class QAction;
class QToolBar;
class Post2dObjectBrowser;
class Post2dWindowProjectDataItem;
class Post2dWindowActionManager;
class Post2dWindowGraphicsView;

/// This class represents the two-dimensional post-processing window.
class Post2dWindow :
	public PostProcessorWindow,
	public AdditionalMenuWindowI,
	public WindowWithObjectBrowserI,
	public WindowWithPropertyBrowser,
	public WindowWithTmsI,
	public CfShapeExportWindowI,
	public ParticleExportWindowI,
	public SVKmlExportWindowI,
	public BackgroundColorEditVtkI,
	public AutoParticleWindowI,
	public WindowWithVtkGraphicsViewI
{
	Q_OBJECT

public:
	Post2dWindow(QWidget* parent, int index, Post2dWindowProjectDataItem* pdi);
	~Post2dWindow();

	QPixmap snapshot() const override;
	QWidget* snapshotArea() const override;
	vtkRenderWindow* getVtkRenderWindow() const override;
	VTKGraphicsView* getVtkGraphicsView() const override;

	QList<QMenu*> getAdditionalMenus() const override;
	const std::shared_ptr<QToolBar>& getAdditionalToolBar() const override;

	ObjectBrowser* objectBrowser() const override;
	Post2dWindowDataModel* dataModel() const override;
	int index() const {return m_index;}

	QList<QString> particleDrawingZones() override;
	std::vector<std::string> discreteColorDrawingZones() override;
	bool hasTransparentPart();

	bool exportContourFigureToShape(const QString& filePrefix, int index, double time, const std::string& zonename) override;
	bool exportParticles(const QString& filePrefix, int fileIndex, double time, const QString& zonename) override;

	bool checkShapeExportCondition(const std::string& zoneName) const override;

	bool checkKmlExportCondition(const std::string &zonename) const override;
	bool exportKMLHeader(QXmlStreamWriter& writer, const std::string& zonename) override;
	bool exportKMLFooter(QXmlStreamWriter& writer, const std::string& zonename) override;
	bool exportKMLForTimestep(QXmlStreamWriter& writer, int index, double time, const std::string& zonename, bool oneShot) override;
	bool isAutoParticleOutput() const override;

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
	void worldPositionChangedForStatusBar(const QPointF& pos);

private:
	void init();

	VTKGraphicsView* viewForBackgroundColor() const override;
	void setupDefaultGeometry(int index) override;

	Post2dObjectBrowser* m_objectBrowser;
	Post2dWindowDataModel* m_dataModel;
	Post2dWindowActionManager* m_actionManager;
	QByteArray m_initialState;
	mutable std::string m_exportScalarName;

public:
	friend class Post2dWindowProjectDataItem;
	friend class Post2dWindowActionManager;
	friend class Post2dWindowDataModel;
};

#endif // POST2DWINDOW_H
