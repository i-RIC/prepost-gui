#ifndef PREPROCESSORRAWDATAGROUPDATAITEM_H
#define PREPROCESSORRAWDATAGROUPDATAITEM_H

#include <guicore/pre/base/preprocessorrawdatagroupdataiteminterface.h>
#include <guibase/scalarbarsetting.h>
#include "../misc/preprocessorwindowscalarbarsettingwidget.h"

#include <QSignalMapper>

class SolverDefinitionGridRelatedCondition;
class PreProcessorRawdataDataItem;
class PreProcessorScalarBarLegendBoxSettingDialog;
class GridRelatedConditionEditWidget;
class Grid;
class QMenu;
class QModelIndex;
class RawDataPolygon;
class RawDataRiverSurvey;
class RawDataRiverSurveyCrosssectionWindowProjectDataItem;
class WaitDialog;

class PreProcessorRawDataGroupDataItem : public PreProcessorRawDataGroupDataItemInterface
{
	Q_OBJECT
public:
	/// Constructor
	PreProcessorRawDataGroupDataItem(SolverDefinitionGridRelatedCondition* cond, PreProcessorDataItem* parent);
	virtual ~PreProcessorRawDataGroupDataItem();
	void addCustomMenuItems(QMenu* menu) override;
	void closeCgnsFile() override {}
	SolverDefinitionGridRelatedCondition* condition() override {return m_condition;}
	bool isChildCaptionAvailable(const QString& cap);
	int mappingCount() const;
	void executeMapping(Grid* grid, WaitDialog* dialog);
	void setDefaultValue(Grid* grid);
	void informValueRangeChange();
	void informDataChange();
	bool getValueRange(double* min, double* max) override;
	void updateZDepthRangeItemCount() override {m_zDepthRange.setItemCount(10);}
	bool importAvailable();
	const QList<PreProcessorRawdataDataItemInterface*> rawDatas() const override;
	void editScalarBarLegendBox(PreProcessorScalarBarLegendBoxSettingDialog* dialog);
	ScalarBarSetting& scalarBarSetting() {return m_scalarBarSetting;}
	QString title() {return m_title;}
	QAction* importAction() {return m_importAction;}
	void addImportAction(QMenu* menu);
	QStringList getRawDatasNotMapped();
	void addCopyPolygon(RawDataPolygon* polygon) override;
	GridRelatedConditionDimensionsContainer* dimensions() const override {return m_dimensions;}

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
	virtual void setupEditWidget(GridRelatedConditionEditWidget* /*widget*/) override {}
	void updateCrossectionWindows() override;
	void openCrossSectionWindow(RawDataRiverSurvey* rs, double crosssection) override;
	void toggleCrosssectionWindowsGridCreatingMode(bool gridMode, RawDataRiverSurvey* rs) override;
	void informCtrlPointUpdateToCrosssectionWindows() override;
	void requestCrosssectionWindowDelete(RawDataRiverSurveyCrosssectionWindowProjectDataItem* item) override;
	bool addToolBarButtons(QToolBar* /*parent*/) override;
	QStringList containedFiles() override;
	void setDimensionsToFirst();

public slots:
	void addRawData(QObject* creator);
	virtual void editScalarsToColors();
	void import();
	void doExport();
	void exportAllPolygons();
	void deleteAll();

private slots:
	void cancelImport() {m_cancelImport = true;}

signals:
	void selectRawData(const QModelIndex& current);

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;
	void setupConnectionToRawData(RawData* rawdata);

	void addBackground();

	QMenu* m_addMenu;
	QAction* m_importAction;
	QAction* m_editColorMapAction;
	QAction* m_setupScalarBarAction;
	QAction* m_exportAllPolygonsAction;
	QAction* m_deleteAllAction;
	QSignalMapper* m_addSignalMapper;
	SolverDefinitionGridRelatedCondition* m_condition;
	GridRelatedConditionDimensionsContainer* m_dimensions;

	ScalarBarSetting m_scalarBarSetting;

	QString m_title;
	PreProcessorRawdataDataItem* m_backgroundItem;

private:
	void moveBackgroundToLast();
	QList<RawDataRiverSurveyCrosssectionWindowProjectDataItem*> m_crosssectionWindows;

	bool m_cancelImport;
};

#endif // PREPROCESSORRAWDATAGROUPDATAITEM_H
