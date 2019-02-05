#include "backgroundimageinfogeoreferencedialog.h"
#include "backgroundimageinfo.h"
#include "addiblegcptablemodel.h"
#include "gcptablemodel.h"
#include "gcptablerow.h"
#include "ui_backgroundimageinfogeoreferencedialog.h"
#include "private/backgroundimageinfogeoreferencedialog_styleditemdelegate.h"

#include <QPushButton>
#include <QItemSelectionModel>

#include <array>
#include <functional>

#define _USE_MATH_DEFINES
#include <math.h>

BackgroundImageInfoGeoreferenceDialog::BackgroundImageInfoGeoreferenceDialog(BackgroundImageInfo* info, QWidget* parent) :
	QDialog {parent, Qt::WindowTitleHint | Qt::WindowCloseButtonHint},
	m_info {info},
	m_leftbottomX {info->translateX()},
	m_leftbottomY {info->translateY()},
	m_scale {info->scale()},
	m_angle {info->angle()},
	m_origLeftbottomX {info->translateX()},
	m_origLeftbottomY {info->translateY()},
	m_origScale {info->scale()},
	m_origAngle {info->angle()},
	m_gcpTableModel {new AddibleGcpTableModel()},
	ui (new Ui::BackgroundImageInfoGeoreferenceDialog)
{
	ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);

	ui->gcpTableView->setModel(m_gcpTableModel.get());
	ui->gcpTableView->setItemDelegate(new StyledItemDelegate(this));

	ui->gcpTableView->setColumnWidth(0, 20);
	ui->gcpTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
	for (auto i = 1; i < m_gcpTableModel->columnCount(); ++i) {
		ui->gcpTableView->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
	}

	auto s = info->filename().toStdString();
	ui->imageWidget->setInfo(info, m_gcpTableModel.get());
	ui->imageWidget->setDialog(this);

	connect(ui->gcpTableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(handleSelectionChanged(const QItemSelection&, const QItemSelection&)));

	connect(ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
	connect(ui->hideButton, SIGNAL(clicked()), this, SLOT(handleHideButtonClicked()));

	setHideButtonText();
	connect(info, SIGNAL(isVisibilityChanged()), this, SLOT(setHideButtonText()));
}

BackgroundImageInfoGeoreferenceDialog::~BackgroundImageInfoGeoreferenceDialog()
{
	delete ui->gcpTableView->itemDelegate();
	delete ui;
}

void BackgroundImageInfoGeoreferenceDialog::clearSelection()
{
	ui->gcpTableView->selectionModel()->clear();
}

void BackgroundImageInfoGeoreferenceDialog::setSelection(const std::unordered_set<std::vector<GcpTableRow>::size_type>& indices)
{
	for (auto iRow : indices) {
		for (auto iColumn = 0; iColumn < m_gcpTableModel->columnCount(); ++iColumn) {
			auto index = m_gcpTableModel->index(iRow, iColumn);
			ui->gcpTableView->selectionModel()->select(index, QItemSelectionModel::Select);
		}
	}
}

std::vector<GcpTableRow>* BackgroundImageInfoGeoreferenceDialog::gcpTable()
{
	return ui->imageWidget->gcpTable();
}

AddibleGcpTableModel* BackgroundImageInfoGeoreferenceDialog::gcpTableModel()
{
	return m_gcpTableModel.get();
}

double BackgroundImageInfoGeoreferenceDialog::leftBottomX() const
{
	return m_leftbottomX;
}

double BackgroundImageInfoGeoreferenceDialog::leftBottomY() const
{
	return m_leftbottomY;
}

double BackgroundImageInfoGeoreferenceDialog::scale() const
{
	return m_scale;
}

double BackgroundImageInfoGeoreferenceDialog::angle() const
{
	return m_angle;
}

double BackgroundImageInfoGeoreferenceDialog::origLeftBottomX() const
{
	return m_origLeftbottomX;
}

double BackgroundImageInfoGeoreferenceDialog::origLeftBottomY() const
{
	return m_origLeftbottomY;
}

double BackgroundImageInfoGeoreferenceDialog::origScale() const
{
	return m_origScale;
}

double BackgroundImageInfoGeoreferenceDialog::origAngle() const
{
	return m_origAngle;
}

