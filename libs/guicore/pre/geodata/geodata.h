#ifndef GEODATA_H
#define GEODATA_H

#include "../../guicore_global.h"
#include "../../project/projectdataitem.h"
#include "../../solverdef/solverdefinitiongridattribute.h"

#include <misc/compositecontainer.h>
#include <misc/stringcontainer.h>
#include <misc/boolcontainer.h>

class GeoDataCreator;
class GeoDataImporter;
class GeoDataExporter;
class GeoDataMapper;
class GeoDataProxy;
class GridAttributeDimensionsContainer;
class MouseBoundingBox;
class PreProcessorWindowI;
class PreProcessorDataModelI;
class PreProcessorGeoDataTopDataItemI;
class PreProcessorGeoDataGroupDataItemI;
class PreProcessorGeoDataDataItemI;
class PreProcessorGraphicsViewI;
class PreProcessorGridTypeDataItemI;
class PreProcessorHydraulicDataTopDataItemI;
class PreProcessorHydraulicDataGroupDataItemI;
class ZDepthRange;

class vtkRenderer;
class vtkActorCollection;
class vtkActor2DCollection;

class QAction;
class QDialog;
class QKeyEvent;
class QMenu;
class QUndoCommand;
class QMouseEvent;
class QResizeEvent;
class QToolBar;
class QWidget;

#include <h5cgnsgeographicdatagroup.h>

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

	QString caption() const;
	virtual void setCaption(const QString& cap);

	SolverDefinitionGridAttribute* gridAttribute() const;
	GeoDataCreator* creator() const;

	virtual GeoDataMapper* mapper() const = 0;

	std::vector<GeoDataImporter*> importers() const;
	std::vector<GeoDataExporter*> exporters() const;

	virtual void setupDataItem();
	/// setup VTK actors.
	virtual void setupActors();
	virtual void setupMenu();
	virtual void showInitialDialog();
	virtual bool addToolBarButtons(QToolBar* parent);
	virtual void applyColorMapSetting();

	QMenu* menu() const;

	virtual void handleStandardItemChange();
	virtual void handleStandardItemDoubleClicked();
	virtual void informSelection(PreProcessorGraphicsViewI* v);
	virtual void informDeselection(PreProcessorGraphicsViewI* v);

	virtual void viewOperationEnded(PreProcessorGraphicsViewI* v);
	virtual void viewOperationEndedGlobal(PreProcessorGraphicsViewI* v);

	virtual void handleResize(QResizeEvent* event, PreProcessorGraphicsViewI* v);

	virtual void keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewI* v);
	virtual void keyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewI* v);

	virtual void mouseDoubleClickEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v);
	virtual void mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v);
	virtual void mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v);
	virtual void mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v);

	virtual void addCustomMenuItems(QMenu* menu);

	virtual void updateZDepthRangeItemCount(ZDepthRange& range);
	virtual void assignActorZValues(const ZDepthRange& range);
	virtual bool getValueRange(double* min, double* max);

	virtual void showPropertyDialog();
	virtual QDialog* propertyDialog(QWidget* parent);
	virtual void handlePropertyDialogAccepted(QDialog* propDialog);

	ColorMapSettingContainerI* colorMapSettingContainer() const;
	virtual void update2Ds();

	bool isVisible() const;
	bool isMapped() const;
	void setMapped(bool mapped = true);

	virtual GeoDataProxy* getProxy();

	void applyOffset(double x, double y);
	virtual iRICLib::H5CgnsGeographicDataGroup::Type iRICLibType() const;

signals:
	void updateActorSettingExecuted();

public slots:
	virtual void handleDimensionCurrentIndexChange(int oldIndex, int newIndex);
	virtual void handleDimensionValuesChange(const std::vector<QVariant>& before, const std::vector<QVariant>& after);

protected slots:
	void editName();

protected:
	void updateVisibility();
	void updateVisibilityWithoutRendering();
	QAction* deleteAction();
	vtkRenderer* renderer();
	void renderGraphicsView();
	vtkActorCollection* actorCollection();
	vtkActor2DCollection* actor2DCollection();
	virtual void updateFilename() {}
	virtual void updateActorSetting();

	PreProcessorWindowI* preProcessorWindow();
	PreProcessorGraphicsViewI* graphicsView();
	PreProcessorGeoDataTopDataItemI* geoDataTopDataItem() const;
	PreProcessorGeoDataGroupDataItemI* geoDataGroupDataItem() const;
	PreProcessorGeoDataDataItemI* geoDataDataItem() const;
	PreProcessorGridTypeDataItemI* gridTypeDataItem() const;
	PreProcessorHydraulicDataTopDataItemI* hydraulicDataTopDataItem() const;
	PreProcessorHydraulicDataGroupDataItemI* hydraulicDataGroupDataItem(const std::string& name) const;
	void pushCommand(QUndoCommand* com);
	void pushRenderCommand(QUndoCommand* com);
	void pushRenderRedoOnlyCommand(QUndoCommand* com);
	void showPropertyDialogModal();
	void showPropertyDialogModeless();

	MouseBoundingBox* mouseBoundingBox();
	PreProcessorDataModelI* dataModel() const;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	GridAttributeDimensionsContainer* dimensions() const;

	Setting m_setting;

	GeoDataCreator* m_creator;
	SolverDefinitionGridAttribute* m_gridAttribute;

	QMenu* m_menu;
	QAction* m_editNameAction;

	class PropertyDialog;
	class UpdateActorSettingCommand;

public:
	friend class GeoDataProxy;
};

#endif // GEODATA_H
