#ifndef PREPROCESSORGEODATAGROUPDATAITEM_H
#define PREPROCESSORGEODATAGROUPDATAITEM_H

#include "../pre_global.h"

#include <guibase/scalarbarsetting.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>

class SolverDefinitionGridAttribute;
class PreProcessorGeoDataDataItem;
class PreProcessorScalarBarLegendBoxSettingDialog;
class GridAttributeEditWidget;
class Grid;
class GeoDataImporter;
class GeoDataPolygon;
class GeoDataRiverSurvey;
class GeoDataRiverSurveyCrosssectionWindowProjectDataItem;
class WaitDialog;

class QMenu;
class QModelIndex;
class QSignalMapper;

class PREDLL_EXPORT PreProcessorGeoDataGroupDataItem : public PreProcessorGeoDataGroupDataItemInterface
{
	Q_OBJECT

public:
	PreProcessorGeoDataGroupDataItem(SolverDefinitionGridAttribute* cond, PreProcessorDataItem* parent);
	~PreProcessorGeoDataGroupDataItem() override;

	void addCustomMenuItems(QMenu* menu) override;
	void closeCgnsFile() override;
	SolverDefinitionGridAttribute* condition() override;
	bool isChildCaptionAvailable(const QString& cap);

	int mappingCount() const;
	void executeMapping(Grid* grid, WaitDialog* dialog);

	void setDefaultValue(Grid* grid);
	void informValueRangeChange();
	void informDataChange();
	bool getValueRange(double* min, double* max) override;
	void updateZDepthRangeItemCount() override;

	bool importAvailable();
	bool webImportAvailable();

	void clearDimensionsIfNoDataExists();

	const QList<PreProcessorGeoDataDataItemInterface*> geoDatas() const override;
	void editScalarBarLegendBox(PreProcessorScalarBarLegendBoxSettingDialog* dialog);
	ScalarBarSetting& scalarBarSetting();
	const QString& scalarBarTitle() const;
	QAction* importAction() const;
	bool addImportAction(QMenu* menu);
	bool addImportFromWebAction(QMenu* menu);
	QStringList getGeoDatasNotMapped();
	void addCopyPolygon(GeoDataPolygon* polygon) override;
	void addGeoData(PreProcessorGeoDataDataItemInterface *geoData) override;
	GridAttributeDimensionsContainer* dimensions() const override;
	PreProcessorGeoDataDataItemInterface* buildGeoDataDataItem() override;
	GeoDataCreator* getPointMapCreator() override;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void keyPressEvent(QKeyEvent* event, VTKGraphicsView* v) override;
	void keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v) override;
	void mouseDoubleClickEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	bool polygonExists() const;
	void saveToCgnsFile(const int fn) override;
	virtual void saveComplexGroupsToCgnsFile(const int fn);
	void setupEditWidget(GridAttributeEditWidget* widget) override;
	void updateCrossectionWindows() override;
	void openCrossSectionWindow(GeoDataRiverSurvey* rs, const QString& crosssection) override;
	void toggleCrosssectionWindowsGridCreatingMode(bool gridMode, GeoDataRiverSurvey* rs) override;
	void informCtrlPointUpdateToCrosssectionWindows() override;
	void requestCrosssectionWindowDelete(GeoDataRiverSurveyCrosssectionWindowProjectDataItem* item) override;
	bool addToolBarButtons(QToolBar* parent) override;
	QStringList containedFiles() override;
	void setDimensionsToFirst();

	std::vector<GeoDataImporter*> importers() const;
	GeoDataImporter* importer(const std::string& name) const;

public slots:
	void addGeoData(QObject* creator);
	virtual void editScalarsToColors();
	void import();
	void importFromWeb();
	void doExport();
	void exportAllPolygons();

private slots:
	void deleteSelected();
	void deleteAll();
	void cancelImport();

signals:
	void selectGeoData(const QModelIndex& current);

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;
	void setupConnectionToGeoData(GeoData* geodata);

	void addBackground();

	QMenu* m_addMenu;
	QAction* m_importAction;
	QAction* m_webImportAction;
	QAction* m_editColorMapAction;
	QAction* m_setupScalarBarAction;
	QAction* m_exportAllPolygonsAction;
	QAction* m_deleteSelectedAction;
	QAction* m_deleteAllAction;
	QSignalMapper* m_addSignalMapper;
	SolverDefinitionGridAttribute* m_condition;
	GridAttributeDimensionsContainer* m_dimensions;

	ScalarBarSetting m_scalarBarSetting;
	QString m_scalarBarTitle;

	PreProcessorGeoDataDataItem* m_backgroundItem;

private:
	QList<GeoDataRiverSurveyCrosssectionWindowProjectDataItem*> m_crosssectionWindows;

	bool m_cancelImport;
};

#endif // PREPROCESSORGEODATAGROUPDATAITEM_H
