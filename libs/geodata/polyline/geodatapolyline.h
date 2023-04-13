#ifndef GEODATAPOLYLINE_H
#define GEODATAPOLYLINE_H

#include "gd_polyline_global.h"

#include <geodata/polydata/geodatapolydata.h>

class GeoDataPolyLineAbstractPolyLine;
class GeoDataPolyLineImplPolyLine;

class vtkActor;
class vtkCellArray;
class vtkMapper;
class vtkPoints;
class vtkPolyData;

class QAction;
class QMenu;
class QToolBar;

namespace geos {
namespace geom{

class LineString;

} // geom
} // geos


class GD_POLYLINE_EXPORT GeoDataPolyLine : public GeoDataPolyData
{
	Q_OBJECT

public:
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

	GeoDataPolyLine(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition);
	~GeoDataPolyLine() override;

	QColor color() const;

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

	void getBoundingRect(double* xmin, double* xmax, double* ymin, double* ymax) override;

	void definePolyLine(bool doubleClick, bool noEditVal = false);

	void clear();

	void setShape(geos::geom::LineString* lineString);

	void showInitialDialog() override;

	bool inNormalMode() const override;
	bool isDefined() const override;

	geos::geom::LineString* getGeosLineString(const QPointF& offset = QPointF(0, 0));

	GeoDataProxy* getProxy() override;
	void copyShape(GeoDataPolyLine* polyline);

	GeoDataPolyLineImplPolyLine* polyLine() const;

	QDialog* propertyDialog(QWidget* parent) override;
	void showPropertyDialog() override;

	class DisplaySettingWidget;

public slots:
	void restoreMouseEventMode();

private slots:
	void addVertexMode(bool on);
	void removeVertexMode(bool on);
	void editCoordinates();

signals:
	void modified();

private:
	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;
	void doApplyOffset(double x, double y) override;
	iRICLib::H5CgnsGeographicDataGroup::Type iRICLibType() const override;

	void setMouseEventMode(MouseEventMode mode);

	void updateMouseCursor(PreProcessorGraphicsViewInterface* v);
	void updateScalarValues() override;
	void updateActorSetting() override;
	bool polylineHasThreeVertices();
	void updateMouseEventMode();
	void updateActionStatus();

	bool isReady() const override;
	QString shapeName() const override;
	QString shapeNameCamelCase() const override;

	QAction* addVertexAction() const;
	QAction* removeVertexAction() const;
	QAction* coordEditAction() const;

	class AddVertexCommand;
	class PushNewPointCommand;
	class EditCoordinatesCommand;
	class FinishPolyLineDefiningCommand;
	class ModifyAbstractPolyLineCommand;
	class MovePolyLineCommand;
	class MoveVertexCommand;
	class RemoveVertexCommand;

	class CoordinatesEditor;
	class DisplaySetting;

	class Impl;
	Impl* impl;

public:
	friend class GeoDataPolyLineAbstractPolyLine;
	friend class GeoDataPolyLineGroup;
	friend class GeoDataPolyLineGroupPolyLine;
	friend class GeoDataPolyLineImporter;
	friend class GeoDataPolyLineShapeExporter;
	friend class GeoDataPolyLineProxy;
};

#endif // GEODATAPOLYLINE_H
