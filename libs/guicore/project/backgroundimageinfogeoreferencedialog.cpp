#include "backgroundimageinfogeoreferencedialog.h"
#include "backgroundimageinfo.h"
#include "addiblegcptablemodel.h"
#include "gcptablemodel.h"
#include "gcptablerow.h"
#include "ui_backgroundimageinfogeoreferencedialog.h"

#include "private/backgroundimageinfo_impl.h"
#include "private/backgroundimageinfogeoreferencedialog_styleditemdelegate.h"

#include <QPushButton>
#include <QItemSelectionModel>

#include <array>
#include <functional>

#define _USE_MATH_DEFINES
#include <math.h>

BackgroundImageInfoGeoreferenceDialog::BackgroundImageInfoGeoreferenceDialog(BackgroundImageInfo* info, QWidget* parent) :
	QDialog {parent, Qt::WindowTitleHint | Qt::WindowCloseButtonHint},
	m_setting {info->impl->m_setting},
	m_originalSetting {info->impl->m_setting},
	m_info {info},
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

void BackgroundImageInfoGeoreferenceDialog::calculate()
{
	auto table = ui->imageWidget->gcpTable();

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
		auto row_sourceX = row.sourceX;
		auto row_sourceY = row.sourceY;
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

	m_setting.angle = k * 180 / M_PI;

	QPixmap pixmap {m_info->fullFileName()};

	m_setting.positionX = c + s * pixmap.height() * std::sin(k); // (c, d) is the top-left coordinate.
	m_setting.positionY = d - s * pixmap.height() * std::cos(k); //
	m_setting.scale = s;
}

void BackgroundImageInfoGeoreferenceDialog::reject()
{
	m_info->impl->m_setting = m_originalSetting;
	m_info->informChange();

	QDialog::reject();
}

BackgroundImageInfo::Setting BackgroundImageInfoGeoreferenceDialog::setting() const
{
	return m_setting;
}

void BackgroundImageInfoGeoreferenceDialog::accept()
{
	calculate();

	QDialog::accept();
}

void BackgroundImageInfoGeoreferenceDialog::apply()
{
	calculate();

	m_info->impl->m_setting = m_setting;
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
