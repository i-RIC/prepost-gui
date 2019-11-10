#include "distancemeasuredataitem.h"
#include "distancemeasurepropertydialog.h"
#include "private/distancemeasuredataitem_definecommand.h"
#include "private/distancemeasuredataitem_impl.h"
#include "private/distancemeasuredataitem_movevertexcommand.h"
#include "private/distancemeasuredataitem_redefinecommand.h"
#include "private/distancemeasuredataitem_setsettingcommand.h"
#include "private/distancemeasuredataitem_translatecommand.h"

#include <guicore/datamodel/graphicswindowdatamodel.h>
#include <guicore/datamodel/vtk2dgraphicsview.h>
#include <misc/qundocommandhelper.h>

#include <misc/iricundostack.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QAction>
#include <QDomNode>
#include <QMainWindow>
#include <QMenu>
#include <QMouseEvent>
#include <QStandardItem>
#include <QUndoCommand>
#include <QXmlStreamWriter>

#include <vtkActor2DCollection.h>
#include <vtkDataSetMapper.h>
#include <vtkLabeledDataMapper.h>
#include <vtkLine.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkTextProperty.h>
#include <vtkVertex.h>

#define LABEL "Label"

DistanceMeasureDataItem::Impl::Impl(DistanceMeasureDataItem* parent) :
	m_dragPointTarget {0},
	m_mouseEventMode {meBeforeDefining},
	m_shiftPressed {false},
	m_setting {}
{
	m_movePointPixmap = QPixmap(":/libs/guibase/images/cursorOpenHandPoint.png");
	m_movePointCursor = QCursor(m_movePointPixmap);

	m_rightClickingMenu = new QMenu(parent->mainWindow());
	m_propertyAction = new QAction(DistanceMeasureDataItem::tr("Property"), m_rightClickingMenu);
	m_propertyAction->setIcon(QIcon(":/libs/guibase/images/iconProperty.png"));
	m_redefineAction = new QAction(DistanceMeasureDataItem::tr("Redefine"), m_rightClickingMenu);

	m_rightClickingMenu->addAction(m_redefineAction);
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_propertyAction);
}

DistanceMeasureDataItem::Impl::~Impl()
{
	delete m_rightClickingMenu;
}

// public interfaces

DistanceMeasureDataItem::DistanceMeasureDataItem(const QString& name, GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem {name, QIcon(":/libs/guibase/images/iconPaper.png"), parent},
	impl {new Impl(this)}
{
	setupStandardItem(Checked, NotReorderable, Deletable);

	connect(impl->m_propertyAction, SIGNAL(triggered()), this, SLOT(showPropDialog()));
	connect(impl->m_redefineAction, SIGNAL(triggered()), this, SLOT(redefine()));

	setupActors();
}

DistanceMeasureDataItem::~DistanceMeasureDataItem()
{
	vtkRenderer* r = renderer();
	r->RemoveActor(impl->m_lineActor.pointsActor());
	r->RemoveActor(impl->m_lineActor.lineActor());
	r->RemoveActor2D(impl->m_labelActor.actor());

	delete impl;
}

void DistanceMeasureDataItem::setupActors()
{
	vtkProperty* pointsProp = impl->m_lineActor.pointsActor()->GetProperty();
	pointsProp->SetLighting(false);
	pointsProp->SetPointSize(7);
	pointsProp->SetColor(0, 0, 0);
	impl->m_lineActor.pointsActor()->SetVisibility(0);
	renderer()->AddActor(impl->m_lineActor.pointsActor());

	vtkProperty* lineProp = impl->m_lineActor.lineActor()->GetProperty();
	lineProp->SetLighting(false);
	lineProp->SetLineWidth(1);
	lineProp->SetColor(0, 0, 0);
	impl->m_lineActor.lineActor()->SetVisibility(0);
	renderer()->AddActor(impl->m_lineActor.lineActor());

	impl->m_labelActor.actor()->GetProperty()->SetColor(0, 0, 0);
	impl->m_labelActor.actor()->SetVisibility(0);
	renderer()->AddActor2D(impl->m_labelActor.actor());
}

void DistanceMeasureDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void DistanceMeasureDataItem::assignActorZValues(const ZDepthRange& range)
{
	impl->m_lineActor.pointsActor()->SetPosition(0, 0, range.max());
	impl->m_lineActor.lineActor()->SetPosition(0, 0, range.max());
}

void DistanceMeasureDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	VTK2DGraphicsView* v2 = dynamic_cast<VTK2DGraphicsView*>(v);
	double x, y;
	x = event->x();
	y = event->y();
	v2->viewportToWorld(x, y);
	QPointF globalPos(x, y);
	if (impl->m_mouseEventMode == Impl::meDefining) {
		QPointF newv2 = updatePoint2ByShift(impl->m_setting.point1, globalPos);
		iRICUndoStack::instance().push(new DefineCommand(impl->m_setting.point1, newv2, false, this));
	} else if (impl->m_mouseEventMode == Impl::meMoveVertex) {
		QPointF otherp;
		if (impl->m_dragPointTarget == 1) {
			otherp = impl->m_setting.point2;
		} else {
			otherp = impl->m_setting.point1;
		}
		QPointF newv = updatePoint2ByShift(otherp, globalPos);
		iRICUndoStack::instance().push(new MoveVertexCommand(impl->m_dragPointTarget, newv, false, this));
	} else if (impl->m_mouseEventMode == Impl::meTranslate) {
		x = impl->m_dragStartPoint.x();
		y = impl->m_dragStartPoint.y();
		v2->viewportToWorld(x, y);
		QPointF startGlobalPos(x, y);
		QPointF offset = globalPos - startGlobalPos;

		pushRenderCommand(new TranslateCommand(impl->m_dragStartPoint1 + offset, impl->m_dragStartPoint2 + offset, false, this), this);
	} else {
		updateMouseEventMode(globalPos, v);
		updateMouseCursor();
	}
}

void DistanceMeasureDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	impl->m_dragStartPoint = QPoint(event->x(), event->y());
	if (event->button() != Qt::LeftButton) {return;}
	VTK2DGraphicsView* v2 = dynamic_cast<VTK2DGraphicsView*>(v);
	double x, y;
	x = event->x();
	y = event->y();
	v2->viewportToWorld(x, y);
	QPointF tmpv(x, y);

	if (impl->m_mouseEventMode == Impl::meBeforeDefining) {
		iRICUndoStack::instance().push(new DefineCommand(tmpv, tmpv, false, this));
	} else if (impl->m_mouseEventMode == Impl::meMoveVertexPrepare) {
		iRICUndoStack::instance().push(new MoveVertexCommand(impl->m_dragPointTarget, tmpv, false, this));
	} else if (impl->m_mouseEventMode == Impl::meTranslatePrepare) {
		impl->m_dragStartPoint1 = impl->m_setting.point1;
		impl->m_dragStartPoint2 = impl->m_setting.point2;
		pushRenderCommand(new TranslateCommand(impl->m_setting.point1, impl->m_setting.point2, false, this), this);
	}
}

void DistanceMeasureDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (impl->m_mouseEventMode == Impl::meDefining && event->button() == Qt::LeftButton) {
		iRICUndoStack::instance().push(new DefineCommand(impl->m_setting.point1, impl->m_setting.point2, true, this));
	} else if (impl->m_mouseEventMode == Impl::meMoveVertex && event->button() == Qt::LeftButton) {
		QPointF tmpv;
		if (impl->m_dragPointTarget == 1) {
			tmpv = impl->m_setting.point1;
		} else if (impl->m_dragPointTarget == 2) {
			tmpv = impl->m_setting.point2;
		}
		iRICUndoStack::instance().push(new MoveVertexCommand(impl->m_dragPointTarget, tmpv, true, this));
	} else if (impl->m_mouseEventMode == Impl::meTranslate && event->button() == Qt::LeftButton) {
		VTK2DGraphicsView* v2 = dynamic_cast<VTK2DGraphicsView*>(v);
		double x, y;
		x = event->x();
		y = event->y();
		v2->viewportToWorld(x, y);
		QPointF globalPos(x, y);
		x = impl->m_dragStartPoint.x();
		y = impl->m_dragStartPoint.y();
		v2->viewportToWorld(x, y);
		QPointF startGlobalPos(x, y);
		QPointF offset = globalPos - startGlobalPos;

		pushRenderCommand(new TranslateCommand(impl->m_dragStartPoint1 + offset, impl->m_dragStartPoint2 + offset, true, this), this);
	}
	if (event->button() == Qt::RightButton) {
		if (iRIC::isNear(impl->m_dragStartPoint, event->pos())) {
			impl->m_redefineAction->setEnabled(impl->m_setting.defined);

			impl->m_rightClickingMenu->move(event->globalPos());
			impl->m_rightClickingMenu->show();
		}
	}
}

