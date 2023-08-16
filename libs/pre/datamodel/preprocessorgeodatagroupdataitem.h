#ifndef PREPROCESSORGEODATAGROUPDATAITEM_H
#define PREPROCESSORGEODATAGROUPDATAITEM_H

#include "../pre_global.h"

#include <guibase/scalarbarsetting.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/stringcontainer.h>

class ColorMapSettingToolBarWidgetController;
class SolverDefinitionGridAttribute;
class PreProcessorGeoDataDataItem;
class PreProcessorGridTypeDataItem;
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

	class VariationSetting : public CompositeContainer
	{
	public:
		VariationSetting();
		VariationSetting(const VariationSetting& s);

		VariationSetting& operator=(const VariationSetting& s);
		XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

		BoolContainer enabled;
		StringContainer activeVariation;
	};

	PreProcessorGeoDataGroupDataItem(SolverDefinitionGridAttribute* cond, PreProcessorDataItem* parent);
	~PreProcessorGeoDataGroupDataItem() override;

	void addCustomMenuItems(QMenu* menu) override;
	void closeCgnsFile() override;
	SolverDefinitionGridAttribute* condition() override;
	bool isChildCaptionAvailable(const QString& cap);
	bool colorBarShouldBeVisible() const;

	int mappingCount() const;
	void executeMapping(Grid* grid, WaitDialog* dialog);

	void setDefaultValue(Grid* grid);
	void informValueRangeChange();
	void informDataChange();
	bool getValueRange(double* min, double* max) override;
	void applyColorMapSetting() override;
	void updateZDepthRangeItemCount() override;
	void handleStandardItemChange() override;

	bool importAvailable();
	bool webImportAvailable();

	void clearDimensionsIfNoDataExists();

	const QList<PreProcessorGeoDataDataItemInterface*> geoDatas() const override;
	bool addImportAction(QMenu* menu);
	bool addImportFromWebAction(QMenu* menu);
	QStringList getGeoDatasNotMapped();
	void addCopyPolygon(GeoDataPolygon* polygon) override;
	void addGeoData(PreProcessorGeoDataDataItemInterface *geoData) override;
	GridAttributeDimensionsContainer* dimensions() const override;
	PreProcessorGeoDataDataItemInterface* buildGeoDataDataItem() override;
	GeoDataCreator* getPointMapCreator() override;

	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

	bool polygonExists() const;
	int saveToCgnsFile() override;
	virtual int saveComplexGroupsToCgnsFile();
	void setupStringConverter(GridAttributeStringConverter* converter) override;
	void setupEditWidget(GridAttributeEditWidget* widget) override;
	void updateCrossectionWindows() override;
	void openCrossSectionWindow(GeoDataRiverSurvey* rs, const QString& crosssection) override;
	void toggleCrosssectionWindowsGridCreatingMode(bool gridMode, GeoDataRiverSurvey* rs) override;
	void informCtrlPointUpdateToCrosssectionWindows() override;
	void requestCrosssectionWindowDelete(GeoDataRiverSurveyCrosssectionWindowProjectDataItem* item) override;
	bool addToolBarButtons(QToolBar* toolBar) override;
	QStringList containedFiles() const override;
	void updateVisibility(bool visible) override;
	void setDimensionsToFirst();
	PreProcessorGridTypeDataItem* gridTypeDataItem() const;

	std::vector<GeoDataImporter*> importers() const;
	GeoDataImporter* importer(const std::string& name) const;

	void showPropertyDialog() override;
	QDialog* propertyDialog(QWidget* parent) override;

public slots:
	void importGeoData(QObject* creator);
	void addGeoData(QObject* creator);
	void import();
	void importFromWeb();
	void doExport();
	void exportAllPolygons();

private slots:
	void deleteSelected();
	void deleteAll();
	void cancelImport();
	void editVariationSetting();

signals:
	void selectGeoData(const QModelIndex& current);

protected:
	void importGeoData(GeoDataImporter* importer, const QString& filename, const QString& selectedFilter);

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;
	void setupConnectionToGeoData(GeoData* geodata);

	void addBackground();

	QMenu* m_importMenu;
	QMenu* m_addMenu;
	QAction* m_webImportAction;
	QAction* m_editVariationSettingAction;
	QAction* m_exportAllPolygonsAction;
	QAction* m_deleteSelectedAction;
	QAction* m_deleteAllAction;
	QSignalMapper* m_importSignalMapper;
	QSignalMapper* m_addSignalMapper;
	ColorMapSettingToolBarWidgetController* m_toolBarWidgetController;
	SolverDefinitionGridAttribute* m_condition;

	GridAttributeDimensionsContainer* m_dimensions;

	PreProcessorGeoDataDataItem* m_backgroundItem;

private:
	VariationSetting m_variationSetting;
	QList<GeoDataRiverSurveyCrosssectionWindowProjectDataItem*> m_crosssectionWindows;

	bool m_cancelImport;

	class ColorMapSettingEditDialog;
	class VariationSettingDialog;
};

#endif // PREPROCESSORGEODATAGROUPDATAITEM_H
