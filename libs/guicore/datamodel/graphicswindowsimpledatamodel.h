#ifndef GRAPHICSWINDOWSIMPLEDATAMODEL_H
#define GRAPHICSWINDOWSIMPLEDATAMODEL_H

#include "../guicore_global.h"

#include "../project/projectdataitem.h"

class QMainWindow;
class VTKGraphicsView;

class GUICOREDLL_EXPORT GraphicsWindowSimpleDataModel : public ProjectDataItem
{
	Q_OBJECT

public:
	/// Constructor
	GraphicsWindowSimpleDataModel(QMainWindow* w, ProjectDataItem* parent);
	/// Destructor
	virtual ~GraphicsWindowSimpleDataModel();
	/// The view to display pre-processor main region
	VTKGraphicsView* graphicsView() const {return m_graphicsView;}
	QMainWindow* mainWindow() const {return m_mainWindow;}
	virtual void viewOperationEndedGlobal();

protected:
	QMainWindow* m_mainWindow;
	VTKGraphicsView* m_graphicsView;
};

#endif // GRAPHICSWINDOWDATAMODEL_H