void DistanceMeasureDataItem::keyPressEvent(QKeyEvent* event, VTKGraphicsView* /*v*/)
{
	if (event->key() == Qt::Key_Shift) {
		impl->m_shiftPressed = true;
	}
	if (impl->m_mouseEventMode == Impl::meDefining && event->key() == Qt::Key_Shift) {
		QPointF newv2 = updatePoint2ByShift(impl->m_setting.point1, impl->m_setting.point2);
		iRICUndoStack::instance().push(new DefineCommand(impl->m_setting.point1, newv2, false, this));
	} else if (impl->m_mouseEventMode == Impl::meMoveVertex && event->key() == Qt::Key_Shift) {
		QPointF movep;
		QPointF otherp;
		if (impl->m_dragPointTarget == 1) {
			movep = impl->m_setting.point1;
			otherp = impl->m_setting.point2;
		} else {
			movep = impl->m_setting.point2;
			otherp = impl->m_setting.point1;
		}
		QPointF newv = updatePoint2ByShift(otherp, movep);
		iRICUndoStack::instance().push(new MoveVertexCommand(impl->m_dragPointTarget, newv, false, this));
	}
}
void DistanceMeasureDataItem::keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* /*v*/)
{
	if (event->key() == Qt::Key_Shift) {
		impl->m_shiftPressed = false;
	}
}

void DistanceMeasureDataItem::setPoints(const QPointF& v1, const QPointF& v2)
{
	impl->m_setting.point1 = v1;
	impl->m_setting.point2 = v2;
	updateActorSettings();
	renderGraphicsView();
}

void DistanceMeasureDataItem::updateActorSettings()
{
	std::vector<QPointF> line;
	line.push_back(impl->m_setting.point1);
	line.push_back(impl->m_setting.point2);
	impl->m_lineActor.setLine(line);

	QPointF mid = (impl->m_setting.point1 + impl->m_setting.point2) * 0.5;
	impl->m_labelActor.setPosition(mid);

	QString label;
	if (impl->m_setting.labelMode == DistanceMeasureSetting::Auto) {
		label = autoLabel();
	} else {
		label = impl->m_setting.customLabel;
	}
	impl->m_labelActor.setLabel(iRIC::toStr(label));
	impl->m_labelActor.setLabelPosition(impl->m_setting.labelPosition);

	vtkTextProperty* txtProp = impl->m_labelActor.labelTextProperty();
	impl->m_setting.labelFontSetting.applySetting(txtProp);
	txtProp->SetColor(impl->m_setting.color);

	impl->m_lineActor.pointsActor()->GetProperty()->SetColor(impl->m_setting.color);
	impl->m_lineActor.pointsActor()->GetProperty()->SetPointSize(impl->m_setting.markerSize);
	impl->m_lineActor.lineActor()->GetProperty()->SetColor(impl->m_setting.color);

	impl->m_lineActor.pointsActor()->VisibilityOff();
	impl->m_lineActor.lineActor()->VisibilityOff();
	impl->m_labelActor.actor()->VisibilityOff();
	actorCollection()->RemoveItem(impl->m_lineActor.pointsActor());
	actorCollection()->RemoveItem(impl->m_lineActor.lineActor());
	actor2DCollection()->RemoveItem(impl->m_labelActor.actor());

	if (impl->m_setting.defined) {
		if (impl->m_setting.showMarkers) {
			actorCollection()->AddItem(impl->m_lineActor.pointsActor());
		}
		actorCollection()->AddItem(impl->m_lineActor.lineActor());
		if (impl->m_setting.showLabel) {
			actor2DCollection()->AddItem(impl->m_labelActor.actor());
		}
	}

	updateVisibilityWithoutRendering();
}

QDialog* DistanceMeasureDataItem::propertyDialog(QWidget* parent)
{
	QVector2D v1, v2;

	std::vector<QPointF> line = impl->m_lineActor.line();
	if (line.size() < 2) {
		return nullptr;
	}
	v1 = QVector2D(line.at(0).x(), line.at(0).y());
	v2 = QVector2D(line.at(1).x(), line.at(1).y());

	auto dialog = new DistanceMeasurePropertyDialog(parent);
	dialog->setName(m_standardItem->text().trimmed());
	dialog->setSetting(impl->m_setting);

	return dialog;
}

void DistanceMeasureDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	auto dialog = dynamic_cast<DistanceMeasurePropertyDialog*>(propDialog);
	iRICUndoStack::instance().push(new SetSettingCommand(dialog->name(), dialog->setting(), this));
}

