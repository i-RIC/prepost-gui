#define _USE_MATH_DEFINES
#include <cmath>

#include "vtkgraphicsview.h"
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

VTKGraphicsView::VTKGraphicsView(QWidget* parent)
	: QVTKWidget(parent)
{
	m_isViewChanging = false;
	m_interactive = false;
	m_isRubberBandZooming = false;
	m_rubberBarStyle = vtkInteractorStyleRubberBandZoom::New();

	// Set cursors for mouse view change events.
	m_zoomPixmap = QPixmap(":/libs/guibase/images/cursorZoom.png");
	m_rotatePixmap = QPixmap(":/libs/guibase/images/cursorRotate.png");
	m_movePixmap = QPixmap(":/libs/guibase/images/cursorMove.png");

	m_zoomCursor = QCursor(m_zoomPixmap);
	m_rotateCursor = QCursor(m_rotatePixmap);
	m_moveCursor = QCursor(m_movePixmap);

	m_mainRenderer = vtkRenderer::New();
	m_mainRenderer->SetBackground(1.0, 1.0, 1.0);

	m_logoImage = QImage(":/libs/guicore/images/logo.png");
	vtkSmartPointer<vtkQImageToImageSource> imgToImg = vtkSmartPointer<vtkQImageToImageSource>::New();
	imgToImg->SetQImage(&m_logoImage);
	vtkSmartPointer<vtkImageMapper> imgMapper = vtkSmartPointer<vtkImageMapper>::New();
	imgMapper->SetColorWindow(255);
	imgMapper->SetColorLevel(127.5);
	imgMapper->SetInputConnection(imgToImg->GetOutputPort());

	// setup logo mapper
	m_logoActor = vtkActor2D::New();
	m_logoActor->SetMapper(imgMapper);
	m_mainRenderer->AddActor2D(m_logoActor);

	vtkRenderWindow* renderWindow = GetRenderWindow();
	renderWindow->SetMultiSamples(0);
	renderWindow->AddRenderer(m_mainRenderer);
	renderWindow->SetStereoTypeToDresden();

	m_camera = vtkCamera::New();

	// Set the camera to be paralell projection, because it suits
	// for 2D graphics.
	m_mainRenderer->GetActiveCamera()->ParallelProjectionOn();
}

VTKGraphicsView::~VTKGraphicsView()
{
	m_mainRenderer->Delete();
	m_logoActor->Delete();
	m_rubberBarStyle->Delete();
}

void VTKGraphicsView::scale(double s)
{
	setupCamera();
	m_mainRenderer->GetActiveCamera()->Zoom(s);
	update2Ds();
}

void VTKGraphicsView::keyPressEvent(QKeyEvent* event)
{
	if (m_activeDataItem != nullptr) {
		m_activeDataItem->keyPressEvent(event, this);
	} else if (m_interactive) {
		QVTKWidget::keyPressEvent(event);
	}
}

void VTKGraphicsView::keyReleaseEvent(QKeyEvent* event)
{
	if (m_activeDataItem != nullptr) {
		m_activeDataItem->keyReleaseEvent(event, this);
	} else if (m_interactive) {
		QVTKWidget::keyReleaseEvent(event);
	}
}

