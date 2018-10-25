#ifndef GEODATAPOLYLINE_H
#define GEODATAPOLYLINE_H

#include "gd_polyline_global.h"
#include "geodatapolylinecolorsimplesettingdialog.h"

#include <guicore/pre/geodata/geodata.h>

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


class GD_POLYLINE_EXPORT GeoDataPolyLine : public GeoData
{
	Q_OBJECT

private:
	const static int normalEdgeWidth = 1;
	const static int selectedEdgeWidth = 2;

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
	virtual ~GeoDataPolyLine();

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

	void definePolyLine(bool doubleClick, bool noEditVal = false);
	QColor color() const;

	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* d) override;

	QColor doubleToColor(double d);
	void clear();
	bool ready() const;

	void showInitialDialog() override;

	const QVariant& variantValue() const;
	void setVariantValue(const QVariant& v, bool disableInform = false);

	geos::geom::LineString* getGeosLineString(const QPointF& offset);

	bool getValueRange(double* min, double* max) override;
	void updateFilename() override;
	GeoDataProxy* getProxy() override;
	void copyShape(GeoDataPolyLine* polyline);

public slots:
	void editValue();
	void restoreMouseEventMode();
	void handleDimensionCurrentIndexChange(int oldIndex, int newIndex) override;
	void handleDimensionValuesChange(const std::vector<QVariant>& before, const std::vector<QVariant>& after) override;

private slots:
	void addVertexMode(bool on);
	void removeVertexMode(bool on);
	void editCoordinates();
	void editColorSetting();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;
	void doApplyOffset(double x, double y) override;
	int iRICLibType() const override;

	void setMouseEventMode(MouseEventMode mode);

	GeoDataPolyLineColorSimpleSettingDialog::Setting colorSetting() const;
	void setColorSetting(GeoDataPolyLineColorSimpleSettingDialog::Setting);

	void updateMouseCursor(PreProcessorGraphicsViewInterface* v);
	void updateScalarValues();
	void updateActorSettings();
	bool polylineHasThreeVertices();
	void initParams();
	void updateMouseEventMode();
	void updateActionStatus();

	GeoDataPolyLineImplPolyLine* polyLine() const;

	class AddVertexCommand;
	class PushNewPointCommand;
	class EditCoordinatesCommand;
	class EditPropertyCommand;
	class EditValueCommand;
	class FinishPolyLineDefiningCommand;
	class ModifyAbstractPolyLineCommand;
	class MovePolyLineCommand;
	class MoveVertexCommand;
	class RemoveVertexCommand;

	class CoordinatesEditor;

	class Impl;
	Impl* impl;

public:
	friend class GeoDataPolyLineAbstractPolyLine;
	friend class GeoDataPolyLineImporter;
	friend class GeoDataPolyLineShapeExporter;
	friend class GeoDataPolyLineProxy;
};

#endif // GEODATAPOLYLINE_H
