#include "geodatapolydatagroup.h"
#include "geodatapolydatagroupattributebrowser.h"
#include "geodatapolydatagrouppolydata.h"
#include "private/geodatapolydatagroupattributebrowser_tableitemdelegate.h"
#include "private/geodatapolydatagroup_impl.h"

#include <geodata/polydata/geodatapolydata.h>
#include <geodata/polydatagroup/geodatapolydatagroupcreator.h>

#include <QItemSelection>
#include <QItemSelectionModel>
#include <QMutex>
#include <QMutexLocker>
#include <QToolBar>
#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QWidget>

#include <set>

namespace {

void setDataToModel(QStandardItemModel* model, GeoDataPolyDataGroupPolyData* data, int row, bool isRef)
{
	model->setData(model->index(row, 0), data->name());
	if (! isRef) {
		model->setData(model->index(row, 1), data->value());
		model->setData(model->index(row, 2), "view");
	} else {
		model->setData(model->index(row, 1), "view");
	}
}

void setDataToModel(QStandardItemModel* model, GeoDataPolyData* data, int row, bool isRef)
{
	model->setData(model->index(row, 0), data->caption());
	if (! isRef) {
		model->setData(model->index(row, 1), data->variantValue());
		model->setData(model->index(row, 2), "view");
	} else {
		model->setData(model->index(row, 1), "view");
	}
}

} // namespace

GeoDataPolyDataGroupAttributeBrowser::GeoDataPolyDataGroupAttributeBrowser(QWidget *parent) :
	QDockWidget(parent),
	m_mainWidget {new QWidget {this}},
	m_mainWidgetLayout {new QVBoxLayout(m_mainWidget)},
	m_tableView {new QTableView {m_mainWidget}},
	m_model {new QStandardItemModel {this}},
	m_selectionModel {new QItemSelectionModel {m_model}},
	m_tableItemDelegate {new TableItemDelegate(this)},
	m_updating {false}
{
	setWindowTitle(GeoDataPolyDataGroupAttributeBrowser::tr("PolyData Group Attribute Browser"));

	setWidget(m_mainWidget);
	m_mainWidgetLayout->setMargin(0);
	m_mainWidgetLayout->setSpacing(0);
	m_mainWidgetLayout->addWidget(m_tableView);
	m_mainWidget->setLayout(m_mainWidgetLayout);
	m_tableView->setModel(m_model);
	m_tableView->setSelectionModel(m_selectionModel);
	m_tableView->setItemDelegate(m_tableItemDelegate);
	m_tableView->setEditTriggers(QTableView::AllEditTriggers);

	connect(m_selectionModel, SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(applySelectionToPolyData()));
	connect(m_tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(handleItemClick(QModelIndex)));
}

GeoDataPolyDataGroupAttributeBrowser::~GeoDataPolyDataGroupAttributeBrowser()
{}

void GeoDataPolyDataGroupAttributeBrowser::setPolyDataGroup(GeoDataPolyDataGroup* g)
{
	m_group = g;
	m_tableItemDelegate->setPolyDataGroup(g);

	if (g->gridAttribute()->isReferenceInformation()) {
		m_model->setColumnCount(2);
		m_model->setHeaderData(0, Qt::Horizontal, tr("Name"));
		m_model->setHeaderData(1, Qt::Horizontal, tr("Show"));
	} else {
		m_model->setColumnCount(3);
		m_model->setHeaderData(0, Qt::Horizontal, tr("Name"));
		m_model->setHeaderData(1, Qt::Horizontal, tr("Value"));
		m_model->setHeaderData(2, Qt::Horizontal, tr("Show"));
	}

	QToolBar* toolbar = new QToolBar(tr("Sort toolbar"), m_mainWidget);
	toolbar->addAction(g->impl->m_moveToTopAction);
	toolbar->addAction(g->impl->m_moveUpAction);
	toolbar->addAction(g->impl->m_moveDownAction);
	toolbar->addAction(g->impl->m_moveToBottomAction);

	m_mainWidgetLayout->insertWidget(0, toolbar);
}

