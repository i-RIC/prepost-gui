#include "post2dwindowparticleimagedataitem_settingeditwidget_valuesizepairtablecontroller.h"
#include "post2dwindowparticleimagedataitem_settingeditwidget_valuesizepairtablecontroller_delegate.h"

Post2dWindowParticleImageDataItem::SettingEditWidget::ValueSizePairTableController::ValueSizePairTableController(SettingEditWidget* widget) :
	QObject {nullptr},
	m_widget {widget},
	m_delegate {new Delegate(this)}
{
	auto t = m_widget->sizesTable();
	t->setModel(&m_model);
	t->setItemDelegate(m_delegate);

	connect(t, &QTableView::clicked, this, &ValueSizePairTableController::handleItemClick);

	auto selModel = new QItemSelectionModel(&m_model);
	t->setSelectionModel(selModel);
	connect(selModel, &QItemSelectionModel::selectionChanged, this, &ValueSizePairTableController::handleTableSelectionChange);
}

Post2dWindowParticleImageDataItem::SettingEditWidget::ValueSizePairTableController::~ValueSizePairTableController()
{
	delete m_delegate;
}

void Post2dWindowParticleImageDataItem::SettingEditWidget::ValueSizePairTableController::applyToTable()
{
	m_model.setColumnCount(2);

	auto& s = m_widget->m_setting;
	m_model.setRowCount(static_cast<int> (s.valueSizePairs.size()));

	m_model.setHeaderData(0, Qt::Orientation::Horizontal, SettingEditWidget::tr("Value"));
	m_model.setHeaderData(1, Qt::Orientation::Horizontal, SettingEditWidget::tr("Size (m)"));

	for (int i = 0; i < s.valueSizePairs.size(); ++i) {
		const auto& pair = s.valueSizePairs.at(i);
		auto itemV = new QStandardItem();
		itemV->setData(pair.value.value(), Qt::EditRole);
		m_model.setItem(i, 0, itemV);

		auto itemS = new QStandardItem();
		itemS->setData(pair.size.value(), Qt::EditRole);
		m_model.setItem(i, 1, itemS);
	}
}

void Post2dWindowParticleImageDataItem::SettingEditWidget::ValueSizePairTableController::handleItemClick(const QModelIndex& index)
{
	// discard this
}

void Post2dWindowParticleImageDataItem::SettingEditWidget::ValueSizePairTableController::handleTableSelectionChange(const QItemSelection& selected, const QItemSelection& deselected)
{
	const auto& s = m_widget->m_setting;

	auto rows = m_widget->sizesTable()->selectionModel()->selectedRows();
	bool removeOk = (rows.size() > 0 && s.valueSizePairs.size() - rows.size() >= 2);

	m_widget->removeButton()->setEnabled(removeOk);
}
