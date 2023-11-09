#ifndef PREPROCESSORGEODATADATAITEM_H
#define PREPROCESSORGEODATADATAITEM_H

#include <guicore/pre/base/preprocessorgeodatadataitemi.h>
#include <QSignalMapper>

class ColorMapSettingContainerI;

class QMenu;
class GeoData;
class QSignalMapper;
class QAction;

class PreProcessorGeoDataDataItem : public PreProcessorGeoDataDataItemI
{
	Q_OBJECT

public:
	PreProcessorGeoDataDataItem(PreProcessorDataItem* parent);
	~PreProcessorGeoDataDataItem();

	PreProcessorGeoDataGroupDataItemI* groupDataItem() const override;
	GeoData* geoData() const override;
	void setGeoData(GeoData* geodata) override;

	void addCustomMenuItems(QMenu* menu) override;
	bool addToolBarButtons(QToolBar* toolBar) override;
	void updateMoveUpDownActions(ObjectBrowserView* view) override;
	void handleStandardItemChange() override;
	void handleStandardItemDoubleClicked() override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void viewOperationEnded(VTKGraphicsView* v) override;
	void keyPressEvent(QKeyEvent* event, VTKGraphicsView* v) override;
	void keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v) override;
	void mouseDoubleClickEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	QStringList containedFiles() const override;
	void updateZDepthRangeItemCount() override;
	void informValueRangeChange() override;
	void informDataChange() override;
	bool getValueRange(double* min, double* max);
	void applyColorMapSetting();
	void showPropertyDialog() override;
	QDialog* propertyDialog(QWidget* parent) override;
	void moveUp() override;
	void moveDown() override;
	bool setupExportMenu(QMenu* menu);
	bool isExportAvailable();
	void setDeleteSilently(bool silent) override;
	void removeFile();

	ColorMapSettingContainerI* colorMapSettingContainer() const override;

signals:
	void captionChanged(const QString& newcaption);

public slots:
	void exportGeoData();

protected:
	void assignActorZValues(const ZDepthRange& range) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void innerUpdate2Ds() override;
	void doViewOperationEndedGlobal(VTKGraphicsView* v) override;
	void doApplyOffset(double x, double y) override;

	GeoData* m_geoData;

private:
	/// Action to export current data
	QAction* m_exportAction;
	bool m_deleteSilently;
};

#endif // PREPROCESSORGEODATADATAITEM_H
