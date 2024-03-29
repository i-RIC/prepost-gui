#ifndef PREPROCESSORWINDOW_H
#define PREPROCESSORWINDOW_H

#include "pre_global.h"
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/base/additionalmenuwindowi.h>
#include <guicore/base/windowwithobjectbrowseri.h>
#include <guicore/base/windowwithpropertybrowser.h>
#include <guicore/base/windowwithtmsi.h>
#include <guicore/base/windowwithzindexi.h>
#include <guicore/datamodel/windowwithvtkgraphicsviewi.h>
#include <guicore/pre/base/preprocessordatamodeli.h>
#include <guicore/pre/base/preprocessorwindowi.h>
#include <guicore/bgcolor/backgroundcoloreditvtki.h>

#include <QCloseEvent>
#include <QByteArray>

class QMenu;
class QAction;
class QModelIndex;
class QToolBar;
class PreObjectBrowser;
class PrePropertyBrowser;
class ProjectData;
class ProjectDataItem;
class PreProcessorWindowProjectDataItem;
class PreProcessorDataModel;
class PreProcessorWindowActionManager;
class PreProcessorGraphicsView;

/// PreProcessorWindow class implements the main window of pre-processor.
class PREDLL_EXPORT PreProcessorWindow :
	public PreProcessorWindowI,
	public AdditionalMenuWindowI,
	public WindowWithObjectBrowserI,
	public WindowWithTmsI,
	public WindowWithPropertyBrowser,
	public WindowWithZIndexI,
	public BackgroundColorEditVtkI,
	public WindowWithVtkGraphicsViewI
{
	Q_OBJECT

public:
	enum GridState {
		grFinished,
		grPartiallyUnfinished,
		grUnfinished
	};
	PreProcessorWindow(QWidget* parent);
	~PreProcessorWindow();
	/// Set newly created project data.
	void setProjectData(ProjectData* d);
	PreProcessorWindowProjectDataItem* projectDataItem();
	/// Close event handler
	/**
	 * When close event occures, Preprocessor window is not close,
	 * but only hidden.
	 */
	void closeEvent(QCloseEvent* e) override;
	/// Setup window position and size to the default.
	void setupDefaultGeometry();
	/// Import Calculation Condition from specified file.
	bool importInputCondition(const QString& filename);
	/// Export Calculation Condition into specified file.
	bool exportInputCondition(const QString& filename);
	bool isInputConditionSet();
	GridState checkGridState();
	void checkCalculationConditionImportSourceUpdate() override;
	/// Check grid shape and if problems found, returns warning message.
	QString checkGrid(bool detail) override;
	void showEvent(QShowEvent* e) override;
	void hideEvent(QHideEvent* e) override;
	QPixmap snapshot() const override;
	QWidget* snapshotArea() const override;
	vtkRenderWindow* getVtkRenderWindow() const override;
	QList<QMenu*> getAdditionalMenus() const override;
	const std::shared_ptr<QToolBar>& getAdditionalToolBar() const override;
	ObjectBrowser* objectBrowser() const override;
	QMenu* calcCondMenu() const override;
	void addGridCreatingConditionImportMenu(QMenu* menu);
	void addGridCreatingConditionExportMenu(QMenu* menu);
	void addGridImportMenu(QMenu* menu);
	void addGridExportMenu(QMenu* menu);
	void informUnfocusRiverCrosssectionWindows();
	bool isSetupCorrectly() const;
	bool checkMappingStatus() override;
	PreProcessorDataModelI* dataModel() const override;
	VTKGraphicsView* getVtkGraphicsView() const override;
	bool setupCgnsFilesIfNeeded(bool readGrid);

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
	void importCalcCondition();
	void exportCalcCondition();
	void showCalcConditionDialog();
	void handleAdditionalMenusUpdate(const QList<QMenu*>& menus);
	void setupGeoDataImportMenu();
	void setupGeoDataImportFromWebMenu();
	void setupGeoDataExportMenu();
	void setupHydraulicDataImportMenu();

signals:
	void additionalMenusUpdated(const QList<QMenu*>& menus);
	void worldPositionChangedForStatusBar(const QPointF& pos);

private:
	void init();
	PreProcessorDataModel* model() const;
	VTKGraphicsView* viewForBackgroundColor() const override;
	/// Object browser
	PreObjectBrowser* m_objectBrowser;
	PreProcessorDataModel* m_dataModel;
	PreProcessorWindowProjectDataItem* m_projectDataItem;
	PreProcessorWindowActionManager* m_actionManager;
	PreProcessorGraphicsView* m_graphicsView;
	QByteArray m_initialState;
	bool m_isFirstHiding;
	bool m_isLastHiding;

public:
	friend class PreProcessorWindowProjectDataItem;
	friend class PreProcessorWindowActionManager;
	friend class BackgroundImageInfo;
};

#endif // PREPROCESSORWINDOW_H
