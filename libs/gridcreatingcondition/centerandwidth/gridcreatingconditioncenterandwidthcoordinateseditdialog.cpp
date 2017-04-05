#include "ui_gridcreatingconditioncenterandwidthcoordinateseditdialog.h"

#include "gridcreatingconditioncenterandwidth.h"
#include "gridcreatingconditioncenterandwidthcoordinateseditdialog.h"

#include <guibase/widget/coordinateeditwidget.h>
#include <misc/iricundostack.h>

#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QUndoCommand>
#include <QVector2D>
#include <QVector>

#include <vtkPoints.h>
#include <vtkPolygon.h>

class GridCreatingConditionCenterAndWidthCoordinatesEditDialogDelegate : public QStyledItemDelegate
{
public:
	GridCreatingConditionCenterAndWidthCoordinatesEditDialogDelegate(QObject* = nullptr) {}
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&,
												const QModelIndex&) const {
		CoordinateEditWidget* coordEdit = new CoordinateEditWidget(parent);
		return coordEdit;
	}
	void setEditorData(QWidget* editor, const QModelIndex& index) const {
		QVariant dat = index.model()->data(index, Qt::DisplayRole);
		CoordinateEditWidget* coordEdit = dynamic_cast<CoordinateEditWidget*>(editor);
		coordEdit->setValue(dat.toDouble());
	}
	void setModelData(QWidget* editor, QAbstractItemModel* model,
										const QModelIndex& index) const {
		// Float. Edit with coordEdit
		CoordinateEditWidget* coordEdit = dynamic_cast<CoordinateEditWidget*>(editor);
		model->setData(index, coordEdit->value(), Qt::EditRole);
	}

	void updateEditorGeometry(QWidget* editor,
														const QStyleOptionViewItem& option, const QModelIndex&) const {
		editor->setGeometry(option.rect);
	}
};

class GridCreatingConditionCenterAndWidth::EditCoordinatesCommand : public QUndoCommand
{
public:
	EditCoordinatesCommand(bool apply, std::vector<QPointF> coords, GridCreatingConditionCenterAndWidth* cond) :
		QUndoCommand {GridCreatingConditionCenterAndWidth::tr("Edit Center Line Coordinates")},
		m_apply {apply},
		m_newCoords (coords),
		m_oldCoords (cond->polyLine()),
		m_condition {cond}
	{}
	void redo() {
		m_condition->setPolyLine(m_newCoords);
		m_condition->updateShapeData();
		m_condition->renderGraphicsView();
	}
	void undo() {
		m_condition->setPolyLine(m_oldCoords);
		m_condition->updateShapeData();
		if (! m_apply) {
			m_condition->renderGraphicsView();
		}
	}

private:
	bool m_apply;
	std::vector<QPointF> m_newCoords;
	std::vector<QPointF> m_oldCoords;
	GridCreatingConditionCenterAndWidth* m_condition;
};

GridCreatingConditionCenterAndWidthCoordinatesEditDialog::GridCreatingConditionCenterAndWidthCoordinatesEditDialog(GridCreatingConditionCenterAndWidth* condition, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionCenterAndWidthCoordinatesEditDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	m_condition = condition;
	m_applyed = false;
	setupData();
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));
}

GridCreatingConditionCenterAndWidthCoordinatesEditDialog::~GridCreatingConditionCenterAndWidthCoordinatesEditDialog()
{
	delete ui;
}

void GridCreatingConditionCenterAndWidthCoordinatesEditDialog::accept()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	std::vector<QPointF> coords = getCoords();
	iRICUndoStack::instance().push(new GridCreatingConditionCenterAndWidth::EditCoordinatesCommand(false, coords, m_condition));
	QDialog::accept();
}

void GridCreatingConditionCenterAndWidthCoordinatesEditDialog::reject()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
		m_condition->updateShapeData();
		m_condition->renderGraphicsView();
	}
	QDialog::reject();
}

void GridCreatingConditionCenterAndWidthCoordinatesEditDialog::apply()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	std::vector<QPointF> coords = getCoords();
	iRICUndoStack::instance().push(new GridCreatingConditionCenterAndWidth::EditCoordinatesCommand(true, coords, m_condition));
	m_applyed = true;
}

void GridCreatingConditionCenterAndWidthCoordinatesEditDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void GridCreatingConditionCenterAndWidthCoordinatesEditDialog::setupData()
{
	// setup model first.
	m_model = new QStandardItemModel(0, 2, this);
	m_model->setHeaderData(0, Qt::Horizontal, tr("X"));
	m_model->setHeaderData(1, Qt::Horizontal, tr("Y"));

	auto line = m_condition->polyLine();
	for (int i = 0; i < line.size(); ++i) {
		m_model->insertRow(i);
		m_model->setData(m_model->index(i, 0, QModelIndex()), line[i].x());
		m_model->setData(m_model->index(i, 1, QModelIndex()), line[i].y());
	}
	// set the model into view.
	ui->tableView->setModel(m_model);
	for (int i = 0; i < line.size(); ++i) {
		ui->tableView->setRowHeight(i, defaultRowHeight);
	}
	QAbstractItemDelegate* delegate = new GridCreatingConditionCenterAndWidthCoordinatesEditDialogDelegate(this);
	ui->tableView->setItemDelegate(delegate);
}

std::vector<QPointF> GridCreatingConditionCenterAndWidthCoordinatesEditDialog::getCoords()
{
	std::vector<QPointF> ret;
	int rows = m_model->rowCount();
	for (int i = 0; i < rows; ++i) {
		double x = m_model->data(m_model->index(i, 0, QModelIndex())).toDouble();
		double y = m_model->data(m_model->index(i, 1, QModelIndex())).toDouble();
		ret.push_back(QPointF(x, y));
	}
	return ret;
}