void BackgroundImageInfoGeoreferenceDialog::calculate()
{
	auto table = ui->imageWidget->gcpTable();
	auto resizeScale = m_info->resizeScale();

	// Helmert transformation.
	auto n = table->size();

	decltype(GcpTableRow::sourceX) x = 0.;
	decltype(GcpTableRow::sourceY) y = 0.;
	decltype(GcpTableRow::destX) X = 0.;
	decltype(GcpTableRow::destY) Y = 0.;

	decltype(GcpTableRow::sourceX) x2_plus_y2 = 0.;
	decltype(GcpTableRow::sourceX) xX_plus_yY = 0.;
	decltype(GcpTableRow::sourceX) yX_minus_xY = 0.;

	decltype(GcpTableRow::destX) XAve = 0.;
	decltype(GcpTableRow::destX) YAve = 0.;
	for (const auto& row : *table) {
		XAve += row.destX;
		YAve += row.destY;
	}
	XAve /= n;
	YAve /= n;

	for (const auto& row : *table) {
		auto row_sourceX = row.sourceX * resizeScale;
		auto row_sourceY = row.sourceY * resizeScale;
		x += row_sourceX;
		y += row_sourceY;
		X += (row.destX - XAve);
		Y += (row.destY - YAve);

		x2_plus_y2  += row_sourceX * row_sourceX          + row_sourceY * row_sourceY;
		xX_plus_yY  += row_sourceX * (row.destX - XAve)   + row_sourceY * (row.destY - YAve);
		yX_minus_xY += row_sourceY * (row.destX - XAve)   - row_sourceX * (row.destY - YAve);
	}

	auto a = (x * X + y * Y - n * xX_plus_yY) / (x * x + y * y - n * x2_plus_y2);
	auto b = (y * X - x * Y - n * yX_minus_xY) / (x * x + y * y - n * x2_plus_y2);
	auto c = (X - a * x - b * y) / n;
	auto d = (Y - a * y + b * x) / n;

	c += XAve;
	d += YAve;

	auto s = std::hypot(a, b);
	decltype(GcpTableRow::sourceX) k;
	if (a > 0) {
		k = std::atan(-b / a);
	} else if (a < 0) {
		k = std::atan(-b / a) + M_PI;
	} else {
		k = M_PI * (b > 0 ? 0.5 : -0.5);
	}

	m_angle = k * 180 / M_PI;

	QPixmap pixmap {m_info->name()};
	m_leftbottomX = c + s * pixmap.height() * resizeScale * std::sin(k); // (c, d) is the top-left coordinate.
	m_leftbottomY = d - s * pixmap.height() * resizeScale * std::cos(k); //
	m_scale = s;
}

void BackgroundImageInfoGeoreferenceDialog::reject()
{
	m_info->m_translateX = m_origLeftbottomX;
	m_info->m_translateY = m_origLeftbottomY;
	m_info->m_scale = m_origScale;
	m_info->m_angle = m_origAngle;
	m_info->informChange();

	QDialog::reject();
}

void BackgroundImageInfoGeoreferenceDialog::accept()
{
	calculate();

	QDialog::accept();
}

void BackgroundImageInfoGeoreferenceDialog::apply()
{
	calculate();

	m_info->m_translateX = m_leftbottomX;
	m_info->m_translateY = m_leftbottomY;
	m_info->m_scale = m_scale;
	m_info->m_angle = m_angle;
	m_info->informChange();
}

void BackgroundImageInfoGeoreferenceDialog::handleSelectionChanged(const QItemSelection&, const QItemSelection&)
{
	std::unordered_set<std::vector<GcpTableRow>::size_type> rowIndices;

	for (const auto& index : ui->gcpTableView->selectionModel()->selectedIndexes()) {
		auto row = index.row();
		rowIndices.insert(row);
	}

	emit selectionChanged(rowIndices);
}

void BackgroundImageInfoGeoreferenceDialog::handleHideButtonClicked()
{
	m_info->toggleVisibility();
}

void BackgroundImageInfoGeoreferenceDialog::setHideButtonText()
{
	if (m_info->isVisible()) {
		ui->hideButton->setText(tr("Hide"));
	} else {
		ui->hideButton->setText(tr("Show"));
	}
}
