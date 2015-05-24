#ifndef PREPROCESSORBCSETTINGDATAITEM_H
#define PREPROCESSORBCSETTINGDATAITEM_H

#include <guicore/pre/base/preprocessordataitem.h>
#include <rawdata/polygon/rawdatapolygon.h>
#include <guicore/pre/base/preprocessorrawdatadataiteminterface.h>

#include <QString>
#include <QColor>
#include <QSet>
#include <QPoint>

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkActor2D.h>

class BoundaryConditionDialog;
class SolverDefinition;
class SolverDefinitionBoundaryCondition;
class PreProcessorBCDataItem;
class QAction;
class VTKGraphicsView;
class Grid;
class WaitDialog;

class PreProcessorBCSettingDataItem : public PreProcessorRawdataDataItemInterface
{
	Q_OBJECT
public:
	/// Constructor
	PreProcessorBCSettingDataItem(PreProcessorBCDataItem* item, GraphicsWindowDataItem* parent);
	virtual ~PreProcessorBCSettingDataItem();
	void loadFromCgnsFile(const int fn);
	void saveToCgnsFile(const int fn);
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void handleStandardItemDoubleClicked();
	void handleStandardItemChange();

	void informSelection(VTKGraphicsView* v);
	void informDeselection(VTKGraphicsView* v);
	void viewOperationEnded(VTKGraphicsView* v);
	void mouseDoubleClickEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);
	void keyPressEvent(QKeyEvent*, VTKGraphicsView*);
	void keyReleaseEvent(QKeyEvent*, VTKGraphicsView*);
	void loadData();
	void addCustomMenuItems(QMenu* menu) override;
	PreProcessorBCDataItem* bcDataItem() const {return m_bcDataItem;}
	bool isMapped() const {return m_polygon->isMapped();}
	RawData* rawData() {return m_polygon;}
	void informValueRangeChange() {}
	void informDataChange() {}

	QAction* editAction() const {return m_editAction;}
	QAction* deleteAction() {return PreProcessorDataItem::deleteAction();}

public slots:
	void executeMapping(bool noDraw, WaitDialog* dialog);

private slots:
	void setModified() {
		PreProcessorDataItem::setModified();
	}
	void updateItem();

protected:
	void assignActorZValues(const ZDepthRange& range) override;
	void loadExternalData(const QString& filename);
	void saveExternalData(const QString& filename);
	void doApplyOffset(double x, double y);
private:
	void setupActors();
	void updateFilename();

	PreProcessorBCDataItem* m_bcDataItem;
	QAction* m_editAction;
	RawDataPolygon* m_polygon;
	QPoint m_dragStartPoint;
	QMenu* m_rightClickingMenu;

	BoundaryConditionDialog* m_dialog;
};

#endif // PREPROCESSORBCSETTINGDATAITEM_H
