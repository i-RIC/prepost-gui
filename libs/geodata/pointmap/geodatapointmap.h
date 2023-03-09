#ifndef GEODATAPOINTMAP_H
#define GEODATAPOINTMAP_H

#include "geodatapointmaprepresentationdialog.h"

#include <guibase/polyline/polylinecontroller.h>
#include <guibase/polygon/polygoncontroller.h>
#include <guicore/pre/geodata/geodata.h>
#include <misc/zdepthrange.h>

#include <vtkUnstructuredGrid.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolygon.h>
#include <vtkActor.h>
#include <vtkLODActor.h>
#include <vtkDataSetMapper.h>
#include <vtkPointLocator.h>
#include <vtkMaskPoints.h>

#include <vector>

namespace geos {
namespace index{
namespace quadtree {
class Quadtree;
} // quadtree
} // index
} // geos

class QAction;
class QPolygonF;

class GeoDataPointmapBreakLine;

/// This class handles the coordinates of points (X, Y), and attribute values
/// defined in at the position of the points.
/**
 * The attibute value type can be a real number, or an integer number.
 * We often use TIN to map attribute values defined at grid nodes,
 * se we use vtkUnstructuredGrid instance to hold the data.
 * vtkUnstructuredGrid can handle so many types of cells, but
 * in this class, all cells in the vtkGrid() are vtkTriangle instances.
 *
 * All Z values of vertices are 0.
 */

#include "gd_pointmap_global.h"

class GD_POINTMAP_EXPORT GeoDataPointmap : public GeoData
{
	Q_OBJECT

private:
	static const char* VALUES;

public:
	enum MappingMode {
		mmTIN,
		mmTemplate
	};
	enum MouseEventMode {
		meNormal,
		meNormalWithShift,
		meSMBox,
		meSMBoxPrepare,
		meSMBoxNotPossible,
		meSMPolygon,
		meSMPolygonPrepare,
		meSMPolygonNotPossible,

		meAddPointSelectReferenceNotPossible,
		meAddPointSelectReference,
		meAddPoint,

		meSMAddPoint,        // depricated
		meSMAddCtrlPoint,    // depricated
		meSMAddPointPrepare, // depricated
		meSMInterpPoint,
		meSMInterpCtrlPoint,
		meSMInterpPointPrepare,
		meSMInterpPointNotPossible,
		meBreakLineAddNotPossible,
		meBreakLineAdd,
		meBreakLineRemoveNotPossible,
		meBreakLineRemove,

		meLongEdgeRemoveDialog,
	};
	GeoDataPointmap(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* att);
	virtual ~GeoDataPointmap();

	vtkPolyData* vtkGrid() const;
	vtkPolyData* delaunayedPolyData() const;

	void setPoints(vtkPoints* points, vtkDataArray* values);
	void setSTLData(vtkPolyData* data, vtkDataArray* values);

	bool getValueAt(double x, double y, double* value);
	bool getValueAt(const QPointF& pos, double* value);

	/// Execute the delaunay division.
	bool doDelaunay(bool allowCancel = false);
	void setupActors() override;
	void setupMenu() override;
	bool addToolBarButtons(QToolBar* parent) override;
	void applyColorMapSetting() override;
	void updateZDepthRangeItemCount(ZDepthRange& range) override;
	void assignActorZValues(const ZDepthRange& range) override;

	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

	void addCustomMenuItems(QMenu* menu) override;
	void keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void keyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mouseDoubleClickEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;

	vtkPolygon* getVtkInterpPolygon() const;
	vtkDoubleArray* getVtkInterpValue() const;
	vtkPolyData* selectedVerticesGrid() const;

	QVector<vtkIdType> selectedVertices();
	void definePolygon(bool doubleClick, bool xOr);
	void selectPointsInsidePolygon(bool xOr);
	void selectPointsInsideBox(MouseBoundingBox* box, bool xOr);
	void selectPointsNearPoint(const QVector2D& pos, bool xOr);
	void clearPointsSelection();
	void finishAddPoint();
	void finishInterpPoint();
	bool needRemeshing() {return m_needRemeshing;}
	vtkCell* findCell(double x, double y, double* weights);

public slots:
	void remeshTINS(bool nodialog = false);

private:
	void updateActorSettings();
	void updateRepresentation();

