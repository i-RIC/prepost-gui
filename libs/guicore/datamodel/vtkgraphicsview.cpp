#define _USE_MATH_DEFINES
#include <cmath>

#include "vtkgraphicsview.h"
#include "private/vtkgraphicsview_impl.h"
#include "graphicswindowdataitem.h"
#include "graphicswindowdatamodel.h"

#include <misc/iricundostack.h>

#include <vtkTextSource.h>
#include <vtkVectorText.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleRubberBandZoom.h>
#include <vtkMath.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkInteractorStyle.h>
#include <vtkActor2D.h>
#include <vtkImageMapper.h>

#include <QMouseEvent>
#include <QWheelEvent>
#include <QPixmap>
#include <QCursor>
#include <QInputEvent>
#include <QUndoCommand>
#include <QPainter>
#include <vtkQImageToImageSource.h>

VTKGraphicsView::Impl::Impl() :
	m_activeDataItem {nullptr},
	m_model {nullptr},

	m_rubberBarStyle(vtkInteractorStyleRubberBandZoom::New()),
	m_styleBackUp(),
	m_camera(vtkCamera::New()),
	m_mainRenderer(vtkRenderer::New()),
	m_logoActor(vtkActor2D::New()),

	m_logoImage(":/libs/guicore/images/logo.png"),

	m_zoomPixmap(":/libs/guibase/images/cursorZoom.png"),
	m_rotatePixmap(":/libs/guibase/images/cursorRotate.png"),
	m_movePixmap(":/libs/guibase/images/cursorMove.png"),
	m_rubberBandPixmap(":/libs/guicore/images/cursorImageZoom.png"),

	m_zoomCursor(m_zoomPixmap),
	m_rotateCursor(m_rotatePixmap),
	m_moveCursor(m_movePixmap),
	m_rubberBandCursor(m_rubberBandPixmap),

	m_interactive {false},
	m_isViewChanging {false},
	m_isRubberBandZooming {false}
{}

VTKGraphicsView::Impl::~Impl()
{
	m_rubberBarStyle->Delete();
	m_mainRenderer->Delete();
	m_logoActor->Delete();
}

VTKGraphicsView::VTKGraphicsView(QWidget* parent) :
	QVTKWidget(parent),
	impl {new Impl()}
{
	impl->m_mainRenderer->SetBackground(1.0, 1.0, 1.0);

	vtkSmartPointer<vtkQImageToImageSource> imgToImg = vtkSmartPointer<vtkQImageToImageSource>::New();
	imgToImg->SetQImage(&(impl->m_logoImage));
	vtkSmartPointer<vtkImageMapper> imgMapper = vtkSmartPointer<vtkImageMapper>::New();
	imgMapper->SetColorWindow(255);
	imgMapper->SetColorLevel(127.5);
	imgMapper->SetInputConnection(imgToImg->GetOutputPort());

	// setup logo mapper
	impl->m_logoActor->SetMapper(imgMapper);
	impl->m_mainRenderer->AddActor2D(impl->m_logoActor);

	vtkRenderWindow* renderWindow = GetRenderWindow();
	renderWindow->SetMultiSamples(0);
	renderWindow->AddRenderer(impl->m_mainRenderer);
	renderWindow->SetStereoTypeToDresden();

	// Set the camera to be paralell projection, because it suits
	// for 2D graphics.
	impl->m_mainRenderer->GetActiveCamera()->ParallelProjectionOn();
}

VTKGraphicsView::~VTKGraphicsView()
{
	delete impl;
}


void VTKGraphicsView::setModel(GraphicsWindowSimpleDataModel* m)
{
	impl->m_model = m;
}

GraphicsWindowDataItem* VTKGraphicsView::activeDataItem() const
{
	return impl->m_activeDataItem;
}

void VTKGraphicsView::setActiveDataItem(GraphicsWindowDataItem* i)
{
	impl->m_activeDataItem = i;
}

