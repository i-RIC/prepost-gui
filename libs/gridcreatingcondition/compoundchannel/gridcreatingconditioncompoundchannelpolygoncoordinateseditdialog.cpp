#include "ui_gridcreatingconditioncompoundchannelpolygoncoordinateseditdialog.h"

#include "gridcreatingconditioncompoundchannel.h"
#include "gridcreatingconditioncompoundchannelabstractpolygon.h"
#include "gridcreatingconditioncompoundchannelpolygoncoordinateseditdialog.h"

#include <guibase/coordinateeditwidget.h>
#include <misc/iricundostack.h>

#include <QPolygonF>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QUndoCommand>
#include <QVector2D>
#include <QVector>

#include <vtkPoints.h>
#include <vtkPolygon.h>

class GridCreatingConditionCompoundChannelPolygonCoordinatesEditDialogDelegate : public QStyledItemDelegate
{
public:
	GridCreatingConditionCompoundChannelPolygonCoordinatesEditDialogDelegate(QObject* = nullptr) {}
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& /*option*/,
												const QModelIndex& /*index*/) const {
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
														const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const {
		editor->setGeometry(option.rect);
	}
};

class GridCreatingConditionCompoundChannel::EditPolygonCoordinatesCommand : public QUndoCommand
{
public:
	EditPolygonCoordinatesCommand(bool apply, QVector<QVector2D> coords, GridCreatingConditionCompoundChannelAbstractPolygon* pol, GridCreatingConditionCompoundChannel* cond) :
		QUndoCommand {GridCreatingConditionCompoundChannel::tr("Edit Polygon Coordinates")}
	{
		m_apply = apply;
		m_newCoords = coords;
		m_polygon = pol;
		m_cond = cond;
		double p[3];
		vtkPoints* points = m_polygon->getVtkPolygon()->GetPoints();
		for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
			points->GetPoint(i, p);
			m_oldCoords.append(QVector2D(p[0], p[1]));
		}
	}
	void undo() {
		vtkPoints* points = m_polygon->getVtkPolygon()->GetPoints();
		for (vtkIdType i = 0; i < m_oldCoords.count(); ++i) {
			QVector2D v = m_oldCoords.at(i);
			points->SetPoint(i, v.x(), v.y(), 0);
		}
		points->Modified();
		m_polygon->updateShapeData();
		if (! m_apply) {
			m_cond->renderGraphicsView();
		}
	}
	void redo() {
		vtkPoints* points = m_polygon->getVtkPolygon()->GetPoints();
		for (vtkIdType i = 0; i < m_newCoords.count(); ++i) {
			QVector2D v = m_newCoords.at(i);
			points->SetPoint(i, v.x(), v.y(), 0);
		}
		points->Modified();
		m_polygon->updateShapeData();
		m_cond->renderGraphicsView();
	}
private:
	bool m_apply;
	QVector<QVector2D> m_newCoords;
	QVector<QVector2D> m_oldCoords;
	GridCreatingConditionCompoundChannelAbstractPolygon* m_polygon;
	GridCreatingConditionCompoundChannel* m_cond;
};

GridCreatingConditionCompoundChannelPolygonCoordinatesEditDialog::GridCreatingConditionCompoundChannelPolygonCoordinatesEditDialog(GridCreatingConditionCompoundChannelAbstractPolygon* polygon, GridCreatingConditionCompoundChannel* cond, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionCompoundChannelPolygonCoordinatesEditDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	m_polygon = polygon;
	m_condition = cond;
	m_applyed = false;
	setupData();
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));
}

GridCreatingConditionCompoundChannelPolygonCoordinatesEditDialog::~GridCreatingConditionCompoundChannelPolygonCoordinatesEditDialog()
{
	delete ui;
}

void GridCreatingConditionCompoundChannelPolygonCoordinatesEditDialog::accept()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	QVector<QVector2D> coords = getCoords();
	iRICUndoStack::instance().push(new GridCreatingConditionCompoundChannel::EditPolygonCoordinatesCommand(false, coords, m_polygon, m_condition));
	QDialog::accept();
}

void GridCreatingConditionCompoundChannelPolygonCoordinatesEditDialog::reject()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
		m_polygon->updateShapeData();
		m_condition->renderGraphicsView();
	}
	QDialog::reject();
}

void GridCreatingConditionCompoundChannelPolygonCoordinatesEditDialog::apply()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	QVector<QVector2D> coords = getCoords();
	iRICUndoStack::instance().push(new GridCreatingConditionCompoundChannel::EditPolygonCoordinatesCommand(true, coords, m_polygon, m_condition));
	m_applyed = true;
}

void GridCreatingConditionCompoundChannelPolygonCoordinatesEditDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void GridCreatingConditionCompoundChannelPolygonCoordinatesEditDialog::setupData()
{
	// setup model first.
	m_model = new QStandardItemModel(0, 2, this);
	m_model->setHeaderData(0, Qt::Horizontal, tr("X"));
	m_model->setHeaderData(1, Qt::Horizontal, tr("Y"));

	vtkPoints* points = m_polygon->getVtkPolygon()->GetPoints();
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
	QAbstractItemDelegate* delegate = new GridCreatingConditionCompoundChannelPolygonCoordinatesEditDialogDelegate(this);
	ui->tableView->setItemDelegate(delegate);
}

QVector<QVector2D> GridCreatingConditionCompoundChannelPolygonCoordinatesEditDialog::getCoords()
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
