#include "post3dwindowcellrangesettingcontainer.h"
#include "post3dwindowcellrangelistsettingeditwidget.h"
#include "ui_post3dwindowcellrangelistsettingeditwidget.h"

Post3dWindowCellRangeListSettingEditWidget::Post3dWindowCellRangeListSettingEditWidget(QWidget *parent) :
	QWidget(parent),
	m_currentRow {-1},
	ui(new Ui::Post3dWindowCellRangeListSettingEditWidget)
{
	ui->setupUi(this);

	connect(ui->rangeAddButton, &QPushButton::clicked, [=](bool) {addRange();});
	connect(ui->rangeRemoveButton, &QPushButton::clicked, [=](bool) {removeRange();});
	connect(ui->rangeListWidget, &QListWidget::currentRowChanged, this, &Post3dWindowCellRangeListSettingEditWidget::setCurrentRange);

	QList<int> sizes;
	sizes.append(50);
	sizes.append(200);
	ui->splitter->setSizes(sizes);
}

Post3dWindowCellRangeListSettingEditWidget::~Post3dWindowCellRangeListSettingEditWidget()
{
	delete ui;
}

void Post3dWindowCellRangeListSettingEditWidget::setDimensions(int* dims)
{
	ui->rangeSettingWidget->setDimensions(dims);
	for (int i = 0; i < 3; ++i) {
		m_gridDimensions[i] = *(dims + i);
	}
}

std::vector<Post3dWindowCellRangeSettingContainer> Post3dWindowCellRangeListSettingEditWidget::ranges()
{
	saveCurrentRange();

	return m_ranges;
}

void Post3dWindowCellRangeListSettingEditWidget::setRanges(const std::vector<Post3dWindowCellRangeSettingContainer>& ranges)
{
	m_ranges = ranges;
	updateRangeList();

	if (m_ranges.size() == 0) {
		ui->rangeSettingWidget->setDisabled(true);
	} else {
		ui->rangeListWidget->setCurrentRow(0);
	}
}

void Post3dWindowCellRangeListSettingEditWidget::addRange()
{
	ui->rangeSettingWidget->setEnabled(true);

	Post3dWindowCellRangeSettingContainer newSetting;

	if (m_ranges.size() == 0) {
		// create default
		newSetting.enabled = true;
		newSetting.iMin = 0;
		newSetting.iMax = m_gridDimensions[0] - 2;
		newSetting.jMin = 0;
		newSetting.jMax = m_gridDimensions[1] - 2;
		newSetting.kMin = 0;
		newSetting.kMax = m_gridDimensions[2] - 2;
	} else {
		// copy current;
		saveCurrentRange();
		newSetting = m_ranges.at(ui->rangeListWidget->currentRow());
	}

	m_ranges.push_back(newSetting);
	updateRangeList();

	int row = m_ranges.size() - 1;
	ui->rangeListWidget->setCurrentRow(row);
	ui->rangeListWidget->scrollToItem(ui->rangeListWidget->item(row));

	setCurrentRange(row);
}

void Post3dWindowCellRangeListSettingEditWidget::removeRange()
{
	if (m_ranges.size() == 0) {return;}

	int row = ui->rangeListWidget->currentRow();
	m_ranges.erase(m_ranges.begin() + row);
	m_currentRow = -1;

	updateRangeList();

	if (m_ranges.size() == 0) {
		Post3dWindowCellRangeSettingContainer dummy;
		ui->rangeSettingWidget->setSetting(dummy);
		ui->rangeSettingWidget->setDisabled(true);
		return;
	} else if (row >= m_ranges.size()) {
		row = m_ranges.size() - 1;
	}
	ui->rangeListWidget->setCurrentRow(row);
}

void Post3dWindowCellRangeListSettingEditWidget::setCurrentRange(int row)
{
	if (m_currentRow == row) {return;}

	saveCurrentRange();

	const auto& rs = m_ranges[row];
	ui->rangeSettingWidget->setSetting(rs);
	m_currentRow = row;
}

void Post3dWindowCellRangeListSettingEditWidget::updateRangeList()
{
	auto w = ui->rangeListWidget;
	w->blockSignals(true);

	w->clear();
	int idx = 1;
	for (auto rs : m_ranges) {
		w->addItem(tr("Range%1").arg(idx));
		++ idx;
	}
	w->blockSignals(false);
}

void Post3dWindowCellRangeListSettingEditWidget::saveCurrentRange()
{
	if (m_currentRow == -1) {return;}

	m_ranges[m_currentRow] = ui->rangeSettingWidget->setting();
}