QString DistanceMeasureDataItem::autoLabel() const
{
	return QString::number(iRIC::length(impl->m_setting.point2 - impl->m_setting.point1));
}

void DistanceMeasureDataItem::showPropDialog()
{
	showPropertyDialog();
}

QPointF DistanceMeasureDataItem::updatePoint2ByShift(const QPointF& v1, const QPointF& v2)
{
	if (! impl->m_shiftPressed) {return v2;}
	double xdist = qAbs(v2.x() - v1.x());
	double ydist = qAbs(v2.y() - v1.y());
	if (xdist >= ydist) {
		return QPointF(v2.x(), v1.y());
	} else {
		return QPointF(v1.x(), v2.y());
	}
}

void DistanceMeasureDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_isCommandExecuting = true;
	m_standardItem->setText(node.toElement().attribute("name"));
	m_isCommandExecuting = false;
	impl->m_setting.load(node);

	if (impl->m_setting.defined) {
		impl->m_mouseEventMode = Impl::meNormal;
	} else {
		impl->m_mouseEventMode = Impl::meBeforeDefining;
	}

	updateActorSettings();
}

void DistanceMeasureDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", standardItem()->text());
	impl->m_setting.save(writer);
}

void DistanceMeasureDataItem::addCustomMenuItems(QMenu* menu)
{
	impl->m_redefineAction->setEnabled(impl->m_setting.defined);
	menu->addAction(impl->m_redefineAction);
}

void DistanceMeasureDataItem::redefine()
{
	pushRenderCommand(new RedefineCommand(this), this);
}

void DistanceMeasureDataItem::informSelection(VTKGraphicsView* v)
{
	updateMouseCursor(v);
}

void DistanceMeasureDataItem::informDeselection(VTKGraphicsView* v)
{
	v->setCursor(Qt::ArrowCursor);
}

void DistanceMeasureDataItem::updateMouseCursor()
{
	updateMouseCursor(dataModel()->graphicsView());
}

void DistanceMeasureDataItem::updateMouseCursor(VTKGraphicsView* v)
{
	switch (impl->m_mouseEventMode) {
	case Impl::meNormal:
		v->setCursor(Qt::ArrowCursor);
		break;
	case Impl::meBeforeDefining:
	case Impl::meDefining:
		v->setCursor(Qt::CrossCursor);
		break;
	case Impl::meTranslate:
		v->setCursor(Qt::ClosedHandCursor);
		break;
	case Impl::meTranslatePrepare:
		v->setCursor(Qt::OpenHandCursor);
		break;
	case Impl::meMoveVertex:
		v->setCursor(Qt::ClosedHandCursor);
		break;
	case Impl::meMoveVertexPrepare:
		v->setCursor(impl->m_movePointCursor);
		break;
	}
}

void DistanceMeasureDataItem::updateMouseEventMode(const QPointF& v, VTKGraphicsView* view)
{
	VTK2DGraphicsView* view2 = dynamic_cast<VTK2DGraphicsView*>(view);
	switch (impl->m_mouseEventMode) {
	case Impl::meNormal:
	case Impl::meMoveVertexPrepare:
	case Impl::meTranslatePrepare:
		if (iRIC::length(impl->m_setting.point1 - v) < view2->stdRadius(iRIC::nearRadius())) {
			impl->m_mouseEventMode = Impl::meMoveVertexPrepare;
			impl->m_dragPointTarget = 1;
		} else if (iRIC::length(impl->m_setting.point2 - v) < view2->stdRadius(iRIC::nearRadius())) {
			impl->m_mouseEventMode = Impl::meMoveVertexPrepare;
			impl->m_dragPointTarget = 2;
		} else {
			QPointF horizontal = impl->m_setting.point2 - impl->m_setting.point1;
			QPointF vertical = iRIC::normalize(horizontal);
			iRIC::rotateVector90(vertical);
			double width = view2->stdRadius(iRIC::nearRadius());
			vertical *= width;
			QPointF posv = impl->m_setting.point1 - vertical * 0.5;
			if (iRIC::isInsideParallelogram(v, posv, horizontal, vertical)) {
				impl->m_mouseEventMode = Impl::meTranslatePrepare;
			} else {
				impl->m_mouseEventMode = Impl::meNormal;
			}
		}

		break;
	case Impl::meBeforeDefining:
	case Impl::meDefining:
	case Impl::meMoveVertex:
	case Impl::meTranslate:
		// do nothing
		break;
	}
}

