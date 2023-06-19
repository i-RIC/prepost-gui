#ifndef GEODATAPOINTMAP_H
#define GEODATAPOINTMAP_H

#include <guibase/polyline/polylinecontroller.h>
#include <guicore/pre/geodata/geodata.h>
#include <misc/zdepthrange.h>

#include <vtkUnstructuredGrid.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolygon.h>
#include <vtkActor.h>
#include <vtkLODActor.h>
#include <vtkDataSetMapper.h>
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
	enum MouseEventMode {
		meNormal,
		meNormalWithShift,

		meLongEdgeRemoveDialog,
	};

	GeoDataPointmap(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* att);
	~GeoDataPointmap();

	bool getValueRange(double* min, double* max) override;
	GeoDataMapper* mapper() const override;

	vtkPolyData* points() const;
	vtkDoubleArray* pointsValues() const;

	vtkPolyData* tin() const;
	vtkDoubleArray* tinValues() const;

	void setPoints(vtkPoints* points, vtkDoubleArray* values);
	void setTin(vtkPolyData* data, vtkDoubleArray* values);

	bool getTinValueAt(double x, double y, double* value);
	bool getTinValueAt(const QPointF& pos, double* value);
	bool mapWithPolygons(double x, double y, double* value);

	bool needRebuildTin() const;
	void setNeedRebuildTin(bool needed = true);

	void rebuildTinFromPointsIfNeeded();
	bool rebuildTinFromPoints(bool allowCancel);
	void setupActors() override;
	void setupMenu() override;
	bool addToolBarButtons(QToolBar* parent) override;
	void applyColorMapSetting() override;
	void updateZDepthRangeItemCount(ZDepthRange& range) override;
	void assignActorZValues(const ZDepthRange& range) override;

	void showPropertyDialog() override;
	QDialog* propertyDialog(QWidget* parent) override;

	void addCustomMenuItems(QMenu* menu) override;
	void informDeselection(PreProcessorGraphicsViewInterface* v) override;
	void keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void keyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mouseDoubleClickEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	QStringList containedFiles() const override;

	QVector<vtkIdType> selectedVertices();
	vtkCell* findTinCell(double x, double y, double* weights);
	GeoDataProxy* getProxy() override;

	class DisplaySettingWidget;

public slots:
	void remeshTin(bool nodialog = false);

private slots:
	void openDisplaySettingDialog();
	void openMappingSettingDialog();

	void removeTrianglesWithLongEdgeStart();
	void removeTrianglesWithLongEdgeEnd();

	void mergePointmaps();
	void togglePointsEditMode(bool on);
	void toggleTinEditMode(bool on);
	void togglePolyonsEditMode(bool on);

private:
	void updateActorSetting() override;
	void updateMenu();
	void updateMenu(QMenu* menu);

	void buildPointsFromTIN();

	iRICLib::H5CgnsGeographicDataGroup::Type iRICLibType() const override;

	void pushModifyCommand(QUndoCommand* command);

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void updateFilename() override;
	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;
	void doApplyOffset(double x, double y) override;

	QPoint m_dragStartPoint;
	ZDepthRange m_zDepthRange;

	GeoDataMapper* m_templateMapper;
	MouseEventMode m_mouseEventMode;

	class DisplaySetting;
	class MappingSetting;
	class MappingSettingDialog;
	class PropertyDialog;

	class ModifyCommand;
	class UpdateActorSettingsCommand;

	// class TrianglesWithLongEdgeRemover;
	// TrianglesWithLongEdgeRemover* m_longEdgeRemover;

	class PointsManager;
	class PolygonsManager;
	class TINManager;

	class TinNodeMapper;
	class TinCellMapper;
	class TemplateNodeMapper;
	class TemplateCellMapper;
	class PolygonsNodeMapper;
	class PolygonsCellMapper;

private:
	class Impl;
	Impl* impl;

public:
	friend class GeoDataPointmapBreakLine;
	friend class GeoDataPointmapProxy;
};

#endif // GEODATAPOINTMAP_H
