#include "../datamodel/graphicswindowdatamodel.h"
#include "../datamodel/vtk2dgraphicsview.h"
#include "../misc/qundocommandhelper.h"
#include "distancemeasuredataitem.h"

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
#include <QVector2D>
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
	r->RemoveActor(m_pointsActor);
	r->RemoveActor(m_lineActor);
	r->RemoveActor2D(m_labelActor);

	delete m_rightClickingMenu;
}

void DistanceMeasureDataItem::setupActors()
{
	m_pointsActor = vtkSmartPointer<vtkActor>::New();
	m_pointsActor->GetProperty()->SetLighting(false);
	m_pointsActor->GetProperty()->SetPointSize(7);
	m_pointsActor->GetProperty()->SetColor(0, 0, 0);
	m_pointsActor->SetVisibility(0);
	renderer()->AddActor(m_pointsActor);

	m_lineActor = vtkSmartPointer<vtkActor>::New();
	m_lineActor->GetProperty()->SetLighting(false);
	m_lineActor->GetProperty()->SetLineWidth(1);
	m_lineActor->GetProperty()->SetColor(0, 0, 0);
	m_lineActor->SetVisibility(0);
	renderer()->AddActor(m_lineActor);

	m_points = vtkSmartPointer<vtkPoints>::New();
	m_line = vtkSmartPointer<vtkUnstructuredGrid>::New();
	m_dots = vtkSmartPointer<vtkUnstructuredGrid>::New();

	vtkSmartPointer<vtkDataSetMapper> mapper;
	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputData(m_dots);
	m_pointsActor->SetMapper(mapper);

	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputData(m_line);
	m_lineActor->SetMapper(mapper);

	// just for test
	m_points->InsertNextPoint(0, 0, 0);
	m_points->InsertNextPoint(10, 0, 0);

	m_dots->SetPoints(m_points);
	vtkVertex* v = vtkVertex::New();
	v->GetPointIds()->SetId(0, 0);
	m_dots->InsertNextCell(v->GetCellType(), v->GetPointIds());
	v->GetPointIds()->SetId(0, 1);
	m_dots->InsertNextCell(v->GetCellType(), v->GetPointIds());
	v->Delete();

	m_line->SetPoints(m_points);
	vtkLine* l = vtkLine::New();
	l->GetPointIds()->SetId(0, 0);
	l->GetPointIds()->SetId(1, 1);
	m_line->InsertNextCell(l->GetCellType(), l->GetPointIds());
	l->Delete();

	m_labelActor = vtkSmartPointer<vtkActor2D>::New();
	m_labelActor->GetProperty()->SetColor(0, 0, 0);
	m_labelActor->SetVisibility(0);
	renderer()->AddActor2D(m_labelActor);

	m_labelPoints = vtkSmartPointer<vtkPoints>::New();
	m_label = vtkSmartPointer<vtkUnstructuredGrid>::New();
	m_labelPoints->InsertNextPoint(5, 0, 0);
	m_label->SetPoints(m_labelPoints);
	v = vtkVertex::New();
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
	m_pointsActor->SetPosition(0, 0, range.max());
	m_lineActor->SetPosition(0, 0, range.max());
}

class DistanceMeasureDefineCommand : public QUndoCommand
{
public:
	DistanceMeasureDefineCommand(const QVector2D& v1, const QVector2D& v2, bool finish, DistanceMeasureDataItem* item)
		: QUndoCommand(QObject::tr("Define Distance Measure")) {
		m_item = item;

		m_newPoint1 = v1;
		m_newPoint2 = v2;

		m_oldPoint1 = m_item->m_point1;
		m_oldPoint2 = m_item->m_point2;
		m_finish = finish;
	}
	void undo() {
		m_item->m_point1 = m_oldPoint1;
		m_item->m_point2 = m_oldPoint2;
		m_item->m_mouseEventMode = DistanceMeasureDataItem::meBeforeDefining;
		m_item->m_defined = false;

		m_item->updateMouseCursor();
		m_item->updateActorSettings();
		m_item->renderGraphicsView();
	}
	void redo() {
		m_item->m_point1 = m_newPoint1;
		m_item->m_point2 = m_newPoint2;
		if (m_finish) {
			m_item->m_mouseEventMode = DistanceMeasureDataItem::meNormal;
		} else {
			m_item->m_mouseEventMode = DistanceMeasureDataItem::meDefining;
		}
		m_item->m_defined = true;

		m_item->updateMouseCursor();
		m_item->updateActorSettings();
		m_item->renderGraphicsView();
	}
	int id() const {return iRIC::generateCommandId("DistanceMeasureDefineCommand");}
	virtual bool mergeWith(const QUndoCommand* other) {
		const DistanceMeasureDefineCommand* other2 = dynamic_cast<const DistanceMeasureDefineCommand*>(other);
		if (other2 == nullptr) { return false; }
		m_newPoint1 = other2->m_newPoint1;
		m_newPoint2 = other2->m_newPoint2;
		m_finish = other2->m_finish;
		return true;
	}

private:
	QVector2D m_newPoint1;
	QVector2D m_newPoint2;

