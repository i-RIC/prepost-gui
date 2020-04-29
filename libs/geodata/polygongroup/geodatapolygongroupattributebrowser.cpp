#include "geodatapolygongroup.h"
#include "geodatapolygongroupattributebrowser.h"
#include "geodatapolygongrouppolygon.h"
#include "private/geodatapolygongroupattributebrowser_tableitemdelegate.h"
#include "private/geodatapolygongroup_impl.h"

#include <geodata/polygon/geodatapolygon.h>

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

void setDataToModel(QStandardItemModel* model, GeoDataPolygonGroupPolygon* pol, int row, bool isRef)
{
	model->setData(model->index(row, 0), pol->name());
	if (! isRef) {
		model->setData(model->index(row, 1), pol->value());
		model->setData(model->index(row, 2), "view");
	} else {
		model->setData(model->index(row, 1), "view");
	}
}

void setDataToModel(QStandardItemModel* model, GeoDataPolygon* pol, int row, bool isRef)
{
	model->setData(model->index(row, 0), pol->caption());
	if (! isRef) {
		model->setData(model->index(row, 1), pol->variantValue());
		model->setData(model->index(row, 2), "view");
	} else {
		model->setData(model->index(row, 1), "view");
	}
}

} // namespace

GeoDataPolygonGroupAttributeBrowser::GeoDataPolygonGroupAttributeBrowser(QWidget *parent) :
	QDockWidget(parent),
	m_mainWidget {new QWidget {this}},
	m_mainWidgetLayout {new QVBoxLayout(m_mainWidget)},
	m_tableView {new QTableView {m_mainWidget}},
	m_model {new QStandardItemModel {this}},
	m_selectionModel {new QItemSelectionModel {m_model}},
	m_tableItemDelegate {new TableItemDelegate(this)},
	m_updating {false}
{
	setWindowTitle(GeoDataPolygonGroupAttributeBrowser::tr("Polygon Group Attribute Browser"));

	setWidget(m_mainWidget);
	m_mainWidgetLayout->setMargin(0);
	m_mainWidgetLayout->setSpacing(0);
	m_mainWidgetLayout->addWidget(m_tableView);
	m_mainWidget->setLayout(m_mainWidgetLayout);
	m_tableView->setModel(m_model);
	m_tableView->setSelectionModel(m_selectionModel);
	m_tableView->setItemDelegate(m_tableItemDelegate);
	m_tableView->setEditTriggers(QTableView::AllEditTriggers);

	connect(m_selectionModel, SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(applySelectionToPolygons()));
	connect(m_tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(handleItemClick(QModelIndex)));
}

GeoDataPolygonGroupAttributeBrowser::~GeoDataPolygonGroupAttributeBrowser()
{}

