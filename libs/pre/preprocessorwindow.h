#ifndef PREPROCESSORWINDOW_H
#define PREPROCESSORWINDOW_H

#include "pre_global.h"
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/base/snapshotenabledwindowinterface.h>
#include <guicore/base/additionalmenuwindowi.h>
#include <guicore/base/windowwithobjectbrowserinterface.h>
#include <guicore/base/windowwithpropertybrowser.h>
#include <guicore/base/windowwithtmsi.h>
#include <guicore/base/windowwithzindexinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/bgcolor/backgroundcoloreditvtkinterface.h>

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
class PreProcessorDataModelInterface;
class PreProcessorDataModel;
class PreProcessorWindowActionManager;
class PreProcessorGraphicsView;

/// PreProcessorWindow class implements the main window of pre-processor.
class PREDLL_EXPORT PreProcessorWindow :
	public PreProcessorWindowInterface,
	public SnapshotEnabledWindowInterface,
	public AdditionalMenuWindowI,
	public WindowWithObjectBrowserInterface,
	public WindowWithTmsI,
	public WindowWithPropertyBrowser,
	public WindowWithZIndexInterface,
	public BackgroundColorEditVtkInterface
{
	Q_OBJECT

public:
	enum GridState {
		grFinished,
		grPartiallyUnfinished,
		grUnfinished
	};
	/// Constructor
	PreProcessorWindow(QWidget* parent);
	/// Destructor
	virtual ~PreProcessorWindow() {}
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
	QPixmap snapshot() override;
	vtkRenderWindow* getVtkRenderWindow() const override;
	QList<QMenu*> getAdditionalMenus() const override;
	QToolBar* getAdditionalToolBar() const override;
	ObjectBrowser* objectBrowser() const override;
	QMenu* calcCondMenu() const;
	void addGridCreatingConditionImportMenu(QMenu* menu);
	void addGridCreatingConditionExportMenu(QMenu* menu);
	void addGridImportMenu(QMenu* menu);
	void addGridExportMenu(QMenu* menu);
	void informUnfocusRiverCrosssectionWindows();
	bool isSetupCorrectly() const;
	bool checkMappingStatus() override;
	PreProcessorDataModelInterface* dataModel() const override;

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
	void worldPositionChangedForStatusBar(const QVector2D& pos);

private:
	void init();
	PreProcessorDataModel* model() const;
	VTKGraphicsView* viewForBackgroundColor() const override;
	/// Object browser
	PreObjectBrowser* m_objectBrowser;
	PreProcessorDataModelInterface* m_dataModel;
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
