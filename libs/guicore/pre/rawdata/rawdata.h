#ifndef RAWDATA_H
#define RAWDATA_H

#include "../../guicore_global.h"
#include "../../project/projectdataitem.h"
#include "../../solverdef/solverdefinitiongridrelatedcondition.h"
#include <QString>
#include <QIcon>
#include <QAction>

#include <iriclib.h>

class RawDataCreator;
class RawDataImporter;
class RawDataExporter;
class RawDataMapper;
class PreProcessorWindowInterface;
class PreProcessorDataModelInterface;
class PreProcessorRawdataDataItemInterface;
class PreProcessorGraphicsViewInterface;
class ZDepthRange;
class ScalarsToColorsContainer;
class MouseBoundingBox;
class RawDataProxy;

class vtkRenderer;
class vtkActorCollection;
class vtkActor2DCollection;

class QKeyEvent;
class QMenu;
class QToolBar;
class QMouseEvent;
class QWidget;
class GridRelatedConditionDimensionsContainer;

class GUICOREDLL_EXPORT RawData : public ProjectDataItem
{
	Q_OBJECT
public:
	/// Constructor
	RawData(ProjectDataItem* d, RawDataCreator* creator, SolverDefinitionGridRelatedCondition* condition);
	virtual ~RawData();
	const QString& name() const {return m_name;}
	const QString& typeName() const;
	void setName(const QString& name) {
		m_name = name;
		updateFilename();
	}
	void setPosition(SolverDefinitionGridRelatedCondition::Position pos) {
		if (pos == SolverDefinitionGridRelatedCondition::Node) {
			mapperFunc = &RawData::nodeMappers;
		} else {
			mapperFunc = &RawData::cellMappers;
		}
	}
	const QString& caption() const {return m_caption;}
	void setCaption(const QString& cap) {m_caption = cap;}
	SolverDefinitionGridRelatedCondition* gridRelatedCondition() {return m_gridRelatedCondition;}
	const QIcon icon() const;
	/// Returns the pointer to the creator that created this instance.
	RawDataCreator* creator() {return m_creator;}
	virtual RawDataMapper* mapper() {return m_mapper;}
	void setMapper(RawDataMapper* m) {m_mapper = m;}
	QList<RawDataMapper*> mappers() {
		return (this->*mapperFunc)();
	}
	void setDefaultMapper();
	QList<RawDataMapper*> nodeMappers();
	QList<RawDataMapper*> cellMappers();
	QList<RawDataImporter*> importers();
	QList<RawDataExporter*> exporters();
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
	virtual void assignActionZValues(const ZDepthRange& /*range*/) {}
	virtual bool getValueRange(double* /*min*/, double* /*max*/) {return false;}
	virtual QDialog* propertyDialog(QWidget* /*parent*/) {return 0;}
	virtual void handlePropertyDialogAccepted(QDialog* /*propDialog*/) {}
	ScalarsToColorsContainer* scalarsToColorsContainer();
	virtual void update2Ds() {}
	bool isVisible();
	void setMapped() {m_mapped = true;}
	bool isMapped() const {return m_mapped;}
	virtual RawDataProxy* getProxy() {return 0;}
	void saveToCgnsFile(const int fn);
	virtual void viewOperationEndedGlobal(PreProcessorGraphicsViewInterface* /*v*/) {}
	void applyOffset(double x, double y) {doApplyOffset(x, y);}
	virtual bool requestCoordinateSystem() {return false;}

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
	QList<RawDataMapper*> (RawData::*mapperFunc)();
	vtkRenderer* renderer();
	void renderGraphicsView();
	vtkActorCollection* actorCollection();
	vtkActor2DCollection* actor2DCollection();
	virtual void updateFilename() {}
	void loadName(const QDomNode& node);
	void saveName(QXmlStreamWriter& writer);
	PreProcessorWindowInterface* preProcessorWindow();
	PreProcessorGraphicsViewInterface* graphicsView();
	MouseBoundingBox* mouseBoundingBox();
	PreProcessorDataModelInterface* dataModel();
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	virtual int iRICLibType() const {return IRIC_GEO_UNKNOWN;}
	GridRelatedConditionDimensionsContainer* dimensions() const;

	QAction* m_editNameAction;
	QString m_name;
	QString m_caption;
	RawDataCreator* m_creator;
	SolverDefinitionGridRelatedCondition* m_gridRelatedCondition;
	RawDataMapper* m_mapper;
	QMenu* m_menu;
	bool m_mapped;
};

#endif // RAWDATA_H
