#ifndef PREPROCESSORGEODATADATAITEM_H
#define PREPROCESSORGEODATADATAITEM_H

#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <QSignalMapper>

class QMenu;
class GeoData;
class QSignalMapper;
class QAction;

class PreProcessorGeoDataDataItem : public PreProcessorGeoDataDataItemInterface
{
	Q_OBJECT

public:
	/// Constructor
	PreProcessorGeoDataDataItem(PreProcessorDataItem* parent);
	~PreProcessorGeoDataDataItem();

	GeoData* geoData() const override {return m_geoData;}
	void setGeoData(GeoData* geodata);

	int index() const {return m_index;}
	void setIndex(int index) {m_index = index;}
	void addCustomMenuItems(QMenu* menu) override;
	bool addToolBarButtons(QToolBar* /*parent*/) override;
	void updateMoveUpDownActions(ObjectBrowserView* view) override;
	void handleStandardItemChange() override;
	void handleStandardItemClicked() override;
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
	virtual QStringList containedFiles() override;
	void updateZDepthRangeItemCount() override;
	void informValueRangeChange() override;
	void informDataChange() override;
	bool getValueRange(double* min, double* max);
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
	void moveUp() override;
	void moveDown() override;
	bool setupExportMenu(QMenu* menu);
	bool isExportAvailable();
	void saveToCgnsFile(const int fn) override;
	void setDeleteSilently(bool silent);
	void removeFile();

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
	int m_index;
	bool m_deleteSilently;
};

#endif // PREPROCESSORGEODATADATAITEM_H
