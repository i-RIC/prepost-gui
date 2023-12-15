#include "abstractcrosssectionwindow_displaysettingtablecontroller.h"
#include "abstractcrosssectionwindow_displaysettingtablecontroller_delegate.h"
#include "abstractcrosssectionwindow_edittablecontroller.h"
#include "abstractcrosssectionwindow_gridattributedisplaysettingeditdialog.h"
#include "abstractcrosssectionwindow_impl.h"

#include <guicore/solverdef/solverdefinitiongridattribute.h>

#include <QStandardItem>

namespace {

const int CHECKBOX_WIDTH = 50;
const int BUTTON_WIDTH = 80;

} // namespace

AbstractCrosssectionWindow::DisplaySettingTableController::DisplaySettingTableController(QTableView* view, Impl* impl) :
	m_view {view},
	m_impl {impl},
	m_model {},
	m_delegate {new Delegate {this}}
{
	view->setModel(&m_model);
	view->setEditTriggers(QTableView::NoEditTriggers);
	view->setItemDelegate(m_delegate);

	connect(view, &QTableView::clicked, this, &DisplaySettingTableController::handleItemClick);
}

void AbstractCrosssectionWindow::DisplaySettingTableController::applyToTable()
{
	m_model.setColumnCount(3);

	m_model.setHeaderData(0, Qt::Orientation::Horizontal, tr("Visible"));
	m_model.setHeaderData(1, Qt::Orientation::Horizontal, tr("Name"));
	m_model.setHeaderData(2, Qt::Orientation::Horizontal, tr("Setting"));

	m_model.setRowCount(m_impl->m_displaySettings.size());
	for (int i = 0; i < static_cast<int> (m_impl->m_displaySettings.size()) ; ++i) {
		const auto& s = m_impl->m_displaySettings.at(i);

		// visible
		auto vItem = new QStandardItem();
		vItem->setData(s.visible.value(), Qt::EditRole);
		m_model.setItem(i, 0, vItem);

		// name
		auto nItem = new QStandardItem();
		auto attName = s.attributeName();
		auto caption = s.captionPrefix + s.attributeDataProvider()->caption(attName);
		nItem->setData(caption, Qt::EditRole);
		m_model.setItem(i, 1, nItem);

		// setting button
		auto sItem = new QStandardItem();
		m_model.setItem(i, 2, sItem);
	}

	m_view->horizontalHeader()->resizeSections(QHeaderView::ResizeMode::ResizeToContents);
}

void AbstractCrosssectionWindow::DisplaySettingTableController::updateVisible()
{
	for (int i = 0; i < m_impl->m_displaySettings.size(); ++i) {

		const auto& s = m_impl->m_displaySettings.at(i);

		// visible
		auto vItem = new QStandardItem();
		vItem->setData(s.visible.value(), Qt::EditRole);
		m_model.setItem(i, 0, vItem);
	}

	m_view->horizontalHeader()->resizeSections(QHeaderView::ResizeMode::ResizeToContents);
}

void AbstractCrosssectionWindow::DisplaySettingTableController::handleItemClick(const QModelIndex& index)
{
	auto& setting = m_impl->m_displaySettings.at(index.row());

	int col = index.column();
	if (col == 1) {return;}

	if (col == 0) {
		setting.visible = ! setting.visible;

		auto vItem = new QStandardItem();
		vItem->setData(setting.visible.value(), Qt::EditRole);
		m_model.setItem(index.row(), 0, vItem);
		m_impl->m_window->update();
	} else if (col == 2) {
		auto dialog = new GridAttributeDisplaySettingEditDialog(&setting, m_impl->m_window);
		auto attName = setting.attributeName();
		auto caption = setting.captionPrefix + setting.attributeDataProvider()->caption(attName);
		dialog->setWindowTitle(tr("Value Display Setting (%1)").arg(caption));
		dialog->setAttribute(Qt::WA_DeleteOnClose);
		dialog->show();
	}
}
