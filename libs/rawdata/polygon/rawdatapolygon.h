#ifndef RAWDATAPOLYGON_H
#define RAWDATAPOLYGON_H

#include "rd_polygon_global.h"
#include <guicore/pre/rawdata/rawdata.h>
#include "rawdatapolygoncolorsettingdialog.h"

#include <guicore/pre/grid/unstructured2dgrid.h>
#include <misc/zdepthrange.h>
#include <vtkSmartPointer.h>
#include <vtkPolygon.h>
#include <vtkUnstructuredGrid.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <QVector>
#include <QPoint>
#include <QPointF>
#include <QPolygonF>
#include <QPixmap>
#include <QCursor>
#include <QVariant>
#include <QList>

#include <iriclib.h>

class QMenu;
class QAction;
class QToolBar;

class GridCreatingConditionCreatorTriangle;
class RawDataPolygonTriangleThread;
class RawDataPolygonAbstractPolygon;
class RawDataPolygonRegionPolygon;
class RawDataPolygonHolePolygon;
class RawDataPolygonPolygonFinishDefiningCommand;
class RawDataPolygonPolygonDefineNewPointCommand;
class RawDataPolygonPolygonMoveCommand;
class RawDataPolygonPolygonMoveVertexCommand;
class RawDataPolygonPolygonRemoveVertexCommand;
class RawDataPolygonPolygonAddVertexCommand;
class RawDataPolygonPolygonCoordinatesEditCommand;
class RawDataPolygonImporter;
class RawDataPolygonShapeExporter;
class RawDataPolygonProxy;
class PreProcessorBCSettingDataItem;

class RawDataPolygonAddHolePolygonCommand;
class RawDataPolygonEditValueCommand;

class RawDataPolygonTriangleThread;

class RD_POLYGON_EXPORT RawDataPolygon : public RawData
{
	Q_OBJECT
private:
	const static int normalEdgeWidth = 1;
	const static int selectedEdgeWidth = 2;
public:
	enum SelectMode {
		smNone,
		smPolygon
	};
	enum MouseEventMode {
		meNormal,
		meBeforeDefining,
		meDefining,
		meTranslate,
		meTranslatePrepare,
		meMoveVertex,
		meMoveVertexPrepare,
		meAddVertex,
		meAddVertexPrepare,
		meAddVertexNotPossible,
		meRemoveVertexPrepare,
		meRemoveVertexNotPossible,

		meTranslateDialog,
		meEditVerticesDialog
	};
	/// Constructor
	RawDataPolygon(ProjectDataItem* d, RawDataCreator* creator, SolverDefinitionGridRelatedCondition* condition);
	virtual ~RawDataPolygon();
	void setupMenu();
	bool addToolBarButtons(QToolBar* /*parent*/);
	void informSelection(PreProcessorGraphicsViewInterface* v);
	void informDeselection(PreProcessorGraphicsViewInterface* v);
	void addCustomMenuItems(QMenu* menu);
	void viewOperationEnded(PreProcessorGraphicsViewInterface* /*v*/);
	void keyPressEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/);
	void keyReleaseEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/);
	void mouseDoubleClickEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/);
	void mouseMoveEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/);
	void mousePressEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/);
	void mouseReleaseEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/);
	void updateZDepthRangeItemCount(ZDepthRange& range);
	void assignActionZValues(const ZDepthRange& range);
	void definePolygon(bool doubleClick, bool noEditVal = false);
	QColor color(){return m_color;}
	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* d);
	void setMapping(RawDataPolygonColorSettingDialog::Mapping m);
	RawDataPolygonColorSettingDialog::Mapping mapping(){return m_mapping;}
	QColor doubleToColor(double d);
	void clear();
	bool ready() const {return true;}
	bool create(QWidget* parent);
	void showInitialDialog();
	const QVariant& variantValue() const;
	void setVariantValue(const QVariant& v);
	void setPolygon(const QPolygonF& p);
	void addHolePolygon(const QPolygonF& p);
	const QPolygonF polygon() const;
	vtkUnstructuredGrid* grid(){return m_grid;}
	bool getValueRange(double* min, double* max);
	void updateFilename(){
		m_filename = m_name;
		m_filename.append(".dat");
	}
	RawDataProxy* getProxy();
	void copyShape(RawDataPolygon* polygon);
	MouseEventMode mouseEventMode() const {return m_mouseEventMode;}
	SelectMode selectMode() const {return m_selectMode;}
	void setBCSettingMode(bool mode){
		m_bcSettingMode = mode;
	}

