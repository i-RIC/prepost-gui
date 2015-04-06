#ifndef PREPROCESSORDATAMODEL_H
#define PREPROCESSORDATAMODEL_H

#include <guicore/pre/base/preprocessordatamodelinterface.h>
#include <QList>
#include <QMap>

class SolverDefinitionGridType;
class PreProcessorWindow;
class ProjectDataItem;
class PreProcessorDataItem;
class PreProcessorRootDataItem;
class PreProcessorGraphicsView;
class MouseBoundingBox;
class PreProcessorGridTypeDataItem;
class PreProcessorGridCreatingConditionDataItemInterface;
class PreProcessorRawDataGroupDataItemInterface;
class PreProcessorRawdataDataItemInterface;
class PreProcessorGridAndGridCreatingConditionDataItemInterface;
class PreProcessorGridDataItem;
class PreProcessorRawDataGroupDataItem;
class PreProcessorRawdataDataItem;
class PreProcessorBackgroundImageDataItem;
class PreProcessorGridCreatingConditionDataItem;
class PreProcessorRawDataTopDataItemInterface;
class PreProcessorBCSettingDataItem;
class PreProcessorBCDataItem;
class MeasuredDataFileDataItem;
class RawDataCreator;
class QAction;
class QSignalMapper;

/// This class handles ALL informations that are handled
/// inside pre-processing window.
class PreProcessorDataModel : public PreProcessorDataModelInterface
{
	Q_OBJECT
private:
	static const int OLDMENULIST_MAX = 5;

public:
	/// Constructor
	PreProcessorDataModel(PreProcessorWindow* w, ProjectDataItem* parent);
	/// Destructor
	~PreProcessorDataModel();

	/// The view to display pre-processor main region
	PreProcessorGraphicsViewInterface* graphicsView() const;
	/// Import input condition from the specified CGNS file.
	bool importInputCondition(const QString& filename);
	bool exportInputCondition(const QString& filename);
	bool gridEdited();
	void toggleGridEditFlag();
	bool isInputConditionSet();
	MouseBoundingBox* mouseBoundingBox(){return m_mouseBoundingBox;}
	const QList<QMenu*>& additionalMenus() const {
		return m_additionalMenus;
	}
	void addGridImportMenu(QMenu* menu);
	void addGridExportMenu(QMenu* menu);
	void setupRawDataImportMenu(QMenu* menu);
	void setupRawDataExportMenu(QMenu* menu);
	void setupHydraulicDataImportMenu(QMenu* menu);
	void disableGridMenu();
	void informUnfocusRiverCrosssectionWindows();
	bool isSetupCorrectly() const;
	bool checkMappingStatus();
	PreProcessorRawDataTopDataItemInterface* rawDataTopDataItem(const QString &type) const;
	PreProcessorGridAndGridCreatingConditionDataItemInterface* getGridAndGridCreatingConditionDataItem(const QString &typeName, const QString& zoneName) const;
	void applyOffset(double x, double y);

public slots:
	void showCalcConditionDialog();
	void importCalcCondition();
	void exportCalcCondition();
	void setupAdditinalMenus();

protected slots:
	virtual void handleObjectBrowserSelectionChange(){
		setupAdditinalMenus();
	}
	void setupGridCreationMenuContent();
	void setupBoundaryConditionSettingMenuContent();
	void importHydraulicData();
signals:
	void additionalMenusUpdated(const QList<QMenu*>& menus);

protected:
	void init();

private:
	void importCalcConditionFromOtherProject(const QString& fname);
	void importCalcConditionFromCGNS(const QString& fname);

	GraphicsWindowRootDataItem* rootDataItem(){return m_rootDataItem;}
	void setupRawDataAddActions(PreProcessorRawDataGroupDataItem* item);
	void setupRawDataMenus();
	QMenu* setupGridCreationMenu(QMenu* parentMenu, PreProcessorGridCreatingConditionDataItemInterface* gcItem);
	QMenu* setupBoundaryConditionSettingMenu(QMenu* parentMenu);
	void setupGridMenu();
	void setupMeasuredValuesMenu();
	static PreProcessorGridTypeDataItem* getGridTypeItem(GraphicsWindowDataItem* item);
	PreProcessorGridDataItem* getGridItem(GraphicsWindowDataItem* item);
	MeasuredDataFileDataItem* getMeasuredDataItem(GraphicsWindowDataItem* item);
	static PreProcessorGridDataItem* getGridItemRecursively(GraphicsWindowDataItem* item);
	static MeasuredDataFileDataItem* getMeasuredDataItemRecursively(GraphicsWindowDataItem* item);
	static PreProcessorBackgroundImageDataItem* getBackgroundImageItem(GraphicsWindowDataItem* item);
	static PreProcessorBCSettingDataItem* getBCSettingDataItem(GraphicsWindowDataItem* item);
	bool addGridImportMenuForGridType(QMenu* menu, PreProcessorGridTypeDataItem* gt, bool alone);
	bool addGridImportMenuForGrid(QMenu* menu, PreProcessorGridAndGridCreatingConditionDataItemInterface* di, const QString& name);
	bool addGridExportMenuForGridType(QMenu* menu, PreProcessorGridTypeDataItem* gt, bool setname);
	bool addGridExportMenuForGrid(QMenu* menu, PreProcessorGridAndGridCreatingConditionDataItemInterface* di, const QString& name);
	bool setupRawDataImportMenuForGridType(QMenu* menu, PreProcessorGridTypeDataItem* gt);
	bool setupRawDataImportMenuForGroup(QMenu* menu, PreProcessorRawDataGroupDataItemInterface* gt);
	bool setupRawDataExportMenuForGridType(QMenu* menu, PreProcessorGridTypeDataItem* gt);
	bool setupRawDataExportMenuForGroup(QMenu* menu, PreProcessorRawDataGroupDataItem* gt);
	bool setupRawDataExportMenuForItem(QMenu* menu, PreProcessorRawdataDataItemInterface* gt);
	bool rawDataExportAvailable(PreProcessorRawDataGroupDataItemInterface* gt);

	MouseBoundingBox* m_mouseBoundingBox;
	QList<QMenu*> m_additionalMenus;
	QList<QMenu*> m_dummyMenus;
	QList<QList<QMenu*> > m_oldDummyMenusList;
	QMap<RawDataCreator*, QAction*> m_rawDataAddActions;
	QSignalMapper* m_rawDataAddSignalMapper;
	QMenu* m_geographicDataMenu;
	QMenu* m_gridMenu;
	QMenu* m_measuredValuesMenu;
	QMenu* m_backgroundImageMenu;

public:
	friend class PreProcessorWindowProjectDataItem;
	friend class PreProcessorRootDataItem;
	friend class PreProcessorWindow;
	friend class PreProcessorWindowActionManager;
};

#endif // PREPROCESSORDATAMODEL_H
