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

DistanceMeasureDataItem::DistanceMeasureDataItem(const QString& name, GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem {name, QIcon(":/libs/guibase/images/iconPaper.png"), parent}
{
	m_isDeletable = true;

	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();

	m_showLabel = true;
	m_labelPosition = DistanceMeasureCopyPropertyDialog::lpTopCenter;
	m_labelMode = DistanceMeasureCopyPropertyDialog::lmAuto;
	m_fontSize = 12;
	m_customLabel = "";
	m_showMarkers = true;
	m_markerSize = 7;
	m_color = Qt::black;

	m_shiftPressed = false;
	m_defined = false;
	m_mouseEventMode = meBeforeDefining;
	m_dragPointTarget = 0;

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
	r->RemoveActor2D(m_labelActor);

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

	m_labelActor = vtkSmartPointer<vtkActor2D>::New();
	m_labelActor->GetProperty()->SetColor(0, 0, 0);
	m_labelActor->SetVisibility(0);
	renderer()->AddActor2D(m_labelActor);

	m_labelPoints = vtkSmartPointer<vtkPoints>::New();
	m_label = vtkSmartPointer<vtkUnstructuredGrid>::New();
	m_labelPoints->InsertNextPoint(5, 0, 0);
	m_label->SetPoints(m_labelPoints);
	vtkVertex* v = vtkVertex::New();
	v->GetPointIds()->SetId(0, 0);
	m_label->InsertNextCell(v->GetCellType(), v->GetPointIds());
	v->Delete();

	m_labelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
	m_labelMapper->SetLabelModeToLabelFieldData();
	m_labelMapper->SetFieldDataName(LABEL);
	vtkTextProperty* txtprop = m_labelMapper->GetLabelTextProperty();
	txtprop->SetColor(0, 0, 0);
	txtprop->SetFontSize(m_fontSize);
	txtprop->BoldOff();
	txtprop->ItalicOff();
	txtprop->ShadowOff();

	m_labelArray = vtkSmartPointer<vtkStringArray>::New();
	m_labelArray->SetName(LABEL);
	m_labelArray->InsertNextValue("");
	m_label->GetPointData()->AddArray(m_labelArray);
	m_labelMapper->SetInputData(m_label);

	m_labelActor->SetMapper(m_labelMapper);
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
	QVector2D globalPos(x, y);
	if (m_mouseEventMode == meDefining) {
		QVector2D newv2 = updatePoint2ByShift(m_point1, globalPos);
		iRICUndoStack::instance().push(new DefineCommand(m_point1, newv2, false, this));
	} else if (m_mouseEventMode == meMoveVertex) {
		QVector2D otherp;
		if (m_dragPointTarget == 1) {
			otherp = m_point2;
		} else {
			otherp = m_point1;
		}
		QVector2D newv = updatePoint2ByShift(otherp, globalPos);
		iRICUndoStack::instance().push(new MoveVertexCommand(m_dragPointTarget, newv, false, this));
	} else if (m_mouseEventMode == meTranslate) {
		x = m_dragStartPoint.x();
		y = m_dragStartPoint.y();
		v2->viewportToWorld(x, y);
		QVector2D startGlobalPos(x, y);
		QVector2D offset = globalPos - startGlobalPos;

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
	QVector2D tmpv(x, y);

	if (m_mouseEventMode == meBeforeDefining) {
		iRICUndoStack::instance().push(new DefineCommand(tmpv, tmpv, false, this));
	} else if (m_mouseEventMode == meMoveVertexPrepare) {
		iRICUndoStack::instance().push(new MoveVertexCommand(m_dragPointTarget, tmpv, false, this));
	} else if (m_mouseEventMode == meTranslatePrepare) {
		m_dragStartPoint1 = m_point1;
		m_dragStartPoint2 = m_point2;
		pushRenderCommand(new TranslateCommand(m_point1, m_point2, false, this), this);
	}
}

void DistanceMeasureDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (m_mouseEventMode == DistanceMeasureDataItem::meDefining && event->button() == Qt::LeftButton) {
		iRICUndoStack::instance().push(new DefineCommand(m_point1, m_point2, true, this));
	} else if (m_mouseEventMode == DistanceMeasureDataItem::meMoveVertex && event->button() == Qt::LeftButton) {
		QVector2D tmpv;
		if (m_dragPointTarget == 1) {
			tmpv = m_point1;
		} else if (m_dragPointTarget == 2) {
			tmpv = m_point2;
		}
		iRICUndoStack::instance().push(new MoveVertexCommand(m_dragPointTarget, tmpv, true, this));
	} else if (m_mouseEventMode == DistanceMeasureDataItem::meTranslate && event->button() == Qt::LeftButton) {
		VTK2DGraphicsView* v2 = dynamic_cast<VTK2DGraphicsView*>(v);
		double x, y;
		x = event->x();
		y = event->y();
		v2->viewportToWorld(x, y);
		QVector2D globalPos(x, y);
		x = m_dragStartPoint.x();
		y = m_dragStartPoint.y();
		v2->viewportToWorld(x, y);
		QVector2D startGlobalPos(x, y);
		QVector2D offset = globalPos - startGlobalPos;

		pushRenderCommand(new TranslateCommand(m_dragStartPoint1 + offset, m_dragStartPoint2 + offset, true, this), this);
	}
	if (event->button() == Qt::RightButton) {
		if (iRIC::isNear(m_dragStartPoint, event->pos())) {
			m_redefineAction->setEnabled(m_defined);

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
		QVector2D newv2 = updatePoint2ByShift(m_point1, m_point2);
		iRICUndoStack::instance().push(new DefineCommand(m_point1, newv2, false, this));
	} else if (m_mouseEventMode == meMoveVertex && event->key() == Qt::Key_Shift) {
		QVector2D movep;
		QVector2D otherp;
		if (m_dragPointTarget == 1) {
			movep = m_point1;
			otherp = m_point2;
		} else {
			movep = m_point2;
			otherp = m_point1;
		}
		QVector2D newv = updatePoint2ByShift(otherp, movep);
		iRICUndoStack::instance().push(new MoveVertexCommand(m_dragPointTarget, newv, false, this));
	}
}
void DistanceMeasureDataItem::keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* /*v*/)
{
	if (event->key() == Qt::Key_Shift) {
		m_shiftPressed = false;
	}
}

void DistanceMeasureDataItem::setPoints(const QVector2D& v1, const QVector2D& v2)
{
	m_point1 = v1;
	m_point2 = v2;
	updateActorSettings();
	renderGraphicsView();
}

void DistanceMeasureDataItem::updateActorSettings()
{
	std::vector<QPointF> line;
	line.push_back(QPointF(m_point1.x(), m_point1.y()));
	line.push_back(QPointF(m_point2.x(), m_point2.y()));
	m_lineActor.setLine(line);

	QVector2D mid = (m_point1 + m_point2) * 0.5;
	m_labelPoints->SetPoint(0, mid.x(), mid.y(), 0);
	vtkTextProperty* txtProp = m_labelMapper->GetLabelTextProperty();

	switch (m_labelPosition) {
	case DistanceMeasureCopyPropertyDialog::lpTopCenter:
		txtProp->SetJustificationToCentered();
		txtProp->SetVerticalJustificationToBottom();
		txtProp->SetLineOffset(-5);
		break;
	case DistanceMeasureCopyPropertyDialog::lpBottomCenter:
		txtProp->SetJustificationToCentered();
		txtProp->SetVerticalJustificationToTop();
		txtProp->SetLineOffset(5);
		break;
	case DistanceMeasureCopyPropertyDialog::lpMiddleLeft:
		txtProp->SetJustificationToRight();
		txtProp->SetVerticalJustificationToCentered();
		txtProp->SetLineOffset(0);
		break;
	case DistanceMeasureCopyPropertyDialog::lpMiddleRight:
		txtProp->SetJustificationToLeft();
		txtProp->SetVerticalJustificationToCentered();
		txtProp->SetLineOffset(0);
		break;
	}

	QString label;
	if (m_labelMode == DistanceMeasureCopyPropertyDialog::lmAuto) {
		label = autoLabel();
	} else {
		label = m_customLabel;
	}
	switch (m_labelPosition) {
	case DistanceMeasureCopyPropertyDialog::lpMiddleLeft:
		label = label.append("   ");
		break;
	case DistanceMeasureCopyPropertyDialog::lpMiddleRight:
		label = QString("   ").append(label);
		break;
	default:
		// do nothing
		break;
	}
	m_labelArray->SetValue(0, iRIC::toStr(label).c_str());

	txtProp->SetFontSize(m_fontSize);
	txtProp->SetColor(m_color.redF(), m_color.greenF(), m_color.blueF());

	m_lineActor.pointsActor()->GetProperty()->SetColor(m_color.redF(), m_color.greenF(), m_color.blueF());
	m_lineActor.pointsActor()->GetProperty()->SetPointSize(m_markerSize);
	m_lineActor.lineActor()->GetProperty()->SetColor(m_color.redF(), m_color.greenF(), m_color.blueF());

	m_lineActor.pointsActor()->VisibilityOff();
	m_lineActor.lineActor()->VisibilityOff();
	m_labelActor->VisibilityOff();
	actorCollection()->RemoveItem(m_lineActor.pointsActor());
	actorCollection()->RemoveItem(m_lineActor.lineActor());
	actor2DCollection()->RemoveItem(m_labelActor);

	if (m_defined) {
		if (m_showMarkers) {
			actorCollection()->AddItem(m_lineActor.pointsActor());
		}
		actorCollection()->AddItem(m_lineActor.lineActor());
		if (m_showLabel) {
			actor2DCollection()->AddItem(m_labelActor);
		}
	}

	updateVisibilityWithoutRendering();
	m_labelPoints->Modified();
}


QDialog* DistanceMeasureDataItem::propertyDialog(QWidget* parent)
{
	QVector2D v1, v2;

	std::vector<QPointF> line = m_lineActor.line();
	v1 = QVector2D(line.at(0).x(), line.at(0).y());
	v2 = QVector2D(line.at(1).x(), line.at(1).y());

	DistanceMeasureCopyPropertyDialog* dialog = new DistanceMeasureCopyPropertyDialog(parent);
	dialog->setName(m_standardItem->text().trimmed());
	dialog->setPoints(v1, v2);
	dialog->setShowLabel(m_showLabel);
	dialog->setLabelPosition(m_labelPosition);
	dialog->setLabelMode(m_labelMode);
	dialog->setLabelFontSize(m_fontSize);
	dialog->setCustomlabel(m_customLabel);
	dialog->setShowMarkers(m_showMarkers);
	dialog->setMarkerSize(m_markerSize);
	dialog->setColor(m_color);

	return dialog;
}

void DistanceMeasureDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	DistanceMeasureCopyPropertyDialog* dialog = dynamic_cast<DistanceMeasureCopyPropertyDialog*>(propDialog);
	iRICUndoStack::instance().push(new SetSettingCommand(dialog->name(), dialog->point1(), dialog->point2(),
																 dialog->showLabel(), dialog->labelPosition(),
																 dialog->labelMode(), dialog->fontSize(), dialog->customLabel(),
																 dialog->showMarkers(), dialog->markerSize(), dialog->color(), this));
}