public slots:
	void editValue();
	void restoreMouseEventMode();
	void handleDimensionCurrentIndexChange(int oldIndex, int newIndex);
	void handleDimensionValuesChange(const QList<QVariant>& before, const QList<QVariant>& after);

private slots:
	void updateGrid(bool noDraw = false);
	void addVertexMode(bool on);
	void removeVertexMode(bool on);
	void editCoordinates();
	void addHolePolygon();
	void deletePolygon(bool force = false);
	void editColorSetting();
	void renderGraphics();
	void copy();

protected:
	void updateMouseCursor(PreProcessorGraphicsViewInterface* v);
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void loadExternalData(const QString& filename);
	void saveExternalData(const QString& filename);
	void setColor(const QColor& color);
	void setColor(double r, double g, double b);
	int iRICLibType() const {return IRIC_GEO_POLYGON;}
	void doApplyOffset(double x, double y);

private:
	bool checkCondition();
	void updateScalarValues();
	bool selectObject(QPoint point);
	void deselectAll();
	bool activePolygonHasFourVertices();
	void initParams();
	void updateMouseEventMode();
	void updateActionStatus();
	void setupHolePolygon(RawDataPolygonHolePolygon* p);
	void applyOffsetToAbstractPolygon(RawDataPolygonAbstractPolygon* polygon, double x, double y);
	ZDepthRange m_depthRange;

	QPoint m_dragStartPoint;
	QPoint m_currentPoint;

	SelectMode m_selectMode;
	MouseEventMode m_mouseEventMode;

	RawDataPolygonRegionPolygon* m_gridRegionPolygon;
	QList<RawDataPolygonHolePolygon*> m_holePolygons;

	RawDataPolygonAbstractPolygon* m_selectedPolygon;

	QAction* m_holeModeAction;
	QAction* m_deleteAction;

	QAction* m_editValueAction;
	QAction* m_copyAction;
	QAction* m_addVertexAction;
	QAction* m_removeVertexAction;
	QAction* m_coordEditAction;
	QAction* m_editColorSettingAction;
	QMenu* m_rightClickingMenu;
	QColor m_color;
	int m_opacityPercent;
	RawDataPolygonColorSettingDialog::Mapping m_mapping;
	QList<QVariant> m_variantValues;

	vtkSmartPointer<vtkUnstructuredGrid> m_grid;
	vtkSmartPointer<vtkActor> m_paintActor;
	vtkSmartPointer<vtkDataSetMapper> m_paintMapper;
	vtkSmartPointer<vtkDoubleArray> m_scalarValues;

	bool m_inhibitSelect;
	bool m_shapeUpdating;
	QPixmap m_addPixmap;
	QPixmap m_removePixmap;
	QPixmap m_movePointPixmap;
	QCursor m_addCursor;
	QCursor m_removeCursor;
	QCursor m_movePointCursor;

	RawDataPolygonTriangleThread* m_triangleThread;

	double lastX;
	double lastY;

	bool m_bcSettingMode;
public:
	friend class RawDataPolygonCreator;
	friend class RawDataPolygonPropertyEditCommand;

	friend class RawDataPolygonPolygonFinishDefiningCommand;
	friend class RawDataPolygonPolygonDefineNewPointCommand;
	friend class RawDataPolygonPolygonMoveCommand;
	friend class RawDataPolygonPolygonMoveVertexCommand;
	friend class RawDataPolygonPolygonRemoveVertexCommand;
	friend class RawDataPolygonPolygonAddVertexCommand;
	friend class RawDataPolygonCoordinatesEditCommand;
	friend class RawDataPolygonAddHolePolygonCommand;
	friend class RawDataPolygonEditValueCommand;

	friend class RawDataPolygonCoordinatesEditDialog;
	friend class RawDataPolygonAbstractPolygon;

	friend class RawDataPolygonTriangleThread;
	friend class RawDataPolygonImporter;
	friend class RawDataPolygonShapeExporter;

	friend class RawDataPolygonProxy;
	friend class PreProcessorBCSettingDataItem;
};

#endif // RAWDATAPOLYGON_H