	bool checkBreakLines();
	void updateBreakLinesOnDelete(QVector<vtkIdType>& deletedPoints);
	void updateBreakLinesOnInsert(QVector<vtkIdType>& deletedPoints);
	bool pointsUsedForBreakLines(const QVector<vtkIdType>& points);
	iRICLib::H5CgnsGeographicDataGroup::Type iRICLibType() const override;

	static void TSplineSTL(std::vector<double>& x, std::vector<double>& y, int n,
										std::vector<double>& xout, std::vector<double>& yout, int iout, float sigma,
										std::vector<double>& yp, std::vector<double>& temp);

	MappingMode m_mappingMode;
	QPoint m_dragStartPoint;
	QPoint m_currentPoint;
	QPoint m_mouseMovePoint;

	MouseEventMode m_mouseEventMode;
	void buildGridFromPolydata();
	void updateActionStatus();
	void updateMouseEventMode();
	bool isVertexSelectable(const QPointF& pos);
	int m_selectedVertexId;
	double m_selectedZPos;
	bool m_canceled;

private slots:
	void showDisplaySetting();
	void selectionModePoint(bool on);
	void selectionModePolygon(bool on);
	void interpolatePoints(bool on);
	void addPoints(bool on);

	void addBreakLine();
	void removeBreakLine();
	void removeAllBreakLines();
	void removeTrianglesWithLongEdgeStart();
	void removeTrianglesWithLongEdgeEnd();

	void editPoints();
	void editPointsDelete();
	void editPointsExport();
	void editPointsLessThan();
	void editPointsGreaterThan();
	void cancel() {m_canceled = true;}

	void mergePointmaps();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void updateFilename() override;
	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;
	void updateMouseCursor(PreProcessorGraphicsViewInterface* v);
	void doApplyOffset(double x, double y) override;

	void rebuildQTree();

	vtkSmartPointer<vtkPolyData> m_vtkDelaunayedPolyData;
	geos::index::quadtree::Quadtree* m_qTree;

	QList<GeoDataPointmapBreakLine*> m_breakLines;
	GeoDataPointmapBreakLine* m_activeBreakLine;


	vtkSmartPointer<vtkPointLocator> m_vtkPointLocator;

	int m_opacityPercent;
	bool m_hideBreakLines;
	int m_pointSize;
	GeoDataPointmapRepresentationDialog::Representation m_representation;

	vtkSmartPointer<vtkPolyData> m_selectedVerticesGrid;
	vtkSmartPointer<vtkPolyDataMapper> m_selectedMapper;
	vtkSmartPointer<vtkActor> m_selectedActor;

	PolyLineController m_interpolatePointsController;
	double m_addPointsValue;
	std::vector<double> m_interpolateValues;
	std::vector<bool> m_interpolateNewFlags;

	PolygonController m_selectionPolygonController;

	bool lastInterpPointKnown;
	bool doubleclick;
	/// When Points are added/deleted after the last remeshing, this flag is true.
	bool m_needRemeshing;
	ZDepthRange m_zDepthRange;

private:
	class InterpolateLineAddPointCommand;
	class AddPointsCommand;
	class DeletePointsCommand;
	class EditPointsCommand;
	class EditSinglePointCommand;
	class BreakLineAddCommand;
	class BreakLineAddPointCommand;
	class BreakLineFinishDefinitionCommand;
	class BreakLineCancelDefinitionCommand;
	class RemoveTrianglesCommand;

	class TrianglesWithLongEdgeRemover;
	TrianglesWithLongEdgeRemover* m_longEdgeRemover;

	class PointsManager;

protected:
	class Impl;
	Impl* impl;

public:
	friend class GeoDataPointmapBreakLine;
};

#endif // GEODATAPOINTMAP_H
