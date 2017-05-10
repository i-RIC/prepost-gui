#ifndef PREPROCESSORGEODATAGROUPDATAITEM_H
#define PREPROCESSORGEODATAGROUPDATAITEM_H

#include "../pre_global.h"
#include "../misc/preprocessorwindowscalarbarsettingwidget.h"

#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guibase/scalarbarsetting.h>

#include <QSignalMapper>

class SolverDefinitionGridAttribute;
class PreProcessorGeoDataDataItem;
class PreProcessorScalarBarLegendBoxSettingDialog;
class GridAttributeEditWidget;
class Grid;
class QMenu;
class QModelIndex;
class GeoDataImporter;
class GeoDataPolygon;
class GeoDataRiverSurvey;
class GeoDataRiverSurveyCrosssectionWindowProjectDataItem;
class WaitDialog;

class PREDLL_EXPORT PreProcessorGeoDataGroupDataItem : public PreProcessorGeoDataGroupDataItemInterface
{
	Q_OBJECT

public:
	PreProcessorGeoDataGroupDataItem(SolverDefinitionGridAttribute* cond, PreProcessorDataItem* parent);
	virtual ~PreProcessorGeoDataGroupDataItem();
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

	const QList<PreProcessorGeoDataDataItemInterface*> geoDatas() const override;
	void editScalarBarLegendBox(PreProcessorScalarBarLegendBoxSettingDialog* dialog);
	ScalarBarSetting& scalarBarSetting();
	const QString& title() const;
	QAction* importAction() const;
	bool addImportAction(QMenu* menu);
	bool addImportFromWebAction(QMenu* menu);
	QStringList getGeoDatasNotMapped();
	void addCopyPolygon(GeoDataPolygon* polygon) override;
	GridAttributeDimensionsContainer* dimensions() const override;
	PreProcessorGeoDataDataItemInterface* buildGeoDataDataItem() override;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void keyPressEvent(QKeyEvent* event, VTKGraphicsView* v) override;
	void keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v) override;
	void mouseDoubleClickEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	bool polygonExists() const;
	virtual void saveToCgnsFile(const int fn) override;
	virtual void saveComplexGroupsToCgnsFile(const int /*fn*/) {}
	virtual void setupEditWidget(GridAttributeEditWidget* /*widget*/) override {}
	void updateCrossectionWindows() override;
	void openCrossSectionWindow(GeoDataRiverSurvey* rs, const QString& crosssection) override;
	void toggleCrosssectionWindowsGridCreatingMode(bool gridMode, GeoDataRiverSurvey* rs) override;
	void informCtrlPointUpdateToCrosssectionWindows() override;
	void requestCrosssectionWindowDelete(GeoDataRiverSurveyCrosssectionWindowProjectDataItem* item) override;
	bool addToolBarButtons(QToolBar* /*parent*/) override;
	QStringList containedFiles() override;
	void setDimensionsToFirst();

	void addGeoData(PreProcessorGeoDataDataItemInterface *geoData);
	std::vector<GeoDataImporter*> importers() const;
	GeoDataImporter* importer(const std::string& name) const;

public slots:
	void addGeoData(QObject* creator);
	virtual void editScalarsToColors();
	void import();
	void importFromWeb();
	void doExport();
	void exportAllPolygons();
	void deleteAll();

private slots:
	void cancelImport() {m_cancelImport = true;}

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
	QAction* m_deleteAllAction;
	QSignalMapper* m_addSignalMapper;
	SolverDefinitionGridAttribute* m_condition;
	GridAttributeDimensionsContainer* m_dimensions;

	ScalarBarSetting m_scalarBarSetting;

	QString m_title;
	PreProcessorGeoDataDataItem* m_backgroundItem;

private:
	QList<GeoDataRiverSurveyCrosssectionWindowProjectDataItem*> m_crosssectionWindows;

	bool m_cancelImport;
};

#endif // PREPROCESSORGEODATAGROUPDATAITEM_H
