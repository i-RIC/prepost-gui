#ifndef GEODATAPROXY_H
#define GEODATAPROXY_H

#include "../../guicore_global.h"
#include "../../project/projectdataitem.h"

class QDialog;
class GeoData;
class vtkActorCollection;
class vtkActor2DCollection;
class vtkRenderer;
class QMainWindow;
class ZDepthRange;

class GUICOREDLL_EXPORT GeoDataProxy : public ProjectDataItem
{
	Q_OBJECT

public:
	GeoDataProxy(GeoData* geodata) : ProjectDataItem(0) {
		m_geoData = geodata;
	}
	virtual ~GeoDataProxy() {}
	virtual void setupActors() {}
	virtual void setupMenu() {}
	virtual void setupDataItem();
	virtual void updateZDepthRangeItemCount(ZDepthRange& range);
	virtual void assignActorZValues(const ZDepthRange& range);
	GeoData* geoData() const {return m_geoData;}
	virtual QDialog* propertyDialog(QWidget* /*parent*/) {return 0;}
	virtual void handlePropertyDialogAccepted(QDialog* /*propDialog*/) {}

public slots:
	virtual void updateGraphics() {}

protected:
	QMainWindow* mainWindow() const;
	void renderGraphicsView();
	vtkActorCollection* actorCollection() const;
	vtkActor2DCollection* actor2DCollection() const;
	vtkRenderer* renderer();
	void updateVisibility();
	void updateVisibilityWithoutRendering();
	bool isVisible();

	GeoData* m_geoData;
};

#endif // GEODATAPROXY_H
