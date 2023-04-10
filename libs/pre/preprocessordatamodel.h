#ifndef PREPROCESSORDATAMODEL_H
#define PREPROCESSORDATAMODEL_H

#include "pre_global.h"
#include <guicore/pre/base/preprocessordatamodelinterface.h>
#include <QList>
#include <QMap>

namespace iRICPython {
	class Pre;
} // iRICPython

class SolverDefinitionGridType;
class PreProcessorWindow;
class ProjectDataItem;
class PreProcessorDataItem;
class PreProcessorRootDataItem;
class PreProcessorGraphicsView;
class MouseBoundingBox;
class PreProcessorGridTypeDataItem;
class PreProcessorGridCreatingConditionDataItemInterface;
class PreProcessorGeoDataGroupDataItemInterface;
class PreProcessorGeoDataDataItemInterface;
class PreProcessorGridAndGridCreatingConditionDataItemInterface;
class PreProcessorGridDataItem;
class PreProcessorGeoDataGroupDataItem;
class PreProcessorGeoDataDataItem;
class PreProcessorBackgroundImageDataItem;
class PreProcessorGridCreatingConditionDataItem;
class PreProcessorGeoDataTopDataItemInterface;
class PreProcessorBCSettingDataItem;
class PreProcessorBCDataItem;
class MeasuredDataFileDataItem;
class GeoDataCreator;
class QAction;
class QSignalMapper;

/// This class handles ALL informations that are handled
/// inside pre-processing window.
class PREDLL_EXPORT PreProcessorDataModel : public PreProcessorDataModelInterface
{
	Q_OBJECT

private:
	static const int OLDMENULIST_MAX = 5;

public:
	PreProcessorDataModel(PreProcessorWindow* w, ProjectDataItem* parent);
	~PreProcessorDataModel();

	/// The view to display pre-processor main region
	PreProcessorGraphicsViewInterface* graphicsView() const override;
	/// Import input condition from the specified CGNS file.
	bool importInputCondition(const QString& filename);
	bool exportInputCondition(const QString& filename);
	bool setupCgnsFilesIfNeeded(bool readGrid);
	bool isGridEdited() const;
	void setGridEdited();
	bool isInputConditionSet();
	MouseBoundingBox* mouseBoundingBox() override {return m_mouseBoundingBox;}
	const QList<QMenu*>& additionalMenus() const {
		return m_additionalMenus;
	}
	void addGridCreatingConditionImportMenu(QMenu* menu);
	void addGridCreatingConditionExportMenu(QMenu* menu);
	void addGridImportMenu(QMenu* menu);
	void addGridExportMenu(QMenu* menu);
	void setupGeoDataImportMenu(QMenu* menu);
	void setupGeoDataImportFromWebMenu(QMenu* menu);
	void setupGeoDataExportMenu(QMenu* menu);
	void setupHydraulicDataImportMenu(QMenu* menu);
	void disableGridMenu();
	void informUnfocusRiverCrosssectionWindows();
	bool isSetupCorrectly() const;
	bool checkMappingStatus();
	PreProcessorGridTypeDataItemInterface* gridTypeDataItem(const std::string& type) const override;
	PreProcessorGeoDataTopDataItemInterface* geoDataTopDataItem() const override;
	PreProcessorGeoDataTopDataItemInterface* geoDataTopDataItem(const std::string& type) const override;
	PreProcessorHydraulicDataTopDataItemInterface* hydraulicDataTopDataItem(const std::string& type) const override;
	PreProcessorGridAndGridCreatingConditionDataItemInterface* getGridAndGridCreatingConditionDataItem(const std::string& typeName, const std::string& zoneName) const override;
	void applyOffset(double x, double y) override;

	void updateTmsList();

public slots:
	void showCalcConditionDialog();
	void importCalcCondition();
	void exportCalcCondition();
	void setupAdditinalMenus();

	void handleObjectBrowserSelectionChange() override;

	void setupGridCreationMenuContent();
	void setupBoundaryConditionSettingMenuContent();

private slots:
	void setupGeoDataSubMenu();

signals:
	void additionalMenusUpdated(const QList<QMenu*>& menus);

protected:
	void init();

private:
	void importCalcConditionFromOtherProject(const QString& fname);
	void importCalcConditionFromCGNS(const QString& fname);
	void importCalcConditionFromYaml(const QString& fname);

	PreProcessorRootDataItem* rootDataItem() const;

	void setupGeoDataAddActions(PreProcessorGeoDataGroupDataItem* item);
	void setupGeoDataMenus();
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

	bool addGridCreatingConditionImportMenuForGridType(QMenu* menu, PreProcessorGridTypeDataItem* gt, bool alone);
	bool addGridCreatingConditionImportMenuForGrid(QMenu* menu, PreProcessorGridAndGridCreatingConditionDataItemInterface* di, const QString& name);
	bool addGridCreatingConditionExportMenuForGridType(QMenu* menu, PreProcessorGridTypeDataItem* gt, bool alone);
	bool addGridCreatingConditionExportMenuForGrid(QMenu* menu, PreProcessorGridAndGridCreatingConditionDataItemInterface* di, const QString& name);
	bool addGridImportMenuForGridType(QMenu* menu, PreProcessorGridTypeDataItem* gt, bool alone);
	bool addGridImportMenuForGrid(QMenu* menu, PreProcessorGridAndGridCreatingConditionDataItemInterface* di, const QString& name);
	bool addGridExportMenuForGridType(QMenu* menu, PreProcessorGridTypeDataItem* gt, bool alone);
	bool addGridExportMenuForGrid(QMenu* menu, PreProcessorGridAndGridCreatingConditionDataItemInterface* di, const QString& name);
	bool setupGeoDataImportMenuForGridType(QMenu* menu, PreProcessorGridTypeDataItem* gt);
	bool setupGeoDataImportFromWebMenuForGridType(QMenu* menu, PreProcessorGridTypeDataItem* gt);
	bool setupGeoDataImportMenuForGroup(QMenu* menu, PreProcessorGeoDataGroupDataItemInterface* gt);
	bool setupGeoDataImportFromWebMenuForGroup(QMenu* menu, PreProcessorGeoDataGroupDataItemInterface* gt);
	bool setupGeoDataExportMenuForGridType(QMenu* menu, PreProcessorGridTypeDataItem* gt);
	bool setupGeoDataExportMenuForGroup(QMenu* menu, PreProcessorGeoDataGroupDataItem* gt);
	bool setupGeoDataExportMenuForItem(QMenu* menu, PreProcessorGeoDataDataItemInterface* gt);
	bool geoDataExportAvailable(PreProcessorGeoDataGroupDataItemInterface* gt);

	MouseBoundingBox* m_mouseBoundingBox;
	QList<QMenu*> m_additionalMenus;
	QList<QMenu*> m_dummyMenus;
	QList<QList<QMenu*> > m_oldDummyMenusList;
	QMap<GeoDataCreator*, QAction*> m_geoDataAddActions;
	QSignalMapper* m_geoDataAddSignalMapper;
	QMenu* m_geographicDataMenu;
	QMenu* m_gridMenu;
	QMenu* m_measuredValuesMenu;
	QMenu* m_backgroundImageMenu;

public:
	friend class PreProcessorWindowProjectDataItem;
	friend class PreProcessorRootDataItem;
	friend class PreProcessorWindow;
	friend class PreProcessorWindowActionManager;

	friend class iRICPython::Pre;
};

#endif // PREPROCESSORDATAMODEL_H
