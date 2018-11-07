#ifndef GEODATA_H
#define GEODATA_H

#include "../../guicore_global.h"
#include "../../project/projectdataitem.h"
#include "../../solverdef/solverdefinitiongridattribute.h"

#include <misc/compositecontainer.h>
#include <misc/stringcontainer.h>
#include <misc/boolcontainer.h>

#include <iriclib.h>

class GeoDataCreator;
class GeoDataImporter;
class GeoDataExporter;
class GeoDataMapper;
class PreProcessorWindowInterface;
class PreProcessorDataModelInterface;
class PreProcessorGeoDataTopDataItemInterface;
class PreProcessorGeoDataGroupDataItemInterface;
class PreProcessorGeoDataDataItemInterface;
class PreProcessorGraphicsViewInterface;
class PreProcessorHydraulicDataTopDataItemInterface;
class PreProcessorHydraulicDataGroupDataItemInterface;
class ZDepthRange;
class ScalarsToColorsContainer;
class MouseBoundingBox;
class GeoDataProxy;

class vtkRenderer;
class vtkActorCollection;
class vtkActor2DCollection;

class QAction;
class QDialog;
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
	struct Setting : public CompositeContainer
	{
		/// Constructor
		Setting();

		StringContainer name;
		StringContainer caption;
		BoolContainer mapped;
	};

	GeoData(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition);
	virtual ~GeoData();

	QString name() const;
	void setName(const QString& name);

	const QString& typeName() const;

	void setPosition(SolverDefinitionGridAttribute::Position pos);

	QString caption() const;
	void setCaption(const QString& cap);

	SolverDefinitionGridAttribute* gridAttribute() const;
	GeoDataCreator* creator() const;

	virtual GeoDataMapper* mapper() const;
	void setMapper(GeoDataMapper* m);
	std::vector<GeoDataMapper*> mappers() const;
	void setDefaultMapper();

	std::vector<GeoDataMapper*> nodeMappers() const;
	std::vector<GeoDataMapper*> cellMappers() const;

	std::vector<GeoDataImporter*> importers() const;
	std::vector<GeoDataExporter*> exporters() const;

	virtual void setupDataItem();
	/// setup VTK actors.
	virtual void setupActors();
	virtual void setupMenu();
	virtual void showInitialDialog();
	virtual bool addToolBarButtons(QToolBar* parent);

	QMenu* menu() const;

	virtual void handleStandardItemChange();
	virtual void handleStandardItemClicked();
	virtual void handleStandardItemDoubleClicked();
	virtual void informSelection(PreProcessorGraphicsViewInterface* v);
	virtual void informDeselection(PreProcessorGraphicsViewInterface* v);

	virtual void viewOperationEnded(PreProcessorGraphicsViewInterface* v);
	virtual void viewOperationEndedGlobal(PreProcessorGraphicsViewInterface* v);

	virtual void keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v);
	virtual void keyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v);

	virtual void mouseDoubleClickEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v);
	virtual void mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v);
	virtual void mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v);
	virtual void mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v);

	virtual void addCustomMenuItems(QMenu* menu);

	virtual void updateZDepthRangeItemCount(ZDepthRange& range);
	virtual void assignActorZValues(const ZDepthRange& range);
	virtual bool getValueRange(double* min, double* max);
	virtual QDialog* propertyDialog(QWidget* parent);
	virtual void handlePropertyDialogAccepted(QDialog* propDialog);
	ScalarsToColorsContainer* scalarsToColorsContainer();
	virtual void update2Ds();

	bool isVisible() const;
	bool isMapped() const;
	void setMapped(bool mapped = true);

	virtual GeoDataProxy* getProxy();

	void saveToCgnsFile(const int fn) override;
	void applyOffset(double x, double y);
	virtual bool requestCoordinateSystem() const;

signals:
	void graphicsUpdated();

public slots:
	virtual void handleDimensionCurrentIndexChange(int oldIndex, int newIndex);
	virtual void handleDimensionValuesChange(const std::vector<QVariant>& before, const std::vector<QVariant>& after);

protected slots:
	void editName();

protected:
	void updateVisibility();
	void updateVisibilityWithoutRendering();
	QAction* deleteAction();
	std::vector<GeoDataMapper*> (GeoData::*mapperFunc)() const;
	vtkRenderer* renderer();
	void renderGraphicsView();
	vtkActorCollection* actorCollection();
	vtkActor2DCollection* actor2DCollection();
	virtual void updateFilename() {}

	PreProcessorWindowInterface* preProcessorWindow();
	PreProcessorGraphicsViewInterface* graphicsView();
	PreProcessorGeoDataTopDataItemInterface* geoDataTopDataItem() const;
	PreProcessorGeoDataGroupDataItemInterface* geoDataGroupDataItem() const;
	PreProcessorGeoDataDataItemInterface* geoDataDataItem() const;
	PreProcessorHydraulicDataTopDataItemInterface* hydraulicDataTopDataItem() const;
	PreProcessorHydraulicDataGroupDataItemInterface* hydraulicDataGroupDataItem(const std::string& name) const;
	void pushCommand(QUndoCommand* com);
	void pushRenderCommand(QUndoCommand* com);
	void pushRenderRedoOnlyCommand(QUndoCommand* com);

	MouseBoundingBox* mouseBoundingBox();
	PreProcessorDataModelInterface* dataModel() const;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	virtual int iRICLibType() const;
	GridAttributeDimensionsContainer* dimensions() const;

	Setting m_setting;

	GeoDataCreator* m_creator;
	SolverDefinitionGridAttribute* m_gridAttribute;
	GeoDataMapper* m_mapper;

	QMenu* m_menu;
	QAction* m_editNameAction;

public:
	friend class GeoDataProxy;
};

#endif // GEODATA_H
