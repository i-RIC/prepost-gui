#include "ui_coordinateseditdialog.h"
#include "coordinateseditdialog.h"
#include "../data/base/coordinatesi.h"

#include "private/coordinateeditwidget.h"

#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QStyledItemDelegate>
#include <QWidget>

namespace {

const int defaultRowHeight = 20;

class CoordinatesEditDialogDelegate : public QStyledItemDelegate
{
public:
	CoordinatesEditDialogDelegate(QObject* = nullptr) {}
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const
	{
		return new CoordinateEditWidget(parent);
	}
	void setEditorData(QWidget* editor, const QModelIndex& index) const
	{
		QVariant dat = index.model()->data(index, Qt::DisplayRole);
		CoordinateEditWidget* coordEdit = dynamic_cast<CoordinateEditWidget*>(editor);
		coordEdit->setValue(dat.toDouble());
	}
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
	{
		CoordinateEditWidget* coordEdit = dynamic_cast<CoordinateEditWidget*>(editor);
		model->setData(index, coordEdit->value(), Qt::EditRole);
	}

	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex&) const
	{
		editor->setGeometry(option.rect);
	}
};

} // namespace

CoordinatesEditDialog::Impl::Impl(QWidget *w) :
	m_model {w},
	m_target {nullptr}
{}

CoordinatesEditDialog::CoordinatesEditDialog(QWidget* parent) :
	QDialog {parent},
	ui (new Ui::CoordinatesEditDialog),
	impl {new Impl {this}}
{
	ui->setupUi(this);
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));
}

CoordinatesEditDialog::~CoordinatesEditDialog()
{
	delete impl;
	delete ui;
}

void CoordinatesEditDialog::setOffset(const QPointF& offset)
{
	impl->m_offset = offset;
}

void CoordinatesEditDialog::setTarget(CoordinatesI* target)
{
	impl->m_target = target;
	impl->m_oldCoordinates = target->coordinates();
	setupData();
}

void CoordinatesEditDialog::accept()
{
	impl->m_target->setCoordinates(getCoords());
	parentWidget()->update();
	QDialog::accept();
}

void CoordinatesEditDialog::reject()
{
	impl->m_target->setCoordinates(impl->m_oldCoordinates);
	parentWidget()->update();
	QDialog::reject();
}

void CoordinatesEditDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void CoordinatesEditDialog::setupData()
{
	auto coords = impl->m_target->coordinates();
	const auto& offset = impl->m_offset;
	auto& m = impl->m_model;
	m.setColumnCount(2);
	m.setHeaderData(0, Qt::Horizontal, tr("X"));
	m.setHeaderData(1, Qt::Horizontal, tr("Y"));

	for (int i = 0; i < coords.size(); ++i) {
		m.insertRow(i);
		QPointF p = coords.at(i);
		m.setData(m.index(i, 0), p.x() + offset.x());
		m.setData(m.index(i, 1), p.y() + offset.y());
	}
	ui->tableView->setModel(&m);

	for (int i = 0; i < coords.size(); ++i) {
		ui->tableView->setRowHeight(i, defaultRowHeight);
	}
	auto delegate = new CoordinatesEditDialogDelegate(this);
	ui->tableView->setItemDelegate(delegate);
}

void CoordinatesEditDialog::apply()
{
	impl->m_target->setCoordinates(getCoords());
	parentWidget()->update();
}

std::vector<QPointF> CoordinatesEditDialog::getCoords() const
{
	std::vector<QPointF> ret;
	auto& m = impl->m_model;
	const auto& offset = impl->m_offset;
	int rows = m.rowCount();
	for (int i = 0; i < rows; ++i) {
		double x = m.data(m.index(i, 0)).toDouble() - offset.x();
		double y = m.data(m.index(i, 1)).toDouble() - offset.y();
		ret.push_back(QPointF(x, y));
	}
	return ret;
}
