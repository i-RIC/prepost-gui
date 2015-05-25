#ifndef PREPROCESSORWINDOW_H
#define PREPROCESSORWINDOW_H

#include "pre_global.h"
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/base/snapshotenabledwindowinterface.h>
#include <guicore/base/additionalmenuwindowinterface.h>
#include <guicore/base/windowwithobjectbrowserinterface.h>
#include <guicore/base/windowwithpropertybrowser.h>
#include <guicore/base/windowwithzindexinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>

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
class PreProcessorWindowEditBackgroundColorCommand;

/// PreProcessorWindow class implements the main window of pre-processor.
class PREDLL_EXPORT PreProcessorWindow :
	public PreProcessorWindowInterface,
	public SnapshotEnabledWindowInterface,
	public AdditionalMenuWindowInterface,
	public WindowWithObjectBrowserInterface,
	public WindowWithPropertyBrowser,
	public WindowWithZIndexInterface
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
	/// Check grid shape and if problems found, returns warning message.
	const QString checkGrid(bool detail) override;
	void showEvent(QShowEvent* e) override;
	void hideEvent(QHideEvent* e) override;
	//public slots:
//	void switchGridMode(SolverDefinition::GridType /*gt*/){}
	QPixmap snapshot() override;
	vtkRenderWindow* getVtkRenderWindow() const override;
	QList<QMenu*> getAdditionalMenus() const override;
	QToolBar* getAdditionalToolBar() const override;
	ObjectBrowser* objectBrowser() const override;
	void addGridImportMenu(QMenu* menu);
	void addGridExportMenu(QMenu* menu);
	void informUnfocusRiverCrosssectionWindows();
	bool isSetupCorrectly() const;
	bool checkMappingStatus() override;
	const PreProcessorDataModelInterface* dataModel() const override {return m_dataModel;}
	PreProcessorDataModelInterface* dataModel() override { return m_dataModel; }

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
	void showCalcConditionDialog();
	void handleAdditionalMenusUpdate(const QList<QMenu*>& menus);
	void setupRawDataImportMenu();
	void setupRawDataExportMenu();
	void setupHydraulicDataImportMenu();

signals:
	void additionalMenusUpdated(const QList<QMenu*>& menus);
	void worldPositionChangedForStatusBar(const QVector2D& pos);
private:
	void init();
	PreProcessorDataModel* model() const;
	/// Background color
	const QColor backgroundColor() const;
	/// Set background color;
	void setBackgroundColor(const QColor& c);
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
	friend class PreProcessorWindowEditBackgroundColorCommand;
	friend class BackgroundImageInfo;
};

#endif // PREPROCESSORWINDOW_H
