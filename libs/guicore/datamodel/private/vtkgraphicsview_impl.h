#ifndef VTKGRAPHICSVIEW_IMPL_H
#define VTKGRAPHICSVIEW_IMPL_H

#include "../vtkgraphicsview.h"

#include <QTimer>

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
	vtkCamera* m_camera;

	QPixmap m_zoomPixmap;
	QPixmap m_rotatePixmap;
	QPixmap m_movePixmap;
	QPixmap m_rubberBandPixmap;

	QCursor m_zoomCursor;
	QCursor m_rotateCursor;
	QCursor m_moveCursor;
	QCursor m_rubberBandCursor;

	QTimer m_resizeTimer;

	bool m_interactive;
	bool m_isViewChanging;
	bool m_isRubberBandZooming;
};

#endif // VTKGRAPHICSVIEW_IMPL_H