QString DistanceMeasureDataItem::autoLabel() const
{
	return QString::number((m_point2 - m_point1).length());
}

void DistanceMeasureDataItem::showPropDialog()
{
	showPropertyDialog();
}

QVector2D DistanceMeasureDataItem::updatePoint2ByShift(const QVector2D& v1, const QVector2D& v2)
{
	if (! m_shiftPressed) {return v2;}
	double xdist = qAbs(v2.x() - v1.x());
	double ydist = qAbs(v2.y() - v1.y());
	if (xdist >= ydist) {
		return QVector2D(v2.x(), v1.y());
	} else {
		return QVector2D(v1.x(), v2.y());
	}
}

void DistanceMeasureDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_isCommandExecuting = true;
	m_standardItem->setText(node.toElement().attribute("name"));
	m_showLabel = iRIC::getBooleanAttribute(node, "showLabel");
	m_labelMode = static_cast<DistanceMeasureCopyPropertyDialog::LabelMode>(iRIC::getIntAttribute(node, "labelMode"));
	m_labelPosition = static_cast<DistanceMeasureCopyPropertyDialog::LabelPosition>(iRIC::getIntAttribute(node, "labelPosition"));
	m_customLabel = node.toElement().attribute("customLabel");
	m_fontSize = iRIC::getIntAttribute(node, "fontSize");
	QVector2D p;
	p.setX(iRIC::getDoubleAttribute(node, "point1x"));
	p.setY(iRIC::getDoubleAttribute(node, "point1y"));
	m_point1 = p;
	p.setX(iRIC::getDoubleAttribute(node, "point2x"));
	p.setY(iRIC::getDoubleAttribute(node, "point2y"));
	m_point2 = p;
	m_showMarkers = iRIC::getBooleanAttribute(node, "showMarkers");
	m_markerSize = iRIC::getIntAttribute(node, "markerSize");
	m_color = iRIC::getColorAttribute(node, "color");
	m_defined = iRIC::getBooleanAttribute(node, "defined");
	m_isCommandExecuting = false;

	if (m_defined) {
		m_mouseEventMode = meNormal;
	} else {
		m_mouseEventMode = meBeforeDefining;
	}

	updateActorSettings();
}

void DistanceMeasureDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", standardItem()->text());
	iRIC::setBooleanAttribute(writer, "showLabel", m_showLabel);
	iRIC::setIntAttribute(writer, "labelMode", static_cast<int>(m_labelMode));
	iRIC::setIntAttribute(writer, "labelPosition", static_cast<int>(m_labelPosition));
	writer.writeAttribute("customLabel", m_customLabel);
	iRIC::setIntAttribute(writer, "fontSize", m_fontSize);
	iRIC::setDoubleAttribute(writer, "point1x", m_point1.x());
	iRIC::setDoubleAttribute(writer, "point1y", m_point1.y());
	iRIC::setDoubleAttribute(writer, "point2x", m_point2.x());
	iRIC::setDoubleAttribute(writer, "point2y", m_point2.y());
	iRIC::setBooleanAttribute(writer, "showMarkers", m_showMarkers);
	iRIC::setIntAttribute(writer, "markerSize", m_markerSize);
	iRIC::setColorAttribute(writer, "color", m_color);
	iRIC::setBooleanAttribute(writer, "defined", m_defined);
}

void DistanceMeasureDataItem::addCustomMenuItems(QMenu* menu)
{
	m_redefineAction->setEnabled(m_defined);
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

void DistanceMeasureDataItem::updateMouseEventMode(const QVector2D& v, VTKGraphicsView* view)
{
	VTK2DGraphicsView* view2 = dynamic_cast<VTK2DGraphicsView*>(view);
	switch (m_mouseEventMode) {
	case meNormal:
	case meMoveVertexPrepare:
	case meTranslatePrepare:
		if ((m_point1 - v).length() < view2->stdRadius(5)) {
			m_mouseEventMode = meMoveVertexPrepare;
			m_dragPointTarget = 1;
		} else if ((m_point2 - v).length() < view2->stdRadius(5)) {
			m_mouseEventMode = meMoveVertexPrepare;
			m_dragPointTarget = 2;
		} else {
			QVector2D horizontal = m_point2 - m_point1;
			QVector2D vertical = horizontal.normalized();
			iRIC::rotateVector90(vertical);
			double width = view2->stdRadius(5);
			vertical *= width;
			QVector2D posv = m_point1 - vertical * 0.5;
			if (iRIC::isInsideParallelogram(v, posv, horizontal, vertical)) {
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