void VTKGraphicsView::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (m_activeDataItem != nullptr) {
		m_activeDataItem->mouseDoubleClickEvent(event, this);
	} else if (m_interactive) {
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
		m_isViewChanging = true;

		// give interactor the event information
		iren->SetEventInformationFlipY(event->x(), event->y(), 0, 0, 0, event->type() == QEvent::MouseButtonDblClick ? 1 : 0);
		switch (event->button()) {
		case Qt::LeftButton:
			if ((event->modifiers() & Qt::ShiftModifier) != 0) {
				m_styleBackUp = iren->GetInteractorStyle();
				iren->SetInteractorStyle(m_rubberBarStyle);
				iren->InvokeEvent(vtkCommand::LeftButtonPressEvent, event);
				m_isRubberBandZooming = true;
			} else {
				setCursor(m_moveCursor);
				iren->InvokeEvent(vtkCommand::MiddleButtonPressEvent, event);
			}
			break;
		case Qt::MidButton:
			setCursor(m_zoomCursor);
			iren->InvokeEvent(vtkCommand::RightButtonPressEvent, event);
			break;
		case Qt::RightButton:
			setCursor(m_rotateCursor);
			iren->InvokeEvent(vtkCommand::LeftButtonPressEvent, event);
			break;
		default:
			// do nothing
			break;
		}
	} else {
		// the mouse press event is informed to the active data item.
		if (m_activeDataItem != nullptr) {
			m_activeDataItem->mousePressEvent(event, this);
		} else if (m_interactive) {
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
	if (m_activeDataItem != nullptr) {
		if (m_isViewChanging) {
			if (m_isRubberBandZooming) {
				iren->InvokeEvent(vtkCommand::LeftButtonReleaseEvent, event);
				m_isRubberBandZooming = false;
				this->mRenWin->GetInteractor()->SetInteractorStyle(m_styleBackUp);
			}
			m_activeDataItem->viewOperationEnded(this);
			GraphicsWindowDataModel* m = dynamic_cast<GraphicsWindowDataModel*>(m_model);
			m->viewOperationEndedGlobal();
		} else {
			m_activeDataItem->mouseReleaseEvent(event, this);
		}
	} else if (m_interactive) {
		vtkRenderWindowInteractor* i = GetRenderWindow()->GetInteractor();
		vtkSmartPointer<vtkInteractorObserver> style = i->GetInteractorStyle();
		i->SetInteractorStyle(nullptr);
		QVTKWidget::mouseReleaseEvent(event);
		i->SetInteractorStyle(style);
	}
	m_isViewChanging = false;
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
	if (m_isViewChanging) {
		// do the QVTKWidget implementation.
		QVTKWidget::mouseMoveEvent(event);
	} else if (m_activeDataItem != nullptr) {
		m_activeDataItem->mouseMoveEvent(event, this);
	} else if (m_interactive) {
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
	if (m_activeDataItem != nullptr) {
		m_activeDataItem->wheelEvent(event, this);
	}
	m_model->viewOperationEndedGlobal();

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

void VTKGraphicsView::render()
{
	m_mainRenderer->GetRenderWindow()->Render();
}

void VTKGraphicsView::restoreUpdateRate()
{
	vtkRenderWindow* w = GetRenderWindow();
	w->SetDesiredUpdateRate(w->GetInteractor()->GetStillUpdateRate());
}

void VTKGraphicsView::setupCamera()
{
	double pos[3];
	double range[2];
	vtkCamera* activeCamera = m_mainRenderer->GetActiveCamera();
	activeCamera->GetPosition(pos);
	m_camera->SetPosition(pos);
	activeCamera->GetFocalPoint(pos);
	m_camera->SetFocalPoint(pos);
	activeCamera->GetClippingRange(range);
	m_camera->SetClippingRange(range);
	m_camera->ParallelProjectionOn();
	m_camera->SetParallelScale(activeCamera->GetParallelScale());
	m_camera->SetDistance(activeCamera->GetDistance());
	m_camera->SetRoll(activeCamera->GetRoll());
}

VTKGraphicsViewArbitraryMove::VTKGraphicsViewArbitraryMove(vtkCamera* camera, VTKGraphicsView* view)
	: QUndoCommand(QObject::tr("Arbitrary Move"))
{
	double pos[3];
	double range[2];
	double viewup[3];

	m_oldCamera = vtkCamera::New();
	view->m_camera->GetPosition(pos);
	m_oldCamera->SetPosition(pos);
	view->m_camera->GetFocalPoint(pos);
	m_oldCamera->SetFocalPoint(pos);
	view->m_camera->GetClippingRange(range);
	m_oldCamera->SetClippingRange(range);
	view->m_camera->GetViewUp(viewup);
	m_oldCamera->SetViewUp(viewup);
	m_oldCamera->ParallelProjectionOn();
	m_oldCamera->SetParallelScale(view->m_camera->GetParallelScale());
	m_oldCamera->SetDistance(view->m_camera->GetDistance());
	m_oldCamera->SetRoll(view->m_camera->GetRoll());

	m_newCamera = vtkCamera::New();
	camera->GetPosition(pos);
	m_newCamera->SetPosition(pos);
	camera->GetFocalPoint(pos);
	m_newCamera->SetFocalPoint(pos);
	camera->GetClippingRange(range);
	m_newCamera->SetClippingRange(range);
	view->m_camera->GetViewUp(viewup);
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
	m_view->m_model->viewOperationEndedGlobal();
	GraphicsWindowDataModel* model = dynamic_cast<GraphicsWindowDataModel*>(m_view->m_model);
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
	if (m_model == nullptr) {return;}
	GraphicsWindowDataModel* model = dynamic_cast<GraphicsWindowDataModel*>(m_model);
	if (model == nullptr) {return;}
	model->update2Ds();
}

void VTKGraphicsView::resizeEvent(QResizeEvent* event)
{
	QSize size = event->size();
	m_logoActor->SetPosition(size.width() - m_logoImage.width() - 10, 5);
	m_logoActor->Modified();
	QVTKWidget::resizeEvent(event);
}

void VTKGraphicsView::cameraFit()
{
	fitInView();
	m_model->viewOperationEndedGlobal();
	render();
}

void VTKGraphicsView::cameraZoomIn()
{
	scale(1.2);
	m_model->viewOperationEndedGlobal();
	render();
}

void VTKGraphicsView::cameraZoomOut()
{
	scale (1 / 1.2);
	m_model->viewOperationEndedGlobal();
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

void VTKGraphicsView::moveCenter(int x, int y)
{
	translate(x, y);
}

int VTKGraphicsView::moveWidth()
{
	return 1;
}
