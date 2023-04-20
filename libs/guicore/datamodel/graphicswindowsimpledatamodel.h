#ifndef GRAPHICSWINDOWSIMPLEDATAMODEL_H
#define GRAPHICSWINDOWSIMPLEDATAMODEL_H

#include "../guicore_global.h"

#include "../project/projectdataitem.h"

class QMainWindow;
class QResizeEvent;
class VTKGraphicsView;

class GUICOREDLL_EXPORT GraphicsWindowSimpleDataModel : public ProjectDataItem
{
	Q_OBJECT

public:
	GraphicsWindowSimpleDataModel(QMainWindow* w, ProjectDataItem* parent);
	~GraphicsWindowSimpleDataModel() override;

	VTKGraphicsView* graphicsView() const;
	QMainWindow* mainWindow() const;

	virtual void viewOperationEndedGlobal();
	virtual void handleResize(QResizeEvent* event);

protected:
	VTKGraphicsView* m_graphicsView;

private:
	QMainWindow* m_mainWindow;
};

#endif // GRAPHICSWINDOWDATAMODEL_H
