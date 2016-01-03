#ifndef GEODATAPROXY_H
#define GEODATAPROXY_H

#include "../../guicore_global.h"
#include "../../project/projectdataitem.h"

class GeoData;
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

	virtual QDialog* propertyDialog(QWidget* parent);
	virtual void handlePropertyDialogAccepted(QDialog* propDialog);

public slots:
	virtual void updateGraphics();

protected:
	QMainWindow* mainWindow() const;

	void renderGraphicsView();
	vtkRenderer* renderer();

	vtkActorCollection* actorCollection() const;
	vtkActor2DCollection* actor2DCollection() const;

	void updateVisibility();
	void updateVisibilityWithoutRendering();
	bool isVisible();

	void pushCommand(QUndoCommand* com);
	void pushRenderCommand(QUndoCommand* com);

private:
	GeoData* m_geoData;
};

#endif // GEODATAPROXY_H
