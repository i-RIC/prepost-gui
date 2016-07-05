#ifndef GEODATAPOLYGON_H
#define GEODATAPOLYGON_H

#include "gd_polygon_global.h"
#include <guicore/pre/geodata/geodata.h>
#include "geodatapolygoncolorsettingdialog.h"

#include <guicore/pre/grid/unstructured2dgrid.h>
#include <misc/zdepthrange.h>

#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <QVector>
#include <QPoint>
#include <QPointF>
#include <QPolygonF>
#include <QPixmap>
#include <QCursor>
#include <QVariant>
#include <QList>

#include <iriclib.h>

#include <vector>

class QMenu;
class QAction;
class QToolBar;

class GridCreatingConditionCreatorTriangle;
class GeoDataPolygonAbstractPolygon;
class GeoDataPolygonRegionPolygon;
class GeoDataPolygonHolePolygon;
class GeoDataPolygonImporter;
class GeoDataPolygonShapeExporter;
class GeoDataPolygonTriangleThread;
class GeoDataPolygonProxy;
class PreProcessorBCSettingDataItem;

class GD_POLYGON_EXPORT GeoDataPolygon : public GeoData
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

	GeoDataPolygon(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition);
	virtual ~GeoDataPolygon();

	void setupMenu() override;
	bool addToolBarButtons(QToolBar* parent) override;
	void informSelection(PreProcessorGraphicsViewInterface* v) override;
	void informDeselection(PreProcessorGraphicsViewInterface* v) override;
	void addCustomMenuItems(QMenu* menu) override;
	void viewOperationEnded(PreProcessorGraphicsViewInterface* v) override;
	void keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void keyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mouseDoubleClickEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void updateZDepthRangeItemCount(ZDepthRange& range) override;
	void assignActorZValues(const ZDepthRange& range) override;
	void definePolygon(bool doubleClick, bool noEditVal = false);
	QColor color() const;

	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* d) override;

	QColor doubleToColor(double d);
	void clear();
	bool ready() const {return true;}
	void showInitialDialog() override;

	const QVariant& variantValue() const;
	void setVariantValue(const QVariant& v, bool disableInform = false);

	void addHolePolygon(const QPolygonF& p);
	vtkPolyData* polyData() const;
	bool getValueRange(double* min, double* max) override;
	void updateFilename() override;
	GeoDataProxy* getProxy() override;
	void copyShape(GeoDataPolygon* polygon);
	MouseEventMode mouseEventMode() const {return m_mouseEventMode;}
	SelectMode selectMode() const {return m_selectMode;}
	void setBCSettingMode(bool mode) {
		m_bcSettingMode = mode;
	}

public slots:
	void editValue();
	void restoreMouseEventMode();
	void handleDimensionCurrentIndexChange(int oldIndex, int newIndex) override;
	void handleDimensionValuesChange(const std::vector<QVariant>& before, const std::vector<QVariant>& after) override;

private slots:
	void updatePolyData(bool noDraw = false);
	void addVertexMode(bool on);
	void removeVertexMode(bool on);
	void editCoordinates();
	void addHolePolygon();
	void deletePolygon(bool force = false);
	void editColorSetting();
	void renderGraphics();
	void updatePolygon(GeoDataPolygon* polygon, vtkPoints* points, vtkCellArray* ca);
	void copy();

protected:
	void updateMouseCursor(PreProcessorGraphicsViewInterface* v);
	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;
	void setMapping(GeoDataPolygonColorSettingDialog::Mapping m);
	void setOpacity(int opacity);
	void setColor(const QColor& color);
	int iRICLibType() const override {return IRIC_GEO_POLYGON;}
	void doApplyOffset(double x, double y) override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	const QPolygonF polygon() const;
	void setPolygon(const QPolygonF& p);

	bool checkCondition();
	void updateScalarValues();
	void updateActorSettings();
	bool selectObject(QPoint point);
	void deselectAll();
	bool activePolygonHasFourVertices();
	void initParams();
	void updateMouseEventMode();
	void updateActionStatus();
	GeoDataPolygonHolePolygon* setupHolePolygon();
	void applyOffsetToAbstractPolygon(GeoDataPolygonAbstractPolygon* polygon, double x, double y);
	void clearHolePolygons();

	void lockMutex();
	void unlockMutex();

	ZDepthRange m_depthRange;

	QPoint m_dragStartPoint;
	QPoint m_currentPoint;

	SelectMode m_selectMode;
	MouseEventMode m_mouseEventMode;

	GeoDataPolygonRegionPolygon* m_gridRegionPolygon;
	QList<GeoDataPolygonHolePolygon*> m_holePolygons;

	GeoDataPolygonAbstractPolygon* m_selectedPolygon;

	QAction* m_holeModeAction;
	QAction* m_deleteAction;

	QAction* m_editValueAction;
	QAction* m_copyAction;
	QAction* m_addVertexAction;
	QAction* m_removeVertexAction;
	QAction* m_coordEditAction;
	QAction* m_editColorSettingAction;
	QMenu* m_rightClickingMenu;

	GeoDataPolygonColorSettingDialog::Setting m_setting;

	std::vector<QVariant> m_variantValues;

	vtkSmartPointer<vtkPolyData> m_polyData;
	vtkSmartPointer<vtkActor> m_paintActor;
	vtkSmartPointer<vtkPolyDataMapper> m_paintMapper;
	vtkSmartPointer<vtkDoubleArray> m_scalarValues;

	bool m_inhibitSelect {false};
	bool m_shapeUpdating {false};
	QPixmap m_addPixmap;
	QPixmap m_removePixmap;
	QPixmap m_movePointPixmap;
	QCursor m_addCursor;
	QCursor m_removeCursor;
	QCursor m_movePointCursor;

	GeoDataPolygonTriangleThread* m_triangleThread;

	bool m_bcSettingMode {false};

private:
	class AddHolePolygonCommand;
	class AddVertexCommand;
	class PushNewPointCommand;
	class EditCoordinatesCommand;
	class EditPropertyCommand;
	class EditValueCommand;
	class FinishPolygonDefiningCommand;
	class ModifyAbstractPolygonCommand;
	class MovePolygonCommand;
	class MoveVertexCommand;
	class RemoveVertexCommand;

public:
	friend class GeoDataPolygonCreator;
	friend class GeoDataPolygonCoordinatesEditDialog;
	friend class GeoDataPolygonAbstractPolygon;
	friend class GeoDataPolygonTriangleThread;
	friend class GeoDataPolygonImporter;
	friend class GeoDataPolygonShapeExporter;
	friend class GeoDataPolygonProxy;
	friend class PreProcessorBCSettingDataItem;
};

#endif // GEODATAPOLYGON_H