void VTKGraphicsView::scale(double s)
{
	setupCamera();
	impl->m_mainRenderer->GetActiveCamera()->Zoom(s);
	update2Ds();
}

void VTKGraphicsView::keyPressEvent(QKeyEvent* event)
{
	if (impl->m_activeDataItem != nullptr) {
		impl->m_activeDataItem->keyPressEvent(event, this);
	} else if (impl->m_interactive) {
		QVTKWidget::keyPressEvent(event);
	}
}

void VTKGraphicsView::keyReleaseEvent(QKeyEvent* event)
{
	if (impl->m_activeDataItem != nullptr) {
		impl->m_activeDataItem->keyReleaseEvent(event, this);
	} else if (impl->m_interactive) {
		QVTKWidget::keyReleaseEvent(event);
	}
}

void VTKGraphicsView::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (impl->m_activeDataItem != nullptr) {
		impl->m_activeDataItem->mouseDoubleClickEvent(event, this);
	} else if (impl->m_interactive) {
		QVTKWidget::mouseDoubleClickEvent(event);
	}
}

void VTKGraphicsView::mousePressEvent(QMouseEvent* event)
{
	// Emit a mouse press event for anyone who might be interested
	emit mouseEvent(event);

	vtkRenderWindowInteractor* iren = NULL;
	if (this->mRenWin) {
		iren = this->mRenWin->GetInteractor();
	}
	if (!iren || !iren->GetEnabled()) {
		return;
	}

	// VTK Interactor implementation is used only for view changes.
	// view changes are done with Ctrl button pressed.
	if ((event->modifiers() & Qt::ControlModifier) != 0) {
		impl->m_isViewChanging = true;

		// give interactor the event information
		iren->SetEventInformationFlipY(event->x(), event->y(), 0, 0, 0, event->type() == QEvent::MouseButtonDblClick ? 1 : 0);
		switch (event->button()) {
		case Qt::LeftButton:
			if ((event->modifiers() & Qt::ShiftModifier) != 0) {
				setCursor(impl->m_rubberBandCursor);
				impl->m_styleBackUp = iren->GetInteractorStyle();
				iren->SetInteractorStyle(impl->m_rubberBarStyle);
				iren->InvokeEvent(vtkCommand::LeftButtonPressEvent, event);
				impl->m_isRubberBandZooming = true;
			} else {
				setCursor(impl->m_moveCursor);
				iren->InvokeEvent(vtkCommand::MiddleButtonPressEvent, event);
			}
			break;
		case Qt::MidButton:
			setCursor(impl->m_zoomCursor);
			iren->InvokeEvent(vtkCommand::RightButtonPressEvent, event);
			break;
		case Qt::RightButton:
			setCursor(impl->m_rotateCursor);
			iren->InvokeEvent(vtkCommand::LeftButtonPressEvent, event);
			break;
		default:
			// do nothing
			break;
		}
	} else {
		// the mouse press event is informed to the active data item.
		if (impl->m_activeDataItem != nullptr) {
			impl->m_activeDataItem->mousePressEvent(event, this);
		} else if (impl->m_interactive) {
			vtkRenderWindowInteractor* i = GetRenderWindow()->GetInteractor();
			vtkSmartPointer<vtkInteractorObserver> style = i->GetInteractorStyle();
			i->SetInteractorStyle(nullptr);
			QVTKWidget::mousePressEvent(event);
			i->SetInteractorStyle(style);
		}
	}
}

void VTKGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
	vtkRenderWindowInteractor* iren = NULL;
	if (this->mRenWin) {
		iren = this->mRenWin->GetInteractor();
	}
	if (!iren || !iren->GetEnabled()) {
		return;
	}

	// give vtk event information
	iren->SetEventInformationFlipY(event->x(), event->y(), 0, 0);

	// invoke appropriate vtk event
	this->unsetCursor();
	if (impl->m_activeDataItem != nullptr) {
		if (impl->m_isViewChanging) {
			if (impl->m_isRubberBandZooming) {
				iren->InvokeEvent(vtkCommand::LeftButtonReleaseEvent, event);
				impl->m_isRubberBandZooming = false;
				this->mRenWin->GetInteractor()->SetInteractorStyle(impl->m_styleBackUp);
			}
			impl->m_activeDataItem->viewOperationEnded(this);
			GraphicsWindowDataModel* m = dynamic_cast<GraphicsWindowDataModel*>(impl->m_model);
			m->viewOperationEndedGlobal();
		} else {
			impl->m_activeDataItem->mouseReleaseEvent(event, this);
		}
	} else if (impl->m_interactive) {
		vtkRenderWindowInteractor* i = GetRenderWindow()->GetInteractor();
		vtkSmartPointer<vtkInteractorObserver> style = i->GetInteractorStyle();
		i->SetInteractorStyle(nullptr);
		QVTKWidget::mouseReleaseEvent(event);
		i->SetInteractorStyle(style);
	}
	impl->m_isViewChanging = false;
	update2Ds();

	switch (event->button()) {
	case Qt::LeftButton:
		iren->InvokeEvent(vtkCommand::MiddleButtonReleaseEvent, event);
		break;
	case Qt::MidButton:
		iren->InvokeEvent(vtkCommand::RightButtonReleaseEvent, event);
		break;
	case Qt::RightButton:
		iren->InvokeEvent(vtkCommand::LeftButtonReleaseEvent, event);
		break;
	default:
		break;
	}
}

void VTKGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
	if (impl->m_isViewChanging) {
		// do the QVTKWidget implementation.
		QVTKWidget::mouseMoveEvent(event);
	} else if (impl->m_activeDataItem != nullptr) {
		impl->m_activeDataItem->mouseMoveEvent(event, this);
	} else if (impl->m_interactive) {
		vtkRenderWindowInteractor* i = GetRenderWindow()->GetInteractor();
		vtkSmartPointer<vtkInteractorObserver> style = i->GetInteractorStyle();
		i->SetInteractorStyle(nullptr);
		QVTKWidget::mouseMoveEvent(event);
		i->SetInteractorStyle(style);
	}
}

void VTKGraphicsView::wheelEvent(QWheelEvent* event)
{
	QVTKWidget::wheelEvent(event);
	if (impl->m_activeDataItem != nullptr) {
		impl->m_activeDataItem->wheelEvent(event, this);
	}
	impl->m_model->viewOperationEndedGlobal();

	update2Ds();
	render();
}

void VTKGraphicsView::standardKeyPressEvent(QKeyEvent* event)
{
	vtkRenderWindowInteractor* i = GetRenderWindow()->GetInteractor();
	vtkSmartPointer<vtkInteractorObserver> style = i->GetInteractorStyle();
	i->SetInteractorStyle(nullptr);
	QVTKWidget::keyPressEvent(event);
	i->SetInteractorStyle(style);
}

void VTKGraphicsView::standardKeyReleaseEvent(QKeyEvent* event)
{
	vtkRenderWindowInteractor* i = GetRenderWindow()->GetInteractor();
	vtkSmartPointer<vtkInteractorObserver> style = i->GetInteractorStyle();
	i->SetInteractorStyle(nullptr);
	QVTKWidget::keyReleaseEvent(event);
	i->SetInteractorStyle(style);
}

void VTKGraphicsView::standardMouseDoubleClickEvent(QMouseEvent* event)
{
	vtkRenderWindowInteractor* i = GetRenderWindow()->GetInteractor();
	vtkSmartPointer<vtkInteractorObserver> style = i->GetInteractorStyle();
	i->SetInteractorStyle(nullptr);
	QVTKWidget::mouseDoubleClickEvent(event);
	i->SetInteractorStyle(style);
}