void GeoDataPolygonGroupAttributeBrowser::setPolygonGroup(GeoDataPolygonGroup* g)
{
	m_group = g;
	m_tableItemDelegate->setPolygonGroup(g);

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

void GeoDataPolygonGroupAttributeBrowser::update()
{
	m_updating = true;

	bool isRef = m_group->gridAttribute()->isReferenceInformation();
	const auto& pols = m_group->allPolygons();
	auto selPol = m_group->editTargetPolygon();
	auto selIdx = m_group->editTargetPolygonIndex();
	int count = pols.size();
	if (selPol != nullptr) {++count;}

	m_model->setRowCount(count);

	int offset = 0;
	for (int i = 0; i < selIdx; ++i) {
		auto p = pols.at(i);
		setDataToModel(m_model, p, i, isRef);
	}
	if (selPol != nullptr) {
		setDataToModel(m_model, selPol, selIdx, isRef);
		offset = 1;
	}

	for (int i = selIdx; i < pols.size(); ++i) {
		auto p = pols.at(i);
		setDataToModel(m_model, p, i + offset, isRef);
	}

	updateSelection();

	m_updating = false;
}

void GeoDataPolygonGroupAttributeBrowser::updateSelection()
{
	bool isRef = m_group->gridAttribute()->isReferenceInformation();
	const auto& pols = m_group->allPolygons();
	auto selPol = m_group->editTargetPolygon();
	auto selIdx = m_group->editTargetPolygonIndex();
	int count = pols.size();
	if (selPol != nullptr) {++count;}
	QItemSelection selection;
	int maxcol = 2;
	if (isRef) {maxcol = 1;}

	int offset = 0;
	for (int i = 0; i < selIdx; ++i) {
		auto p = pols.at(i);
		if (m_group->isSelected(p)) {
			selection.merge(QItemSelection(m_model->index(i, 0), m_model->index(i, maxcol)), QItemSelectionModel::Select);
		}
	}
	if (selPol != nullptr) {
		selection.merge(QItemSelection(m_model->index(selIdx, 0), m_model->index(selIdx, maxcol)), QItemSelectionModel::Select);
		offset = 1;
	}

	for (int i = selIdx; i < pols.size(); ++i) {
		auto p = pols.at(i);
		if (m_group->isSelected(p)) {
			selection.merge(QItemSelection(m_model->index(i + offset, 0), m_model->index(i + offset, maxcol)), QItemSelectionModel::Select);
		}
	}
	m_selectionModel->select(selection, QItemSelectionModel::ClearAndSelect);
}

void GeoDataPolygonGroupAttributeBrowser::applySelectionToPolygons()
{
	if (m_updating) {return;}

	if (selectionCoincides()) {return;}

	static QMutex mutex;
	QMutexLocker locker(&mutex);

	m_group->mergeEditTargetPolygon(true);
	auto& selPols = m_group->impl->m_selectedPolygons;
	selPols.clear();

	auto indices = m_selectionModel->selectedIndexes();
	const auto& pols = m_group->allPolygons();

	for (int i = 0; i < indices.size(); ++i) {
		auto idx = indices.at(i);
		auto p = pols.at(idx.row());
		selPols.insert(p);
	}
	if (selPols.size() == 1) {
		m_group->impl->setupEditTargetPolygonFromSelectedPolygon();
	}
	m_group->updateVtkObjects();
	m_group->updateIndex();
	m_group->impl->updateSelectedPolygonsVtkObjects();
	m_group->impl->updateActionStatus();
	m_group->updateMenu();
	m_group->renderGraphicsView();
}

void GeoDataPolygonGroupAttributeBrowser::handleItemClick(const QModelIndex& index)
{
	bool isRef = m_group->gridAttribute()->isReferenceInformation();
	if ((isRef && index.column() == 1) || (! isRef && index.column() == 2)) {
		m_group->panTo(index.row());
	}
}

bool GeoDataPolygonGroupAttributeBrowser::selectionCoincides()
{
	std::set<int> indices;
	for (auto index : m_selectionModel->selectedIndexes()) {
		indices.insert(index.row());
	}
	if (indices.size() == 1) {
		auto idx = *(indices.begin());
		return (m_group->impl->m_selectedPolygons.size() == 0 && (m_group->impl->m_editTargetPolygon != nullptr && m_group->impl->m_editTargetPolygonIndex == idx));
	} else {
		auto pols = m_group->impl->m_polygons;
		std::unordered_set<GeoDataPolygonGroupPolygon*> selPolygons;
		if (m_group->impl->m_editTargetPolygon != nullptr) {
			bool editTargetSelected = false;
			for (auto idx : indices) {
				if (idx == m_group->impl->m_editTargetPolygonIndex) {
					editTargetSelected = true;
				} else if (idx < m_group->impl->m_editTargetPolygonIndex) {
					selPolygons.insert(pols[idx]);
				} else {
					selPolygons.insert(pols[idx - 1]);
				}
			}
			return (m_group->impl->m_selectedPolygons == selPolygons && editTargetSelected);
		} else {
			for (auto idx : indices) {
				selPolygons.insert(pols[idx]);
			}
			return (m_group->impl->m_selectedPolygons == selPolygons);
		}
	}
}
