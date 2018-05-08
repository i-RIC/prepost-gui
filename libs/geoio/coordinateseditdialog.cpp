#include "coordinateseditdialog.h"
#include "ui_coordinateseditdialog.h"

#include "private/coordinateseditdialogdelegate.h"

#include <QStandardItemModel>

CoordinatesEditDialog::CoordinatesEditDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CoordinatesEditDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);

	ui->setupUi(this);
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));
}

CoordinatesEditDialog::~CoordinatesEditDialog()
{
	delete ui;
}

void CoordinatesEditDialog::setCoordinates(const std::vector<QPointF> coords)
{
	m_model = new QStandardItemModel(0, 2, this);
	m_model->setHeaderData(0, Qt::Horizontal, tr("X"));
	m_model->setHeaderData(1, Qt::Horizontal, tr("Y"));

	for (int i = 0; i < coords.size(); ++i) {
		QPointF p = coords[i];
		m_model->insertRow(i);
		m_model->setData(m_model->index(i, 0), p.x());
		m_model->setData(m_model->index(i, 1), p.y());
	}

	ui->tableView->setModel(m_model);
	for (int i = 0; i < coords.size(); ++i) {
		ui->tableView->setRowHeight(i, DEFAULT_ROW_HEIGHT);
	}
	auto* delegate = new CoordinatesEditDialogDelegate(this);
	ui->tableView->setItemDelegate(delegate);
}

void CoordinatesEditDialog::setAcceptCallback(accept_callback callback)
{
	m_acceptCallback = callback;
}

void CoordinatesEditDialog::setRejectCallback(reject_callback callback)
{
	m_rejectCallback = callback;
}

void CoordinatesEditDialog::setApplyCallback(apply_callback callback)
{
	m_applyCallback = callback;
}

void CoordinatesEditDialog::accept()
{
	(*m_acceptCallback)(this->getCoordinates());
	QDialog::accept();
}

void CoordinatesEditDialog::reject()
{
	(*m_rejectCallback)();
	QDialog::reject();
}

void CoordinatesEditDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void CoordinatesEditDialog::apply()
{
	(*m_applyCallback)(this->getCoordinates());
}

std::vector<QPointF> CoordinatesEditDialog::getCoordinates() const
{
	std::vector<QPointF> ret;
	int rows = m_model->rowCount();
	for (int i = 0; i < rows; ++i) {
		double x = m_model->data(m_model->index(i, 0)).toDouble();
		double y = m_model->data(m_model->index(i, 1)).toDouble();
		ret.push_back(QPointF(x, y));
	}
	return ret;
}
