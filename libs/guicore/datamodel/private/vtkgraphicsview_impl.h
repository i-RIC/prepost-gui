#ifndef VTKGRAPHICSVIEW_IMPL_H
#define VTKGRAPHICSVIEW_IMPL_H

#include "../vtkgraphicsview.h"

class vtkActor2D;
class vtkCamera;
class vtkGenericOpenGLRenderWindow;

class VTKGraphicsView::Impl
{
public:
	Impl();
	~Impl();

	vtkGenericOpenGLRenderWindow* m_renderWindow;
	GraphicsWindowDataItem* m_activeDataItem;
	GraphicsWindowSimpleDataModel* m_model;

	vtkInteractorObserver* m_rubberBarStyle;
	vtkSmartPointer<vtkInteractorObserver> m_styleBackUp;
	vtkRenderer* m_mainRenderer;
	vtkActor2D* m_logoActor;
	vtkCamera* m_camera;

	QImage m_logoImage;

	QPixmap m_zoomPixmap;
	QPixmap m_rotatePixmap;
	QPixmap m_movePixmap;
	QPixmap m_rubberBandPixmap;

	QCursor m_zoomCursor;
	QCursor m_rotateCursor;
	QCursor m_moveCursor;
	QCursor m_rubberBandCursor;

	bool m_interactive;
	bool m_isViewChanging;
	bool m_isRubberBandZooming;
};

#endif // VTKGRAPHICSVIEW_IMPL_H