	QVector2D m_oldPoint1;
	QVector2D m_oldPoint2;

	bool m_finish;
	DistanceMeasureDataItem* m_item;
};

class DistanceMeasureMoveVertexCommand : public QUndoCommand
{
public:
	DistanceMeasureMoveVertexCommand(int point, const QVector2D& v, bool finish, DistanceMeasureDataItem* item)
		: QUndoCommand(QObject::tr("Move Distance Measure Point")) {
		m_item = item;

		m_point = point;
		m_newPoint = v;

		if (point == 1) {
			m_oldPoint = m_item->m_point1;
		} else if (point == 2) {
			m_oldPoint = m_item->m_point2;
		}
		m_finish = finish;
	}
	void undo() {
		if (m_point == 1) {
			m_item->m_point1 = m_oldPoint;
		} else if (m_point == 2) {
			m_item->m_point2 = m_oldPoint;
		}
		m_item->m_mouseEventMode = DistanceMeasureDataItem::meNormal;

		m_item->updateMouseCursor();
		m_item->updateActorSettings();
		m_item->renderGraphicsView();
	}
	void redo() {
		if (m_point == 1) {
			m_item->m_point1 = m_newPoint;
		} else if (m_point == 2) {
			m_item->m_point2 = m_newPoint;
		}
		if (m_finish) {
			m_item->m_mouseEventMode = DistanceMeasureDataItem::meNormal;
		} else {
			m_item->m_mouseEventMode = DistanceMeasureDataItem::meMoveVertex;
		}
		m_item->updateMouseCursor();
		m_item->updateActorSettings();
		m_item->renderGraphicsView();
	}
	int id() const {return iRIC::generateCommandId("DistanceMeasureMoveVertexCommand");}
	virtual bool mergeWith(const QUndoCommand* other) {
		const DistanceMeasureMoveVertexCommand* other2 = dynamic_cast<const DistanceMeasureMoveVertexCommand*>(other);
		if (other2 == nullptr) {return false;}
		if (m_point != other2->m_point) {return false;}
		if (m_finish) {return false;}
		m_newPoint = other2->m_newPoint;
		m_finish = other2->m_finish;
		return true;
	}

private:
	int m_point;
	QVector2D m_newPoint;
	QVector2D m_oldPoint;
	bool m_finish;
	DistanceMeasureDataItem* m_item;
};

class DistanceMeasureTranslateCommand : public QUndoCommand
{
public:
	DistanceMeasureTranslateCommand(const QVector2D& v, bool finish, DistanceMeasureDataItem* item)
		: QUndoCommand(QObject::tr("Translate Distance Measure")) {
		m_item = item;

		m_diff = v;
		m_origPoint1 = m_item->m_dragStartPoint1;
		m_origPoint2 = m_item->m_dragStartPoint2;
		m_finish = finish;
	}
	void undo() {
		m_item->m_point1 = m_origPoint1;
		m_item->m_point2 = m_origPoint2;
		m_item->m_mouseEventMode = DistanceMeasureDataItem::meNormal;

		m_item->updateMouseCursor();
		m_item->updateActorSettings();
		m_item->renderGraphicsView();
	}
	void redo() {
		m_item->m_point1 = m_origPoint1 + m_diff;
		m_item->m_point2 = m_origPoint2 + m_diff;
		if (m_finish) {
			m_item->m_mouseEventMode = DistanceMeasureDataItem::meNormal;
		} else {
			m_item->m_mouseEventMode = DistanceMeasureDataItem::meTranslate;
		}
		m_item->updateMouseCursor();
		m_item->updateActorSettings();
		m_item->renderGraphicsView();
	}
	int id() const {return iRIC::generateCommandId("DistanceMeasureTranslateCommand");}
	virtual bool mergeWith(const QUndoCommand* other) {
		const DistanceMeasureTranslateCommand* other2 = dynamic_cast<const DistanceMeasureTranslateCommand*>(other);
		if (other2 == nullptr) {return false;}
		if (m_finish) {return false;}
		m_diff = other2->m_diff;
		m_finish = other2->m_finish;
		return true;
	}

private:
	QVector2D m_origPoint1;
	QVector2D m_origPoint2;
	QVector2D m_diff;
	bool m_finish;
	DistanceMeasureDataItem* m_item;
};

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
		iRICUndoStack::instance().push(new DistanceMeasureDefineCommand(m_point1, newv2, false, this));
	} else if (m_mouseEventMode == meMoveVertex) {
		QVector2D otherp;
		if (m_dragPointTarget == 1) {
			otherp = m_point2;
		} else {
			otherp = m_point1;
		}
		QVector2D newv = updatePoint2ByShift(otherp, globalPos);
		iRICUndoStack::instance().push(new DistanceMeasureMoveVertexCommand(m_dragPointTarget, newv, false, this));
	} else if (m_mouseEventMode == meTranslate) {
		x = m_dragStartPoint.x();
		y = m_dragStartPoint.y();
		v2->viewportToWorld(x, y);
		QVector2D startGlobalPos(x, y);
		iRICUndoStack::instance().push(new DistanceMeasureTranslateCommand(globalPos - startGlobalPos, false, this));
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
		iRICUndoStack::instance().push(new DistanceMeasureDefineCommand(tmpv, tmpv, false, this));
	} else if (m_mouseEventMode == meMoveVertexPrepare) {
		iRICUndoStack::instance().push(new DistanceMeasureMoveVertexCommand(m_dragPointTarget, tmpv, false, this));
	} else if (m_mouseEventMode == meTranslatePrepare) {
		m_dragStartPoint1 = m_point1;
		m_dragStartPoint2 = m_point2;
		iRICUndoStack::instance().push(new DistanceMeasureTranslateCommand(QVector2D(0, 0), false, this));
	}
}

void DistanceMeasureDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (m_mouseEventMode == DistanceMeasureDataItem::meDefining && event->button() == Qt::LeftButton) {
		iRICUndoStack::instance().push(new DistanceMeasureDefineCommand(m_point1, m_point2, true, this));
	} else if (m_mouseEventMode == DistanceMeasureDataItem::meMoveVertex && event->button() == Qt::LeftButton) {
		QVector2D tmpv;
		if (m_dragPointTarget == 1) {
			tmpv = m_point1;
		} else if (m_dragPointTarget == 2) {
			tmpv = m_point2;
		}
		iRICUndoStack::instance().push(new DistanceMeasureMoveVertexCommand(m_dragPointTarget, tmpv, true, this));
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
		iRICUndoStack::instance().push(new DistanceMeasureTranslateCommand(globalPos - startGlobalPos, true, this));
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
		iRICUndoStack::instance().push(new DistanceMeasureDefineCommand(m_point1, newv2, false, this));
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
		iRICUndoStack::instance().push(new DistanceMeasureMoveVertexCommand(m_dragPointTarget, newv, false, this));
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
	m_points->SetPoint(0, m_point1.x(), m_point1.y(), 0);
	m_points->SetPoint(1, m_point2.x(), m_point2.y(), 0);

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

	m_pointsActor->GetProperty()->SetColor(m_color.redF(), m_color.greenF(), m_color.blueF());
	m_pointsActor->GetProperty()->SetPointSize(m_markerSize);
	m_lineActor->GetProperty()->SetColor(m_color.redF(), m_color.greenF(), m_color.blueF());

	m_pointsActor->VisibilityOff();
	m_lineActor->VisibilityOff();
	m_labelActor->VisibilityOff();
	actorCollection()->RemoveItem(m_pointsActor);
	actorCollection()->RemoveItem(m_lineActor);
	actor2DCollection()->RemoveItem(m_labelActor);

	if (m_defined) {
		if (m_showMarkers) {
			actorCollection()->AddItem(m_pointsActor);
		}
		actorCollection()->AddItem(m_lineActor);
		if (m_showLabel) {
			actor2DCollection()->AddItem(m_labelActor);
		}
	}

	updateVisibilityWithoutRendering();

	m_points->Modified();
	m_labelPoints->Modified();
}


