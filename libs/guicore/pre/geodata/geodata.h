#ifndef GEODATA_H
#define GEODATA_H

#include "../../guicore_global.h"
#include "../../project/projectdataitem.h"
#include "../../solverdef/solverdefinitiongridattribute.h"
#include <QString>
#include <QIcon>
#include <QAction>

#include <iriclib.h>

class GeoDataCreator;
class GeoDataImporter;
class GeoDataExporter;
class GeoDataMapper;
class PreProcessorWindowInterface;
class PreProcessorDataModelInterface;
class PreProcessorGeoDataDataItemInterface;
class PreProcessorGraphicsViewInterface;
class ZDepthRange;
class ScalarsToColorsContainer;
class MouseBoundingBox;
class GeoDataProxy;

class vtkRenderer;
class vtkActorCollection;
class vtkActor2DCollection;

class QKeyEvent;
class QMenu;
class QUndoCommand;
class QToolBar;
class QMouseEvent;
class QWidget;
class GridAttributeDimensionsContainer;

class GUICOREDLL_EXPORT GeoData : public ProjectDataItem
{
	Q_OBJECT

public:
	/// Constructor
	GeoData(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition);
	virtual ~GeoData();
	const QString& name() const {return m_name;}
	const QString& typeName() const;
	void setName(const QString& name) {
		m_name = name;
		updateFilename();
	}
	void setPosition(SolverDefinitionGridAttribute::Position pos) {
		if (pos == SolverDefinitionGridAttribute::Node) {
			mapperFunc = &GeoData::nodeMappers;
		} else {
			mapperFunc = &GeoData::cellMappers;
		}
	}
	const QString& caption() const {return m_caption;}
	void setCaption(const QString& cap) {m_caption = cap;}
	SolverDefinitionGridAttribute* gridRelatedCondition() const {return m_gridRelatedCondition;}
	const QIcon icon() const;
	/// Returns the pointer to the creator that created this instance.
	GeoDataCreator* creator() const {return m_creator;}
	virtual GeoDataMapper* mapper() const {return m_mapper;}
	void setMapper(GeoDataMapper* m) {m_mapper = m;}
	QList<GeoDataMapper*> mappers() const {
		return (this->*mapperFunc)();
	}
	void setDefaultMapper();
	QList<GeoDataMapper*> nodeMappers() const;
	QList<GeoDataMapper*> cellMappers() const;
	QList<GeoDataImporter*> importers();
	QList<GeoDataExporter*> exporters();
	virtual void setupDataItem();
	/// setup VTK actors.
	virtual void setupActors() {}
	virtual void setupMenu() {}
	virtual void showInitialDialog() {}
	virtual bool addToolBarButtons(QToolBar* /*parent*/) {return false;}
	QMenu* menu() {return m_menu;}
	virtual void handleStandardItemChange() {}
	virtual void handleStandardItemClicked() {}
	virtual void handleStandardItemDoubleClicked() {}
	virtual void informSelection(PreProcessorGraphicsViewInterface* /*v*/) {}
	virtual void informDeselection(PreProcessorGraphicsViewInterface* /*v*/) {}
	virtual void viewOperationEnded(PreProcessorGraphicsViewInterface* /*v*/) {}
	virtual void keyPressEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) {}
	virtual void keyReleaseEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) {}
	virtual void mouseDoubleClickEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) {}
	virtual void mouseMoveEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) {}
	virtual void mousePressEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) {}
	virtual void mouseReleaseEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) {}
	virtual void addCustomMenuItems(QMenu* /*menu*/) {}
	virtual void updateZDepthRangeItemCount(ZDepthRange& range);
	virtual void assignActorZValues(const ZDepthRange& /*range*/) {}
	virtual bool getValueRange(double* /*min*/, double* /*max*/) {return false;}
	virtual QDialog* propertyDialog(QWidget* /*parent*/) {return 0;}
	virtual void handlePropertyDialogAccepted(QDialog* /*propDialog*/) {}
	ScalarsToColorsContainer* scalarsToColorsContainer();
	virtual void update2Ds() {}
	bool isVisible();
	void setMapped() {m_mapped = true;}
	bool isMapped() const {return m_mapped;}
	virtual GeoDataProxy* getProxy() {return nullptr;}
	void saveToCgnsFile(const int fn) override;
	virtual void viewOperationEndedGlobal(PreProcessorGraphicsViewInterface* /*v*/) {}
	void applyOffset(double x, double y) {doApplyOffset(x, y);}
	virtual bool requestCoordinateSystem() const {return false;}

signals:
	void graphicsUpdated();

public slots:
	virtual void handleDimensionCurrentIndexChange(int /*oldIndex*/, int /*newIndex*/) {}
	virtual void handleDimensionValuesChange(const QList<QVariant>& /*before*/, const QList<QVariant>& /*after*/) {}

protected slots:
	void editName();

protected:
	void updateVisibility();
	void updateVisibilityWithoutRendering();
	QAction* deleteAction();
	QList<GeoDataMapper*> (GeoData::*mapperFunc)() const;
	vtkRenderer* renderer();
	void renderGraphicsView();
	vtkActorCollection* actorCollection();
	vtkActor2DCollection* actor2DCollection();
	virtual void updateFilename() {}

	void loadName(const QDomNode& node);
	void saveName(QXmlStreamWriter& writer);

	PreProcessorWindowInterface* preProcessorWindow();
	PreProcessorGraphicsViewInterface* graphicsView();
	PreProcessorGeoDataDataItemInterface* geoDataDataItem() const;
	void pushCommand(QUndoCommand* com);
	void pushRenderCommand(QUndoCommand* com);

	MouseBoundingBox* mouseBoundingBox();
	PreProcessorDataModelInterface* dataModel();
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	virtual int iRICLibType() const {return IRIC_GEO_UNKNOWN;}
	GridAttributeDimensionsContainer* dimensions() const;

	QAction* m_editNameAction;
	QString m_name;
	QString m_caption;
	GeoDataCreator* m_creator;
	SolverDefinitionGridAttribute* m_gridRelatedCondition;
	GeoDataMapper* m_mapper;
	QMenu* m_menu;
	bool m_mapped;
};

#endif // GEODATA_H