void GeoDataPolyDataGroupAttributeBrowser::update()
{
	setWindowTitle(tr("%1 Group Attribute Browser").arg(m_group->creator()->shapeNameCamelCase()));

	m_updating = true;

	bool isRef = m_group->gridAttribute()->isReferenceInformation();
	const auto& data = m_group->data();
	auto selData = m_group->editTargetData();
	auto selIdx = m_group->editTargetDataIndex();
	int count = static_cast<int> (data.size());
	if (selData != nullptr) {++count;}

	m_model->setRowCount(count);

	int offset = 0;
	for (int i = 0; i < selIdx; ++i) {
		auto p = data.at(i);
		setDataToModel(m_model, p, i, isRef);
	}
	if (selData != nullptr) {
		setDataToModel(m_model, selData, selIdx, isRef);
		offset = 1;
	}

	for (int i = selIdx; i < data.size(); ++i) {
		auto p = data.at(i);
		setDataToModel(m_model, p, i + offset, isRef);
	}

	updateSelection();

	m_updating = false;
}

void GeoDataPolyDataGroupAttributeBrowser::updateSelection()
{
	bool isRef = m_group->gridAttribute()->isReferenceInformation();
	const auto& data = m_group->data();
	auto selData = m_group->editTargetData();
	auto selIdx = m_group->editTargetDataIndex();
	int count = data.size();
	if (selData != nullptr) {++count;}
	QItemSelection selection;
	int maxcol = 2;
	if (isRef) {maxcol = 1;}

	int offset = 0;
	for (int i = 0; i < selIdx; ++i) {
		auto p = data.at(i);
		if (m_group->isSelected(p)) {
			selection.merge(QItemSelection(m_model->index(i, 0), m_model->index(i, maxcol)), QItemSelectionModel::Select);
		}
	}
	if (selData != nullptr) {
		selection.merge(QItemSelection(m_model->index(selIdx, 0), m_model->index(selIdx, maxcol)), QItemSelectionModel::Select);
		offset = 1;
	}

	for (int i = selIdx; i < data.size(); ++i) {
		auto p = data.at(i);
		if (m_group->isSelected(p)) {
			selection.merge(QItemSelection(m_model->index(i + offset, 0), m_model->index(i + offset, maxcol)), QItemSelectionModel::Select);
		}
	}
	m_selectionModel->select(selection, QItemSelectionModel::ClearAndSelect);
}

void GeoDataPolyDataGroupAttributeBrowser::applySelectionToPolyData()
{
	if (m_updating) {return;}

	if (selectionCoincides()) {return;}

	static QMutex mutex;
	QMutexLocker locker(&mutex);

	m_group->mergeEditTargetData(true);
	auto& selData = m_group->selectedData();
	selData.clear();

	auto indices = m_selectionModel->selectedIndexes();
	const auto& data = m_group->data();

	for (int i = 0; i < indices.size(); ++i) {
		auto idx = indices.at(i);
		auto p = data.at(idx.row());
		selData.insert(p);
	}
	if (selData.size() == 1) {
		m_group->setupEditTargetDataFromSelectedData();
	}

	m_group->updateVtkObjects();
	m_group->updateSelectedDataVtkObjects();
	m_group->updateIndex();

	m_group->impl->updateActionStatus();
	m_group->updateMenu();
	m_group->renderGraphicsView();
}

void GeoDataPolyDataGroupAttributeBrowser::handleItemClick(const QModelIndex& index)
{
	bool isRef = m_group->gridAttribute()->isReferenceInformation();
	if ((isRef && index.column() == 1) || (! isRef && index.column() == 2)) {
		m_group->panTo(index.row());
	}
}

bool GeoDataPolyDataGroupAttributeBrowser::selectionCoincides()
{
	std::set<int> indices;
	for (auto index : m_selectionModel->selectedIndexes()) {
		indices.insert(index.row());
	}
	if (indices.size() == 1) {
		auto idx = *(indices.begin());
		return (m_group->selectedData().size() == 0 && (m_group->editTargetData() != nullptr && m_group->editTargetDataIndex() == idx));
	} else {
		auto data = m_group->data();
		std::unordered_set<GeoDataPolyDataGroupPolyData*> selData;
		if (m_group->editTargetData() != nullptr) {
			bool editTargetSelected = false;
			for (auto idx : indices) {
				if (idx == m_group->editTargetDataIndex()) {
					editTargetSelected = true;
				} else if (idx < m_group->editTargetDataIndex()) {
					selData.insert(data[idx]);
				} else {
					selData.insert(data[idx - 1]);
				}
			}
			return (m_group->selectedData() == selData && editTargetSelected);
		} else {
			for (auto idx : indices) {
				selData.insert(data[idx]);
			}
			return (m_group->selectedData() == selData);
		}
	}
}
