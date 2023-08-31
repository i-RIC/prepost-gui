#ifndef VTKGRAPHICSVIEW_H
#define VTKGRAPHICSVIEW_H

#include "../guicore_global.h"

#include <QVTKOpenGLNativeWidget.h>
#include <QUndoCommand>

#include <vtkSmartPointer.h>

class GraphicsWindowDataItem;
class GraphicsWindowSimpleDataModel;
class vtkCamera;
class vtkRenderer;
class vtkInteractorObserver;

class GUICOREDLL_EXPORT VTKGraphicsView : public QVTKOpenGLNativeWidget
{
	Q_OBJECT

public:
	VTKGraphicsView(QWidget* parent);
	virtual ~VTKGraphicsView();

	double stdDistance(double pixels);

	/// @name Model related functions
	//@{
	void setModel(GraphicsWindowSimpleDataModel* m);
	GraphicsWindowDataItem* activeDataItem() const;
	void setActiveDataItem(GraphicsWindowDataItem* i);
	//@}

	/// @name Mouse event handling functions
	//@{
	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;
	bool event(QEvent* event) override;

	void standardKeyPressEvent(QKeyEvent* event);
	void standardKeyReleaseEvent(QKeyEvent* event);
	void standardMouseDoubleClickEvent(QMouseEvent* event);
	void standardMousePressEvent(QMouseEvent* event);
	void standardMouseReleaseEvent(QMouseEvent* event);
	void standardMouseMoveEvent(QMouseEvent* event);
	void standardWheelEvent(QWheelEvent* event);
	//@}

	vtkRenderer* mainRenderer() const;
	vtkCamera* camera() const;
	void restoreUpdateRate();
	void setInteractive(bool interactive);

	QImage getImage();

	QMouseEvent createScaledEvent(const QMouseEvent& event);
	QMouseEvent createReverseScaledEvent(const QMouseEvent& event);

public slots:
	void render();

	void cameraFit();
	void cameraZoomIn();
	void cameraZoomOut();
	void cameraMoveLeft();
	void cameraMoveRight();
	void cameraMoveUp();
	void cameraMoveDown();

protected:
	GraphicsWindowSimpleDataModel* model() const;

private:
	/// @name Viewport related functions
	//@{
	void scale(double s);
	virtual void fitInView() {}
	virtual void translate(int x, int y) = 0;

	void moveCenter(int x, int y);
	int moveWidth();
	//@}

protected:
	virtual void resizeEvent(QResizeEvent* event) override;
	void update2Ds();

	void setupCamera();

public:
	friend class VTKGraphicsViewArbitraryMove;

private:
	class Impl;
	Impl* impl;
};

class VTKGraphicsViewArbitraryMove : public QUndoCommand
{

public:
	VTKGraphicsViewArbitraryMove(vtkCamera* camera, VTKGraphicsView* view);
	~VTKGraphicsViewArbitraryMove();
	void undo() override;
	void redo() override;

private:
	vtkCamera* m_newCamera;
	vtkCamera* m_oldCamera;
	VTKGraphicsView* m_view;
};

#ifdef _DEBUG
	#include "private/vtkgraphicsview_impl.h"
#endif // _DEBUG

#endif // VTKGRAPHICSVIEW_H