QDialog* DistanceMeasureDataItem::propertyDialog(QWidget* parent)
{
	QVector2D v1, v2;
	double tmpv[3];

	m_points->GetPoint(0, tmpv);
	v1.setX(tmpv[0]);
	v1.setY(tmpv[1]);
	m_points->GetPoint(1, tmpv);
	v2.setX(tmpv[0]);
	v2.setY(tmpv[1]);

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

class DistanceMeasureCopyEditPropertyCommand : public QUndoCommand
{
public:
	DistanceMeasureCopyEditPropertyCommand(const QString& name, const QVector2D& v1, const QVector2D& v2, bool showLabel, DistanceMeasureCopyPropertyDialog::LabelPosition lp, DistanceMeasureCopyPropertyDialog::LabelMode lm, int fontSize, const QString& customLabel, bool showMarker, int markerSize, QColor color, DistanceMeasureDataItem* item)
		: QUndoCommand(QObject::tr("Edit Distance Measure Property")) {

		m_item = item;

		m_newName = name;
		m_newPoint1 = v1;
		m_newPoint2 = v2;
		m_newShowLabel = showLabel;
		m_newLabelPosition = lp;
		m_newLabelMode = lm;
		m_newFontSize = fontSize;
		m_newCustomLabel = customLabel;
		m_newShowMarker = showMarker;
		m_newMarkerSize = markerSize;
		m_newColor = color;

		m_oldName = m_item->standardItem()->text();
		m_oldPoint1 = m_item->m_point1;
		m_oldPoint2 = m_item->m_point2;
		m_oldShowLabel = m_item->m_showLabel;
		m_oldLabelPosition = m_item->m_labelPosition;
		m_oldLabelMode = m_item->m_labelMode;
		m_oldFontSize = m_item->m_fontSize;
		m_oldCustomLabel = m_item->m_customLabel;
		m_oldShowMarker = m_item->m_showMarkers;
		m_oldMarkerSize = m_item->m_markerSize;
		m_oldColor = m_item->m_color;
	}
	void undo() {
		m_item->m_isCommandExecuting = true;
		m_item->standardItem()->setText(m_oldName);
		m_item->m_point1 = m_oldPoint1;
		m_item->m_point2 = m_oldPoint2;
		m_item->m_showLabel = m_oldShowLabel;
		m_item->m_labelPosition = m_oldLabelPosition;
		m_item->m_labelMode = m_oldLabelMode;
		m_item->m_fontSize = m_oldFontSize;
		m_item->m_customLabel = m_oldCustomLabel;
		m_item->m_showMarkers = m_oldShowMarker;
		m_item->m_markerSize = m_oldMarkerSize;
		m_item->m_color = m_oldColor;
		m_item->m_isCommandExecuting = false;

		m_item->updateActorSettings();
		m_item->renderGraphicsView();
	}
	void redo() {
		m_item->m_isCommandExecuting = true;
		m_item->standardItem()->setText(m_newName);
		m_item->m_point1 = m_newPoint1;
		m_item->m_point2 = m_newPoint2;
		m_item->m_showLabel = m_newShowLabel;
		m_item->m_labelPosition = m_newLabelPosition;
		m_item->m_labelMode = m_newLabelMode;
		m_item->m_fontSize = m_newFontSize;
		m_item->m_customLabel = m_newCustomLabel;
		m_item->m_showMarkers = m_newShowMarker;
		m_item->m_markerSize = m_newMarkerSize;
		m_item->m_color = m_newColor;
		m_item->m_isCommandExecuting = false;

		m_item->updateActorSettings();
		m_item->renderGraphicsView();
	}
private:
	QString m_newName;
	QVector2D m_newPoint1;
	QVector2D m_newPoint2;
	bool m_newShowLabel;
	DistanceMeasureCopyPropertyDialog::LabelPosition m_newLabelPosition;
	DistanceMeasureCopyPropertyDialog::LabelMode m_newLabelMode;
	int m_newFontSize;
	QString m_newCustomLabel;
	bool m_newShowMarker;
	int m_newMarkerSize;
	QColor m_newColor;

	QString m_oldName;
	QVector2D m_oldPoint1;
	QVector2D m_oldPoint2;
	bool m_oldShowLabel;
	DistanceMeasureCopyPropertyDialog::LabelPosition m_oldLabelPosition;
	DistanceMeasureCopyPropertyDialog::LabelMode m_oldLabelMode;
	int m_oldFontSize;
	QString m_oldCustomLabel;
	bool m_oldShowMarker;
	int m_oldMarkerSize;
	QColor m_oldColor;

	DistanceMeasureDataItem* m_item;
};


void DistanceMeasureDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	DistanceMeasureCopyPropertyDialog* dialog = dynamic_cast<DistanceMeasureCopyPropertyDialog*>(propDialog);
	iRICUndoStack::instance().push(new DistanceMeasureCopyEditPropertyCommand(dialog->name(), dialog->point1(), dialog->point2(),
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

class DistanceMeasureRedefineCommand : public QUndoCommand
{
public:
	DistanceMeasureRedefineCommand(DistanceMeasureDataItem* item)
		: QUndoCommand(QObject::tr("Redefine Distance Measure")) {
		m_item = item;
	}
	void undo() {
		m_item->m_defined = true;
		m_item->m_mouseEventMode = DistanceMeasureDataItem::meNormal;

		m_item->updateActorSettings();
		m_item->renderGraphicsView();
	}
	void redo() {
		m_item->m_defined = false;
		m_item->m_mouseEventMode = DistanceMeasureDataItem::meBeforeDefining;

		m_item->updateMouseCursor();
		m_item->updateActorSettings();
		m_item->renderGraphicsView();
	}

private:
	DistanceMeasureDataItem* m_item;
};

void DistanceMeasureDataItem::redefine()
{
	iRICUndoStack::instance().push(new DistanceMeasureRedefineCommand(this));
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

