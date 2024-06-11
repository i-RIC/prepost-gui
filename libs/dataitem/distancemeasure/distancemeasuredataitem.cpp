#include "distancemeasuredataitem.h"
#include "private/distancemeasuredataitem_definecommand.h"
#include "private/distancemeasuredataitem_editwidget.h"
#include "private/distancemeasuredataitem_impl.h"
#include "private/distancemeasuredataitem_movevertexcommand.h"
#include "private/distancemeasuredataitem_redefinecommand.h"
#include "private/distancemeasuredataitem_translatecommand.h"

#include <guicore/datamodel/graphicswindowdatamodel.h>
#include <guicore/datamodel/vtk2dgraphicsview.h>
#include <misc/qundocommandhelper.h>

#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
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
	m_propertyAction->setIcon(QIcon(":/libs/guibase/images/iconProperty.svg"));
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
	GraphicsWindowDataItem {name, QIcon(":/libs/guibase/images/iconPaper.svg"), parent},
	impl {new Impl(this)}
{
	setupStandardItem(Checked, NotReorderable, Deletable);

	connect(impl->m_propertyAction, &QAction::triggered, this, &DistanceMeasureDataItem::showPropertyDialog);
	connect(impl->m_redefineAction, &QAction::triggered, this, &DistanceMeasureDataItem::redefine);

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
	auto v = dataModel()->graphicsView();

	auto pointsProp = impl->m_lineActor.pointsActor()->GetProperty();
	pointsProp->SetLighting(false);
	pointsProp->SetPointSize(7 * v->devicePixelRatioF());
	pointsProp->SetColor(0, 0, 0);
	impl->m_lineActor.pointsActor()->SetVisibility(0);
	renderer()->AddActor(impl->m_lineActor.pointsActor());

	auto lineProp = impl->m_lineActor.lineActor()->GetProperty();
	lineProp->SetLighting(false);
	lineProp->SetLineWidth(1 * v->devicePixelRatioF());
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
	auto globalPos = v2->viewportToWorld(event->pos());

	if (impl->m_mouseEventMode == Impl::meDefining) {
		QPointF newv2 = updatePoint2ByShift(impl->m_setting.point1, globalPos);
		emit endPointEdited(newv2);
		pushUpdateActorSettingCommand(new DefineCommand(impl->m_setting.point1, newv2, false, this), this);
	} else if (impl->m_mouseEventMode == Impl::meMoveVertex) {
		QPointF otherp;
		if (impl->m_dragPointTarget == 1) {
			otherp = impl->m_setting.point2;
		} else {
			otherp = impl->m_setting.point1;
		}
		QPointF newv = updatePoint2ByShift(otherp, globalPos);
		if (impl->m_dragPointTarget == 1) {
			emit startPointEdited(newv);
		} else if (impl->m_dragPointTarget == 2) {
			emit endPointEdited(newv);
		}
		pushUpdateActorSettingCommand(new MoveVertexCommand(impl->m_dragPointTarget, newv, false, this), this);
	} else if (impl->m_mouseEventMode == Impl::meTranslate) {
		auto startGlobalPos = v2->viewportToWorld(impl->m_dragStartPoint);
		QPointF offset = globalPos - startGlobalPos;
		emit startPointEdited(impl->m_dragStartPoint1 + offset);
		emit endPointEdited(impl->m_dragStartPoint2 + offset);
		pushRenderCommand(new TranslateCommand(impl->m_dragStartPoint1 + offset, impl->m_dragStartPoint2 + offset, false, this), this);
	} else {
		updateMouseEventMode(globalPos, v);
		updateMouseCursor();
	}
}

void DistanceMeasureDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	impl->m_dragStartPoint = event->pos();
	if (event->button() != Qt::LeftButton) {return;}

	VTK2DGraphicsView* v2 = dynamic_cast<VTK2DGraphicsView*>(v);
	auto tmpv = v2->viewportToWorld(event->pos());

	if (impl->m_mouseEventMode == Impl::meBeforeDefining) {
		emit startPointEdited(tmpv);
		emit endPointEdited(tmpv);
		pushUpdateActorSettingCommand(new DefineCommand(tmpv, tmpv, false, this), this);
	} else if (impl->m_mouseEventMode == Impl::meMoveVertexPrepare) {
		if (impl->m_dragPointTarget == 1) {
			emit startPointEdited(tmpv);
		} else if (impl->m_dragPointTarget == 2) {
			emit endPointEdited(tmpv);
		}
		pushUpdateActorSettingCommand(new MoveVertexCommand(impl->m_dragPointTarget, tmpv, false, this), this);
	} else if (impl->m_mouseEventMode == Impl::meTranslatePrepare) {
		impl->m_dragStartPoint1 = impl->m_setting.point1;
		impl->m_dragStartPoint2 = impl->m_setting.point2;

		emit startPointEdited(impl->m_setting.point1);
		emit endPointEdited(impl->m_setting.point2);
		pushUpdateActorSettingCommand(new TranslateCommand(impl->m_setting.point1, impl->m_setting.point2, false, this), this);
	}
}

void DistanceMeasureDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (impl->m_mouseEventMode == Impl::meDefining && event->button() == Qt::LeftButton) {
		pushUpdateActorSettingCommand(new DefineCommand(impl->m_setting.point1, impl->m_setting.point2, true, this), this);
	} else if (impl->m_mouseEventMode == Impl::meMoveVertex && event->button() == Qt::LeftButton) {
		QPointF tmpv;
		if (impl->m_dragPointTarget == 1) {
			tmpv = impl->m_setting.point1;
		} else if (impl->m_dragPointTarget == 2) {
			tmpv = impl->m_setting.point2;
		}
		pushUpdateActorSettingCommand(new MoveVertexCommand(impl->m_dragPointTarget, tmpv, true, this), this);
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
		pushUpdateActorSettingCommand(new DefineCommand(impl->m_setting.point1, newv2, false, this), this);
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
		pushUpdateActorSettingCommand(new MoveVertexCommand(impl->m_dragPointTarget, newv, false, this), this);
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
	updateActorSetting();
	renderGraphicsView();
}

void DistanceMeasureDataItem::updateActorSetting()
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
	impl->m_setting.labelFontSetting.fontColor.setValue(impl->m_setting.color.value());
	impl->m_setting.labelFontSetting.applySetting(txtProp);
	impl->m_labelActor.actor()->GetProperty()->SetColor(impl->m_setting.color);

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
	std::vector<QPointF> line = impl->m_lineActor.line();
	if (line.size() < 2) {
		return nullptr;
	}

	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, parent);
	auto widget = new EditWidget(this, dialog);
	widget->setName(m_standardItem->text().trimmed());
	connect(this, &DistanceMeasureDataItem::startPointEdited, widget, &EditWidget::setStartPoint);
	connect(this, &DistanceMeasureDataItem::endPointEdited, widget, &EditWidget::setEndPoint);

	dialog->setWidget(widget);
	dialog->setWindowTitle("Distance Measure Setting");
	return dialog;
}

QString DistanceMeasureDataItem::autoLabel() const
{
	return QString::number(iRIC::length(impl->m_setting.point2 - impl->m_setting.point1));
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

	updateActorSetting();
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

void DistanceMeasureDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

void DistanceMeasureDataItem::redefine()
{
	pushUpdateActorSettingCommand(new RedefineCommand(this), this);
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
			vertical = iRIC::rotateVector90(vertical);
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

