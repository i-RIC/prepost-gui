#include "geodatapointgroup_displaysettingwidget_scalesizepairtablecontroller.h"
#include "geodatapointgroup_displaysettingwidget_scalesizepairtablecontroller_delegate.h"

#include <QPushButton>
#include <QTableView>

GeoDataPointGroup::DisplaySettingWidget::ScaleSizePairTableController::ScaleSizePairTableController(DisplaySettingWidget* widget) :
	QObject {nullptr},
	m_widget {widget},
	m_delegate {new Delegate(this)}
{
	auto t = m_widget->sizesTable();
	t->setModel(&m_model);
	t->setItemDelegate(m_delegate);

	connect(t, &QTableView::clicked, this, &ScaleSizePairTableController::handleItemClick);

	auto selModel = new QItemSelectionModel(&m_model);
	t->setSelectionModel(selModel);
	connect(selModel, &QItemSelectionModel::selectionChanged, this, &ScaleSizePairTableController::handleTableSelectionChange);
}

GeoDataPointGroup::DisplaySettingWidget::ScaleSizePairTableController::~ScaleSizePairTableController()
{
	delete m_delegate;
}

void GeoDataPointGroup::DisplaySettingWidget::ScaleSizePairTableController::applyToTable()
{
	m_model.setColumnCount(2);

	auto& s = m_widget->m_copySetting;
	m_model.setRowCount(static_cast<int> (s.scaleSizePairs.size()));

	m_model.setHeaderData(0, Qt::Orientation::Horizontal, DisplaySettingWidget::tr("Scale"));
	m_model.setHeaderData(1, Qt::Orientation::Horizontal, DisplaySettingWidget::tr("Size (%)"));

	for (int i = 0; i < s.scaleSizePairs.size(); ++i) {
		const auto& pair = s.scaleSizePairs.at(i);
		auto itemV = new QStandardItem();
		itemV->setData(pair.scale.value(), Qt::EditRole);
		m_model.setItem(i, 0, itemV);

		auto itemS = new QStandardItem();
		itemS->setData(pair.size.value(), Qt::EditRole);
		m_model.setItem(i, 1, itemS);
	}
}

void GeoDataPointGroup::DisplaySettingWidget::ScaleSizePairTableController::handleItemClick(const QModelIndex& index)
{
	// discard this
}

void GeoDataPointGroup::DisplaySettingWidget::ScaleSizePairTableController::handleTableSelectionChange(const QItemSelection& selected, const QItemSelection& deselected)
{
	const auto& s = m_widget->m_copySetting;

	auto rows = m_widget->sizesTable()->selectionModel()->selectedRows();
	bool removeOk = (rows.size() > 0 && s.scaleSizePairs.size() - rows.size() >= 2);

	m_widget->removeButton()->setEnabled(removeOk);
}
