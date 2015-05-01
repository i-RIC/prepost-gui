#ifndef RAWDATAPROXY_H
#define RAWDATAPROXY_H

#include "../../guicore_global.h"
#include "../../project/projectdataitem.h"

class QDialog;
class RawData;
class vtkActorCollection;
class vtkActor2DCollection;
class vtkRenderer;
class QMainWindow;
class ZDepthRange;

class GUICOREDLL_EXPORT RawDataProxy : public ProjectDataItem
{
	Q_OBJECT
public:
	RawDataProxy(RawData* rawdata) : ProjectDataItem(0) {
		m_rawData = rawdata;
	}
	virtual void setupActors() {}
	virtual void setupMenu() {}
	virtual void setupDataItem();
	virtual void updateZDepthRangeItemCount(ZDepthRange& range);
	virtual void assignActionZValues(const ZDepthRange& range);
	RawData* rawData() const {return m_rawData;}
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

	RawData* m_rawData;
};

#endif // RAWDATAPROXY_H
