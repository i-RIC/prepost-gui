#ifndef GEODATAPROXY_H
#define GEODATAPROXY_H

#include "../../guicore_global.h"
#include "../../project/projectdataitem.h"

class ColorMapSettingContainerI;
class GeoData;
class GraphicsWindowDataModel;
class Post2dWindowGeoDataDataItemInterface;
class Post2dWindowGridTypeDataItemInterface;
class ZDepthRange;

class QDialog;
class QMainWindow;
class QUndoCommand;

class vtkActorCollection;
class vtkActor2DCollection;
class vtkRenderer;

class GUICOREDLL_EXPORT GeoDataProxy : public ProjectDataItem
{
	Q_OBJECT

public:
	GeoDataProxy(GeoData* geodata);
	virtual ~GeoDataProxy();

	virtual void setupActors();
	virtual void setupMenu();
	virtual void setupDataItem();

	virtual void updateZDepthRangeItemCount(ZDepthRange& range);
	virtual void assignActorZValues(const ZDepthRange& range);

	GeoData* geoData() const;

	virtual void showPropertyDialog();
	virtual QDialog* propertyDialog(QWidget* parent);
	virtual void handlePropertyDialogAccepted(QDialog* propDialog);

	Post2dWindowGeoDataDataItemInterface* geoDataDataItem() const;
	Post2dWindowGridTypeDataItemInterface* gridTypeDataItem() const;
	void applyColorMapSetting();
	ColorMapSettingContainerI* colorMapSettingContainer() const;

public slots:
	virtual void updateActorSetting();

protected:
	QMainWindow* mainWindow() const;

	void renderGraphicsView();
	vtkRenderer* renderer();

	vtkActorCollection* actorCollection() const;
	vtkActor2DCollection* actor2DCollection() const;

	void updateVisibility();
	void updateVisibilityWithoutRendering();
	bool isVisible();

	GraphicsWindowDataModel* dataModel() const;

	void pushCommand(QUndoCommand* com);
	void pushRenderCommand(QUndoCommand* com);
	void showPropertyDialogModal();
	void showPropertyDialogModeless();

	class PropertyDialog;
	class UpdateActorSettingCommand;

private:
	GeoData* m_geoData;
};

#endif // GEODATAPROXY_H