void VTKGraphicsView::standardMousePressEvent(QMouseEvent* event)
{
	vtkRenderWindowInteractor* i = GetRenderWindow()->GetInteractor();
	vtkSmartPointer<vtkInteractorObserver> style = i->GetInteractorStyle();
	i->SetInteractorStyle(nullptr);
	QVTKWidget::mousePressEvent(event);
	i->SetInteractorStyle(style);
}

void VTKGraphicsView::standardMouseReleaseEvent(QMouseEvent* event)
{
	vtkRenderWindowInteractor* i = GetRenderWindow()->GetInteractor();
	vtkSmartPointer<vtkInteractorObserver> style = i->GetInteractorStyle();
	i->SetInteractorStyle(nullptr);
	QVTKWidget::mouseReleaseEvent(event);
	i->SetInteractorStyle(style);
}

void VTKGraphicsView::standardMouseMoveEvent(QMouseEvent* event)
{
	vtkRenderWindowInteractor* i = GetRenderWindow()->GetInteractor();
	vtkSmartPointer<vtkInteractorObserver> style = i->GetInteractorStyle();
	i->SetInteractorStyle(nullptr);
	QVTKWidget::mouseMoveEvent(event);
	i->SetInteractorStyle(style);
}

void VTKGraphicsView::standardWheelEvent(QWheelEvent* event)
{
	vtkRenderWindowInteractor* i = GetRenderWindow()->GetInteractor();
	vtkSmartPointer<vtkInteractorObserver> style = i->GetInteractorStyle();
	i->SetInteractorStyle(nullptr);
	QVTKWidget::wheelEvent(event);
	i->SetInteractorStyle(style);
}

vtkRenderer* VTKGraphicsView::mainRenderer() const
{
	return impl->m_mainRenderer;
}

vtkCamera* VTKGraphicsView::camera() const
{
	return impl->m_camera;
}

void VTKGraphicsView::render()
{
	impl->m_mainRenderer->GetRenderWindow()->Render();
}

void VTKGraphicsView::restoreUpdateRate()
{
	vtkRenderWindow* w = GetRenderWindow();
	w->SetDesiredUpdateRate(w->GetInteractor()->GetStillUpdateRate());
}

void VTKGraphicsView::setInteractive(bool interactive)
{
	impl->m_interactive = interactive;
}

void VTKGraphicsView::setupCamera()
{
	double pos[3];
	double range[2];
	vtkCamera* activeCamera = impl->m_mainRenderer->GetActiveCamera();
	activeCamera->GetPosition(pos);
	camera()->SetPosition(pos);
	activeCamera->GetFocalPoint(pos);
	impl->m_camera->SetFocalPoint(pos);
	activeCamera->GetClippingRange(range);
	impl->m_camera->SetClippingRange(range);
	impl->m_camera->ParallelProjectionOn();
	impl->m_camera->SetParallelScale(activeCamera->GetParallelScale());
	impl->m_camera->SetDistance(activeCamera->GetDistance());
	impl->m_camera->SetRoll(activeCamera->GetRoll());
}

VTKGraphicsViewArbitraryMove::VTKGraphicsViewArbitraryMove(vtkCamera* camera, VTKGraphicsView* view)
	: QUndoCommand(QObject::tr("Arbitrary Move"))
{
	double pos[3];
	double range[2];
	double viewup[3];

	m_oldCamera = vtkCamera::New();
	view->camera()->GetPosition(pos);
	m_oldCamera->SetPosition(pos);
	view->camera()->GetFocalPoint(pos);
	m_oldCamera->SetFocalPoint(pos);
	view->camera()->GetClippingRange(range);
	m_oldCamera->SetClippingRange(range);
	view->camera()->GetViewUp(viewup);
	m_oldCamera->SetViewUp(viewup);
	m_oldCamera->ParallelProjectionOn();
	m_oldCamera->SetParallelScale(view->camera()->GetParallelScale());
	m_oldCamera->SetDistance(view->camera()->GetDistance());
	m_oldCamera->SetRoll(view->camera()->GetRoll());

	m_newCamera = vtkCamera::New();
	camera->GetPosition(pos);
	m_newCamera->SetPosition(pos);
	camera->GetFocalPoint(pos);
	m_newCamera->SetFocalPoint(pos);
	camera->GetClippingRange(range);
	m_newCamera->SetClippingRange(range);
	view->camera()->GetViewUp(viewup);
	m_newCamera->SetViewUp(viewup);
	m_newCamera->ParallelProjectionOn();
	m_newCamera->SetParallelScale(camera->GetParallelScale());
	m_newCamera->SetDistance(camera->GetDistance());
	m_newCamera->SetRoll(camera->GetRoll());

	m_view = view;
}

