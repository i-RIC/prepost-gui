#include "distancemeasuredataitem.h"
#include "private/distancemeasuredataitem_definecommand.h"
#include "private/distancemeasuredataitem_movevertexcommand.h"
#include "private/distancemeasuredataitem_redefinecommand.h"
#include "private/distancemeasuredataitem_setsettingcommand.h"
#include "private/distancemeasuredataitem_translatecommand.h"

#include <guicore/datamodel/graphicswindowdatamodel.h>
#include <guicore/datamodel/vtk2dgraphicsview.h>
#include <guicore/misc/qundocommandhelper.h>

#include <misc/iricundostack.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QAction>
#include <QDomNode>
#include <QMainWindow>
#include <QMenu>
#include <QMouseEvent>
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

namespace {

QVector2D toVec(const QPointF v)
{
	return QVector2D(v.x(), v.y());
}

} // namespace

DistanceMeasureDataItem::DistanceMeasureDataItem(const QString& name, GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem {name, QIcon(":/libs/guibase/images/iconPaper.png"), parent},
	m_setting {},
	m_mouseEventMode {meBeforeDefining},
	m_shiftPressed {false},
	m_dragPointTarget {0}
{
	setupStandardItem(Checked, NotReorderable, Deletable);

	m_movePointPixmap = QPixmap(":/libs/guibase/images/cursorOpenHandPoint.png");
	m_movePointCursor = QCursor(m_movePointPixmap);

	m_rightClickingMenu = new QMenu(mainWindow());
	m_propertyAction = new QAction(DistanceMeasureDataItem::tr("Property"), m_rightClickingMenu);
	m_propertyAction->setIcon(QIcon(":/libs/guibase/images/iconProperty.png"));
	connect(m_propertyAction, SIGNAL(triggered()), this, SLOT(showPropDialog()));
	m_redefineAction = new QAction(DistanceMeasureDataItem::tr("Redefine"), m_rightClickingMenu);
	connect(m_redefineAction, SIGNAL(triggered()), this, SLOT(redefine()));

	m_rightClickingMenu->addAction(m_redefineAction);
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_propertyAction);

	setupActors();
}

DistanceMeasureDataItem::~DistanceMeasureDataItem()
{
	vtkRenderer* r = renderer();
	r->RemoveActor(m_lineActor.pointsActor());
	r->RemoveActor(m_lineActor.lineActor());
	r->RemoveActor2D(m_labelActor.actor());

	delete m_rightClickingMenu;
}

void DistanceMeasureDataItem::setupActors()
{
	vtkProperty* pointsProp = m_lineActor.pointsActor()->GetProperty();
	pointsProp->SetLighting(false);
	pointsProp->SetPointSize(7);
	pointsProp->SetColor(0, 0, 0);
	m_lineActor.pointsActor()->SetVisibility(0);
	renderer()->AddActor(m_lineActor.pointsActor());

	vtkProperty* lineProp = m_lineActor.lineActor()->GetProperty();
	lineProp->SetLighting(false);
	lineProp->SetLineWidth(1);
	lineProp->SetColor(0, 0, 0);
	m_lineActor.lineActor()->SetVisibility(0);
	renderer()->AddActor(m_lineActor.lineActor());

	m_labelActor.actor()->GetProperty()->SetColor(0, 0, 0);
	m_labelActor.actor()->SetVisibility(0);
	renderer()->AddActor2D(m_labelActor.actor());
}

void DistanceMeasureDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void DistanceMeasureDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_lineActor.pointsActor()->SetPosition(0, 0, range.max());
	m_lineActor.lineActor()->SetPosition(0, 0, range.max());
}

void DistanceMeasureDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	VTK2DGraphicsView* v2 = dynamic_cast<VTK2DGraphicsView*>(v);
	double x, y;
	x = event->x();
	y = event->y();
	v2->viewportToWorld(x, y);
	QPointF globalPos(x, y);
	if (m_mouseEventMode == meDefining) {
		QPointF newv2 = updatePoint2ByShift(m_setting.point1, globalPos);
		iRICUndoStack::instance().push(new DefineCommand(m_setting.point1, newv2, false, this));
	} else if (m_mouseEventMode == meMoveVertex) {
		QPointF otherp;
		if (m_dragPointTarget == 1) {
			otherp = m_setting.point2;
		} else {
			otherp = m_setting.point1;
		}
		QPointF newv = updatePoint2ByShift(otherp, globalPos);
		iRICUndoStack::instance().push(new MoveVertexCommand(m_dragPointTarget, newv, false, this));
	} else if (m_mouseEventMode == meTranslate) {
		x = m_dragStartPoint.x();
		y = m_dragStartPoint.y();
		v2->viewportToWorld(x, y);
		QPointF startGlobalPos(x, y);
		QPointF offset = globalPos - startGlobalPos;

		pushRenderCommand(new TranslateCommand(m_dragStartPoint1 + offset, m_dragStartPoint2 + offset, false, this), this);
	} else {
		updateMouseEventMode(globalPos, v);
		updateMouseCursor();
	}
}

void DistanceMeasureDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_dragStartPoint = QPoint(event->x(), event->y());
	if (event->button() != Qt::LeftButton) {return;}
	VTK2DGraphicsView* v2 = dynamic_cast<VTK2DGraphicsView*>(v);
	double x, y;
	x = event->x();
	y = event->y();
	v2->viewportToWorld(x, y);
	QPointF tmpv(x, y);

	if (m_mouseEventMode == meBeforeDefining) {
		iRICUndoStack::instance().push(new DefineCommand(tmpv, tmpv, false, this));
	} else if (m_mouseEventMode == meMoveVertexPrepare) {
		iRICUndoStack::instance().push(new MoveVertexCommand(m_dragPointTarget, tmpv, false, this));
	} else if (m_mouseEventMode == meTranslatePrepare) {
		m_dragStartPoint1 = m_setting.point1;
		m_dragStartPoint2 = m_setting.point2;
		pushRenderCommand(new TranslateCommand(m_setting.point1, m_setting.point2, false, this), this);
	}
}

void DistanceMeasureDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (m_mouseEventMode == DistanceMeasureDataItem::meDefining && event->button() == Qt::LeftButton) {
		iRICUndoStack::instance().push(new DefineCommand(m_setting.point1, m_setting.point2, true, this));
	} else if (m_mouseEventMode == DistanceMeasureDataItem::meMoveVertex && event->button() == Qt::LeftButton) {
		QPointF tmpv;
		if (m_dragPointTarget == 1) {
			tmpv = m_setting.point1;
		} else if (m_dragPointTarget == 2) {
			tmpv = m_setting.point2;
		}
		iRICUndoStack::instance().push(new MoveVertexCommand(m_dragPointTarget, tmpv, true, this));
	} else if (m_mouseEventMode == DistanceMeasureDataItem::meTranslate && event->button() == Qt::LeftButton) {
		VTK2DGraphicsView* v2 = dynamic_cast<VTK2DGraphicsView*>(v);
		double x, y;
		x = event->x();
		y = event->y();
		v2->viewportToWorld(x, y);
		QPointF globalPos(x, y);
		x = m_dragStartPoint.x();
		y = m_dragStartPoint.y();
		v2->viewportToWorld(x, y);
		QPointF startGlobalPos(x, y);
		QPointF offset = globalPos - startGlobalPos;

		pushRenderCommand(new TranslateCommand(m_dragStartPoint1 + offset, m_dragStartPoint2 + offset, true, this), this);
	}
	if (event->button() == Qt::RightButton) {
		if (iRIC::isNear(m_dragStartPoint, event->pos())) {
			m_redefineAction->setEnabled(m_setting.defined);

			m_rightClickingMenu->move(event->globalPos());
			m_rightClickingMenu->show();
		}
	}
}

void DistanceMeasureDataItem::keyPressEvent(QKeyEvent* event, VTKGraphicsView* /*v*/)
{
	if (event->key() == Qt::Key_Shift) {
		m_shiftPressed = true;
	}
	if (m_mouseEventMode == meDefining && event->key() == Qt::Key_Shift) {
		QPointF newv2 = updatePoint2ByShift(m_setting.point1, m_setting.point2);
		iRICUndoStack::instance().push(new DefineCommand(m_setting.point1, newv2, false, this));
	} else if (m_mouseEventMode == meMoveVertex && event->key() == Qt::Key_Shift) {
		QPointF movep;
		QPointF otherp;
		if (m_dragPointTarget == 1) {
			movep = m_setting.point1;
			otherp = m_setting.point2;
		} else {
			movep = m_setting.point2;
			otherp = m_setting.point1;
		}
		QPointF newv = updatePoint2ByShift(otherp, movep);
		iRICUndoStack::instance().push(new MoveVertexCommand(m_dragPointTarget, newv, false, this));
	}
}
void DistanceMeasureDataItem::keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* /*v*/)
{
	if (event->key() == Qt::Key_Shift) {
		m_shiftPressed = false;
	}
}

void DistanceMeasureDataItem::setPoints(const QPointF& v1, const QPointF& v2)
{
	m_setting.point1 = v1;
	m_setting.point2 = v2;
	updateActorSettings();
	renderGraphicsView();
}

