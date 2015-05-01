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
	static const int MOVEWIDTH = 1;
	/// Constructor
	GraphicsWindowSimpleDataModel(QMainWindow* w, ProjectDataItem* parent);
	/// Destructor
	virtual ~GraphicsWindowSimpleDataModel();
	/// The view to display pre-processor main region
	VTKGraphicsView* graphicsView() const {return m_graphicsView;}
	QMainWindow* mainWindow() {return m_mainWindow;}
	virtual void viewOperationEndedGlobal();

public slots:
	void fit();
	/// Zoom-in the pre-processor main view.
	void zoomIn();
	/// Zoom-out the pre-processor main view.
	void zoomOut();
	/// Move left
	void moveLeft();
	/// Move right
	void moveRight();
	/// Move up
	void moveUp();
	/// Move down
	void moveDown();

protected:
	QMainWindow* m_mainWindow;
	VTKGraphicsView* m_graphicsView;

private:
	qreal moveWidth();
	void moveCenter(int x, int y);
};

#endif // GRAPHICSWINDOWDATAMODEL_H
