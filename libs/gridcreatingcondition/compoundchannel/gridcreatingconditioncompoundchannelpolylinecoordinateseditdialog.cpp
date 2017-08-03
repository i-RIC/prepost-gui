#include "ui_gridcreatingconditioncompoundchannelpolylinecoordinateseditdialog.h"

#include "gridcreatingconditioncompoundchannel.h"
#include "gridcreatingconditioncompoundchannelabstractline.h"
#include "gridcreatingconditioncompoundchannelpolylinecoordinateseditdialog.h"

#include <guibase/widget/coordinateeditwidget.h>
#include <misc/iricundostack.h>

#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QUndoCommand>
#include <QVector2D>
#include <QVector>

#include <vtkPoints.h>
#include <vtkPolygon.h>

class GridCreatingConditionCompoundChannelPolyLineCoordinatesEditDialogDelegate : public QStyledItemDelegate
{
public:
	GridCreatingConditionCompoundChannelPolyLineCoordinatesEditDialogDelegate(QObject* = nullptr) {}
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

class GridCreatingConditionCompoundChannel::EditPolyLineCoordinatesCommand : public QUndoCommand
{
public:
	EditPolyLineCoordinatesCommand(bool apply, QVector<QVector2D> coords, GridCreatingConditionCompoundChannelAbstractLine* l, GridCreatingConditionCompoundChannel* cond) :
		QUndoCommand {GridCreatingConditionCompoundChannel::tr("Edit Center Line Coordinates")}
	{
		m_apply = apply;
		m_newCoords = coords;
		m_line = l;
		m_condition = cond;
		double p[3];
		vtkPoints* points = m_line->getVtkLine()->GetPoints();
		for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
			points->GetPoint(i, p);
			m_oldCoords.append(QVector2D(p[0], p[1]));
		}
	}
	void undo() {
		vtkPoints* points = m_line->getVtkLine()->GetPoints();
		for (vtkIdType i = 0; i < m_oldCoords.count(); ++i) {
			QVector2D v = m_oldCoords.at(i);
			points->SetPoint(i, v.x(), v.y(), 0);
		}
		points->Modified();
		m_line->updateShapeData();
		if (! m_apply) {
			m_condition->renderGraphicsView();
		}
	}
	void redo() {
		vtkPoints* points = m_line->getVtkLine()->GetPoints();
		for (vtkIdType i = 0; i < m_newCoords.count(); ++i) {
			QVector2D v = m_newCoords.at(i);
			points->SetPoint(i, v.x(), v.y(), 0);
		}
		points->Modified();
		m_line->updateShapeData();
		m_condition->renderGraphicsView();
	}
private:
	bool m_apply;
	QVector<QVector2D> m_newCoords;
	QVector<QVector2D> m_oldCoords;
	GridCreatingConditionCompoundChannelAbstractLine* m_line;
	GridCreatingConditionCompoundChannel* m_condition;
};

GridCreatingConditionCompoundChannelPolyLineCoordinatesEditDialog::GridCreatingConditionCompoundChannelPolyLineCoordinatesEditDialog(GridCreatingConditionCompoundChannelAbstractLine* line, GridCreatingConditionCompoundChannel* condition, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionCompoundChannelPolyLineCoordinatesEditDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	m_line = line;
	m_condition = condition;
	m_applyed = false;
	setupData();
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));
}

GridCreatingConditionCompoundChannelPolyLineCoordinatesEditDialog::~GridCreatingConditionCompoundChannelPolyLineCoordinatesEditDialog()
{
	delete ui;
}

void GridCreatingConditionCompoundChannelPolyLineCoordinatesEditDialog::accept()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	QVector<QVector2D> coords = getCoords();
	iRICUndoStack::instance().push(new GridCreatingConditionCompoundChannel::EditPolyLineCoordinatesCommand(false, coords, m_line, m_condition));
	QDialog::accept();
}

void GridCreatingConditionCompoundChannelPolyLineCoordinatesEditDialog::reject()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
		m_line->updateShapeData();
		m_condition->renderGraphicsView();
	}
	QDialog::reject();
}

void GridCreatingConditionCompoundChannelPolyLineCoordinatesEditDialog::apply()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	QVector<QVector2D> coords = getCoords();
	iRICUndoStack::instance().push(new GridCreatingConditionCompoundChannel::EditPolyLineCoordinatesCommand(true, coords, m_line, m_condition));
	m_applyed = true;
}

void GridCreatingConditionCompoundChannelPolyLineCoordinatesEditDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void GridCreatingConditionCompoundChannelPolyLineCoordinatesEditDialog::setupData()
{
	// setup model first.
	m_model = new QStandardItemModel(0, 2, this);
	m_model->setHeaderData(0, Qt::Horizontal, tr("X"));
	m_model->setHeaderData(1, Qt::Horizontal, tr("Y"));

	vtkPoints* points = m_line->getVtkLine()->GetPoints();
	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
		m_model->insertRow(i);
		double p[3];
		points->GetPoint(i, p);
		m_model->setData(m_model->index(i, 0, QModelIndex()), p[0] + m_condition->offset().x());
		m_model->setData(m_model->index(i, 1, QModelIndex()), p[1] + m_condition->offset().y());
	}
	// set the model into view.
	ui->tableView->setModel(m_model);
	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
		ui->tableView->setRowHeight(i, defaultRowHeight);
	}
	QAbstractItemDelegate* delegate = new GridCreatingConditionCompoundChannelPolyLineCoordinatesEditDialogDelegate(this);
	ui->tableView->setItemDelegate(delegate);
}

QVector<QVector2D> GridCreatingConditionCompoundChannelPolyLineCoordinatesEditDialog::getCoords()
{
	QVector<QVector2D> ret;
	int rows = m_model->rowCount();
	for (int i = 0; i < rows; ++i) {
		double x = m_model->data(m_model->index(i, 0, QModelIndex())).toDouble() - m_condition->offset().x();
		double y = m_model->data(m_model->index(i, 1, QModelIndex())).toDouble() - m_condition->offset().y();
		ret.append(QVector2D(x, y));
	}
	return ret;
}