void DistanceMeasureDataItem::updateActorSettings()
{
	std::vector<QPointF> line;
	line.push_back(m_setting.point1);
	line.push_back(m_setting.point2);
	m_lineActor.setLine(line);

	QPointF mid = (m_setting.point1 + m_setting.point2) * 0.5;
	m_labelActor.setPosition(mid);

	QString label;
	if (m_setting.labelMode == DistanceMeasureSetting::Auto) {
		label = autoLabel();
	} else {
		label = m_setting.customLabel;
	}
	m_labelActor.setLabel(iRIC::toStr(label));
	m_labelActor.setLabelPosition(m_setting.labelPosition);

	vtkTextProperty* txtProp = m_labelActor.labelTextProperty();
	txtProp->SetFontSize(m_setting.labelFontSize);
	txtProp->SetColor(m_setting.color);

	m_lineActor.pointsActor()->GetProperty()->SetColor(m_setting.color);
	m_lineActor.pointsActor()->GetProperty()->SetPointSize(m_setting.markerSize);
	m_lineActor.lineActor()->GetProperty()->SetColor(m_setting.color);

	m_lineActor.pointsActor()->VisibilityOff();
	m_lineActor.lineActor()->VisibilityOff();
	m_labelActor.actor()->VisibilityOff();
	actorCollection()->RemoveItem(m_lineActor.pointsActor());
	actorCollection()->RemoveItem(m_lineActor.lineActor());
	actor2DCollection()->RemoveItem(m_labelActor.actor());

	if (m_setting.defined) {
		if (m_setting.showMarkers) {
			actorCollection()->AddItem(m_lineActor.pointsActor());
		}
		actorCollection()->AddItem(m_lineActor.lineActor());
		if (m_setting.showLabel) {
			actor2DCollection()->AddItem(m_labelActor.actor());
		}
	}

	updateVisibilityWithoutRendering();
}

QDialog* DistanceMeasureDataItem::propertyDialog(QWidget* parent)
{
	QVector2D v1, v2;

	std::vector<QPointF> line = m_lineActor.line();
	v1 = QVector2D(line.at(0).x(), line.at(0).y());
	v2 = QVector2D(line.at(1).x(), line.at(1).y());

	DistanceMeasureCopyPropertyDialog* dialog = new DistanceMeasureCopyPropertyDialog(parent);
	dialog->setName(m_standardItem->text().trimmed());
	dialog->setSetting(m_setting);

	return dialog;
}

void DistanceMeasureDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	DistanceMeasureCopyPropertyDialog* dialog = dynamic_cast<DistanceMeasureCopyPropertyDialog*>(propDialog);
	iRICUndoStack::instance().push(new SetSettingCommand(dialog->name(), dialog->setting(), this));
}

QString DistanceMeasureDataItem::autoLabel() const
{
	return QString::number(toVec(m_setting.point2 - m_setting.point1).length());
}

void DistanceMeasureDataItem::showPropDialog()
{
	showPropertyDialog();
}

QPointF DistanceMeasureDataItem::updatePoint2ByShift(const QPointF& v1, const QPointF& v2)
{
	if (! m_shiftPressed) {return v2;}
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
	m_setting.load(node);

	if (m_setting.defined) {
		m_mouseEventMode = meNormal;
	} else {
		m_mouseEventMode = meBeforeDefining;
	}

	updateActorSettings();
}

void DistanceMeasureDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", standardItem()->text());
	m_setting.save(writer);
}

void DistanceMeasureDataItem::addCustomMenuItems(QMenu* menu)
{
	m_redefineAction->setEnabled(m_setting.defined);
	menu->addAction(m_redefineAction);
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
	switch (m_mouseEventMode) {
	case meNormal:
		v->setCursor(Qt::ArrowCursor);
		break;
	case meBeforeDefining:
	case meDefining:
		v->setCursor(Qt::CrossCursor);
		break;
	case meTranslate:
		v->setCursor(Qt::ClosedHandCursor);
		break;
	case meTranslatePrepare:
		v->setCursor(Qt::OpenHandCursor);
		break;
	case meMoveVertex:
		v->setCursor(Qt::ClosedHandCursor);
		break;
	case meMoveVertexPrepare:
		v->setCursor(m_movePointCursor);
		break;
	}
}

void DistanceMeasureDataItem::updateMouseEventMode(const QPointF& v, VTKGraphicsView* view)
{
	VTK2DGraphicsView* view2 = dynamic_cast<VTK2DGraphicsView*>(view);
	switch (m_mouseEventMode) {
	case meNormal:
	case meMoveVertexPrepare:
	case meTranslatePrepare:
		if (toVec(m_setting.point1 - v).length() < view2->stdRadius(5)) {
			m_mouseEventMode = meMoveVertexPrepare;
			m_dragPointTarget = 1;
		} else if (toVec(m_setting.point2 - v).length() < view2->stdRadius(5)) {
			m_mouseEventMode = meMoveVertexPrepare;
			m_dragPointTarget = 2;
		} else {
			QVector2D horizontal = toVec(m_setting.point2 - m_setting.point1);
			QVector2D vertical = horizontal.normalized();
			iRIC::rotateVector90(vertical);
			double width = view2->stdRadius(5);
			vertical *= width;
			QVector2D posv = toVec(m_setting.point1) - vertical * 0.5;
			if (iRIC::isInsideParallelogram(toVec(v), posv, horizontal, vertical)) {
				m_mouseEventMode = meTranslatePrepare;
			} else {
				m_mouseEventMode = meNormal;
			}
		}

		break;
	case meBeforeDefining:
	case meDefining:
	case meMoveVertex:
	case meTranslate:
		// do nothing
		break;
	}
}