VTKGraphicsViewArbitraryMove::~VTKGraphicsViewArbitraryMove()
{
	m_oldCamera->Delete();
	m_newCamera->Delete();
}

void VTKGraphicsViewArbitraryMove::undo()
{
	m_view->mainRenderer()->SetActiveCamera(m_oldCamera);
	m_view->mainRenderer()->GetRenderWindow()->Render();
}
void VTKGraphicsViewArbitraryMove::redo()
{
	m_view->mainRenderer()->SetActiveCamera(m_newCamera);
	m_view->model()->viewOperationEndedGlobal();
	GraphicsWindowDataModel* model = dynamic_cast<GraphicsWindowDataModel*>(m_view->model());
	if (model != nullptr) {
		model->update2Ds();
	}
	m_view->mainRenderer()->GetRenderWindow()->Render();
}

QImage VTKGraphicsView::getImage()
{
	vtkRenderWindow* rw = GetRenderWindow();
//	rw->OffScreenRenderingOn();
	rw->Render();

	int w = this->width();
	int h = this->height();
	QImage img(w, h, QImage::Format_RGB32);
	vtkUnsignedCharArray* pixels = vtkUnsignedCharArray::New();
	pixels->SetArray(img.bits(), w*h*4, 1);

	rw->GetRGBACharPixelData(0, 0, w-1, h-1, 1, pixels);
	pixels->Delete();
	img = img.rgbSwapped();
	img = img.mirrored();

//	rw->OffScreenRenderingOff();
	return img;
}

void VTKGraphicsView::update2Ds()
{
	if (impl->m_model == nullptr) { return; }
	GraphicsWindowDataModel* model = dynamic_cast<GraphicsWindowDataModel*>(impl->m_model);
	if (model == nullptr) {return;}
	model->update2Ds();
}

void VTKGraphicsView::resizeEvent(QResizeEvent* event)
{
	QSize size = event->size();
	impl->m_logoActor->SetPosition(size.width() - impl->m_logoImage.width() - 10, 5);
	impl->m_logoActor->Modified();
	impl->m_model->handleResize();
	QVTKWidget::resizeEvent(event);
}

void VTKGraphicsView::cameraFit()
{
	fitInView();
	impl->m_model->viewOperationEndedGlobal();
	render();
}

void VTKGraphicsView::cameraZoomIn()
{
	scale(1.2);
	impl->m_model->viewOperationEndedGlobal();
	render();
}

void VTKGraphicsView::cameraZoomOut()
{
	scale (1 / 1.2);
	impl->m_model->viewOperationEndedGlobal();
	render();
}

void VTKGraphicsView::cameraMoveLeft()
{
	moveCenter(moveWidth(), 0);
}

void VTKGraphicsView::cameraMoveRight()
{
	moveCenter(- moveWidth(), 0);
}

void VTKGraphicsView::cameraMoveUp()
{
	moveCenter(0, - moveWidth());
}

void VTKGraphicsView::cameraMoveDown()
{
	moveCenter(0, moveWidth());
}


GraphicsWindowSimpleDataModel* VTKGraphicsView::model() const
{
	return impl->m_model;
}

void VTKGraphicsView::moveCenter(int x, int y)
{
	translate(x, y);
	render();
}

int VTKGraphicsView::moveWidth()
{
	return 1;
}
