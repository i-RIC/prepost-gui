#ifndef PREPROCESSORRAWDATADATAITEM_H
#define PREPROCESSORRAWDATADATAITEM_H

#include <guicore/pre/base/preprocessorrawdatadataiteminterface.h>
#include <QSignalMapper>

class QMenu;
class RawData;
class QSignalMapper;
class QAction;

class PreProcessorRawdataDataItem : public PreProcessorRawdataDataItemInterface
{
	Q_OBJECT
public:
	/// Constructor
	PreProcessorRawdataDataItem(PreProcessorDataItem* parent);
	~PreProcessorRawdataDataItem();
	void setRawData(RawData* rawdata);
	RawData* rawData() {return m_rawData;}
	int index() const {return m_index;}
	void setIndex(int index) {m_index = index;}
	void addCustomMenuItems(QMenu* menu) override;
	bool addToolBarButtons(QToolBar* /*parent*/) override;
	void updateMoveUpDownActions(ObjectBrowserView* view);
	void handleStandardItemChange();
	void handleStandardItemClicked();
	void handleStandardItemDoubleClicked();
	void informSelection(VTKGraphicsView* v);
	void informDeselection(VTKGraphicsView* v);
	void viewOperationEnded(VTKGraphicsView* v);
	void keyPressEvent(QKeyEvent* event, VTKGraphicsView* v);
	void keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v);
	void mouseDoubleClickEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);
	virtual QStringList containedFiles();
	void updateZDepthRangeItemCount();
	void informValueRangeChange();
	void informDataChange();
	bool getValueRange(double* min, double* max);
	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* propDialog);
	void moveUp() {
		PreProcessorDataItem::moveUp();
		informDataChange();
	}
	void moveDown() {
		PreProcessorDataItem::moveDown();
		informDataChange();
	}
	bool setupExportMenu(QMenu* menu);
	bool isExportAvailable();
	void saveToCgnsFile(const int fn);

signals:
	void captionChanged(const QString& newcaption);

public slots:
	void exportRawdata();
protected:
	void assignActorZValues(const ZDepthRange& range) override;
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void innerUpdate2Ds();
	void doViewOperationEndedGlobal(VTKGraphicsView* v);
	void doApplyOffset(double x, double y);

	RawData* m_rawData;

private:
	/// Action to export current data
	QAction* m_exportAction;
	int m_index;
};

#endif // PREPROCESSORRAWDATADATAITEM_H
