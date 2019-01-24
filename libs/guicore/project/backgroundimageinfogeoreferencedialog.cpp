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

	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(handleAccepted()));
	connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(handleRejected()));
	connect(ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
	connect(ui->hideButton, SIGNAL(clicked()), this, SLOT(handleHideButtonClicked()));

	setHideButtonText();
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

void BackgroundImageInfoGeoreferenceDialog::reject()
{
	m_info->m_translateX = m_origLeftbottomX;
	m_info->m_translateY = m_origLeftbottomY;
	m_info->m_scale = m_origScale;
	m_info->m_angle = m_origAngle;
	m_info->informChange();
	QDialog::reject();
}

void BackgroundImageInfoGeoreferenceDialog::apply()
{
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

void BackgroundImageInfoGeoreferenceDialog::handleAccepted()
{
	closeEvent(nullptr);
}

void BackgroundImageInfoGeoreferenceDialog::handleRejected()
{
	closeEvent(nullptr);
}

void BackgroundImageInfoGeoreferenceDialog::handleHideButtonClicked()
{
	m_info->toggleVisibility();
	setHideButtonText();
}

void BackgroundImageInfoGeoreferenceDialog::closeEvent(QCloseEvent*)
{
	m_info->show();
	m_info->closeGeoreferenceDialog();
}

void BackgroundImageInfoGeoreferenceDialog::setHideButtonText()
{
	if (m_info->isVisible()) {
		ui->hideButton->setText("Hide");
	} else {
		ui->hideButton->setText("Show");
	}
}
