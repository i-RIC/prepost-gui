#include "ui_gridcreatingconditiontrianglepolylinecoordinateseditdialog.h"

#include "gridcreatingconditiontriangle.h"
#include "gridcreatingconditiontriangleabstractline.h"
#include "gridcreatingconditiontrianglepolylinecoordinateseditdialog.h"

#include <guibase/realnumbereditwidget.h>
#include <misc/iricundostack.h>

#include <QPolygonF>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QUndoCommand>
#include <QVector2D>
#include <QVector>

#include <vtkPoints.h>
#include <vtkPolyLine.h>

class GridCreatingConditionTrianglePolyLineCoordinatesEditDialogDelegate : public QStyledItemDelegate
{
public:
	GridCreatingConditionTrianglePolyLineCoordinatesEditDialogDelegate(QObject* = nullptr) {}
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/) const {
		RealNumberEditWidget* realEdit = new RealNumberEditWidget(parent);
		return realEdit;
	}
	void setEditorData(QWidget* editor, const QModelIndex& index) const {
		QVariant dat = index.model()->data(index, Qt::DisplayRole);
		RealNumberEditWidget* realEdit = dynamic_cast<RealNumberEditWidget*>(editor);
		realEdit->setValue(dat.toDouble());
	}
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
		// Float. Edit with RealEdit
		RealNumberEditWidget* realEdit = dynamic_cast<RealNumberEditWidget*>(editor);
		model->setData(index, realEdit->value(), Qt::EditRole);
	}

	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const {
		editor->setGeometry(option.rect);
	}
};

class GridCreatingConditionTrianglePolyLineCoordinatesEditCommand : public QUndoCommand
{
public:
	GridCreatingConditionTrianglePolyLineCoordinatesEditCommand(bool apply, QVector<QVector2D> coords, GridCreatingConditionTriangle* pol)
		: QUndoCommand(GridCreatingConditionTriangle::tr("Edit Break Line Coordinates")) {
		m_apply = apply;
		m_newCoords = coords;
		m_polygon = pol;
		m_selectedLine = pol->m_selectedLine;
		double p[3];
		vtkPoints* points = m_selectedLine->getVtkLine()->GetPoints();
		for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
			points->GetPoint(i, p);
			m_oldCoords.append(QVector2D(p[0], p[1]));
		}
	}
	void undo() {
		vtkPoints* points = m_selectedLine->getVtkLine()->GetPoints();
		for (vtkIdType i = 0; i < m_oldCoords.count(); ++i) {
			QVector2D v = m_oldCoords.at(i);
			points->SetPoint(i, v.x(), v.y(), 0);
		}
		points->Modified();
		m_selectedLine->updateShapeData();
		if (! m_apply) {
			m_polygon->renderGraphicsView();
		}
	}
	void redo() {
		vtkPoints* points = m_selectedLine->getVtkLine()->GetPoints();
		for (vtkIdType i = 0; i < m_newCoords.count(); ++i) {
			QVector2D v = m_newCoords.at(i);
			points->SetPoint(i, v.x(), v.y(), 0);
		}
		points->Modified();
		m_selectedLine->updateShapeData();
		m_polygon->renderGraphicsView();
	}
private:
	bool m_apply;
	QVector<QVector2D> m_newCoords;
	QVector<QVector2D> m_oldCoords;
	GridCreatingConditionTriangleAbstractLine* m_selectedLine;
	GridCreatingConditionTriangle* m_polygon;
};

GridCreatingConditionTrianglePolyLineCoordinatesEditDialog::GridCreatingConditionTrianglePolyLineCoordinatesEditDialog(GridCreatingConditionTriangle* polygon, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionTrianglePolyLineCoordinatesEditDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	m_polygon = polygon;
	m_applyed = false;
	setupData();
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));
}

GridCreatingConditionTrianglePolyLineCoordinatesEditDialog::~GridCreatingConditionTrianglePolyLineCoordinatesEditDialog()
{
	delete ui;
}

void GridCreatingConditionTrianglePolyLineCoordinatesEditDialog::accept()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	QVector<QVector2D> coords = getCoords();
	iRICUndoStack::instance().push(new GridCreatingConditionTrianglePolyLineCoordinatesEditCommand(false, coords, m_polygon));
	QDialog::accept();
}

void GridCreatingConditionTrianglePolyLineCoordinatesEditDialog::reject()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
		m_polygon->m_selectedLine->updateShapeData();
		m_polygon->renderGraphicsView();
	}
	QDialog::reject();
}

void GridCreatingConditionTrianglePolyLineCoordinatesEditDialog::apply()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	QVector<QVector2D> coords = getCoords();
	iRICUndoStack::instance().push(new GridCreatingConditionTrianglePolyLineCoordinatesEditCommand(true, coords, m_polygon));
	m_applyed = true;
}

void GridCreatingConditionTrianglePolyLineCoordinatesEditDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void GridCreatingConditionTrianglePolyLineCoordinatesEditDialog::setupData()
{
	// setup model first.
	m_model = new QStandardItemModel(0, 2, this);
	m_model->setHeaderData(0, Qt::Horizontal, tr("X"));
	m_model->setHeaderData(1, Qt::Horizontal, tr("Y"));

	vtkPoints* points = m_polygon->m_selectedLine->getVtkLine()->GetPoints();
	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
		m_model->insertRow(i);
		double p[3];
		points->GetPoint(i, p);
		m_model->setData(m_model->index(i, 0, QModelIndex()), p[0]);
		m_model->setData(m_model->index(i, 1, QModelIndex()), p[1]);
	}
	// set the model into view.
	ui->tableView->setModel(m_model);
	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
		ui->tableView->setRowHeight(i, defaultRowHeight);
	}
	QAbstractItemDelegate* delegate = new GridCreatingConditionTrianglePolyLineCoordinatesEditDialogDelegate(this);
	ui->tableView->setItemDelegate(delegate);
}

QVector<QVector2D> GridCreatingConditionTrianglePolyLineCoordinatesEditDialog::getCoords()
{
	QVector<QVector2D> ret;
	int rows = m_model->rowCount();
	for (int i = 0; i < rows; ++i) {
		double x = m_model->data(m_model->index(i, 0, QModelIndex())).toDouble();
		double y = m_model->data(m_model->index(i, 1, QModelIndex())).toDouble();
		ret.append(QVector2D(x, y));
	}
	return ret;
}
