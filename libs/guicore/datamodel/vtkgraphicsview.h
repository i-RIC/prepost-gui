#ifndef VTKGRAPHICSVIEW_H
#define VTKGRAPHICSVIEW_H

#include "../guicore_global.h"

#include <QVTKWidget.h>
#include <QUndoCommand>

class GraphicsWindowDataItem;
class GraphicsWindowSimpleDataModel;
class vtkRenderer;
class vtkCamera;
class vtkActor2D;
class QCursor;

class GUICOREDLL_EXPORT VTKGraphicsView : public QVTKWidget
{
public:
	VTKGraphicsView(QWidget* parent);
	virtual ~VTKGraphicsView();
	void setModel(GraphicsWindowSimpleDataModel* m) {m_model = m;}
	GraphicsWindowDataItem* activeDataItem() {return m_activeDataItem;}
	void setActiveDataItem(GraphicsWindowDataItem* i) {
		m_activeDataItem = i;
	}
	void scale(double s);
	virtual void fitInView() {}
	virtual void translate(int x, int y) = 0;

	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;

	void standardKeyPressEvent(QKeyEvent* event);
	void standardKeyReleaseEvent(QKeyEvent* event);
	void standardMouseDoubleClickEvent(QMouseEvent* event);
	void standardMousePressEvent(QMouseEvent* event);
	void standardMouseReleaseEvent(QMouseEvent* event);
	void standardMouseMoveEvent(QMouseEvent* event);
	void standardWheelEvent(QWheelEvent* event);

	vtkRenderer* mainRenderer() const {return m_mainRenderer;}
	void render();
	void restoreUpdateRate();
	void setInteractive(bool interactive) {
		m_interactive = interactive;
	}

	QImage getImage();

private:
	QPixmap m_zoomPixmap;
	QPixmap m_rotatePixmap;
	QPixmap m_movePixmap;

	QCursor m_zoomCursor;
	QCursor m_rotateCursor;
	QCursor m_moveCursor;

	QImage m_logoImage;

	GraphicsWindowDataItem* m_activeDataItem;
	GraphicsWindowSimpleDataModel* m_model;
	bool m_isViewChanging;

protected:
	virtual void resizeEvent(QResizeEvent* event);
	void update2Ds();

	bool m_interactive;
	vtkActor2D* m_logoActor;

	vtkRenderer* m_mainRenderer;
	vtkCamera* m_camera;
	void setupCamera();

public:
	friend class VTKGraphicsViewArbitraryMove;
};

class VTKGraphicsViewArbitraryMove : public QUndoCommand
{
public:
	VTKGraphicsViewArbitraryMove(vtkCamera* camera, VTKGraphicsView* view);
	~VTKGraphicsViewArbitraryMove();
	void undo();
	void redo();
private:
	vtkCamera* m_newCamera;
	vtkCamera* m_oldCamera;
	VTKGraphicsView* m_view;
};

#endif // VTKGRAPHICSVIEW_H
