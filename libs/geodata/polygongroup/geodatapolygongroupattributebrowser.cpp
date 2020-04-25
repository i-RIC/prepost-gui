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
	QItemSelection selection;

	m_model->setRowCount(count);

	int offset = 0;
	for (int i = 0; i < selIdx; ++i) {
		auto p = pols.at(i);
		setDataToModel(m_model, p, i, isRef);
		if (m_group->isSelected(p)) {
			selection.merge(QItemSelection(m_model->index(i, 0), m_model->index(i, 2)), QItemSelectionModel::Select);
		}
	}
	if (selPol != nullptr) {
		setDataToModel(m_model, selPol, selIdx, isRef);
		selection.merge(QItemSelection(m_model->index(selIdx, 0), m_model->index(selIdx, 2)), QItemSelectionModel::Select);
		offset = 1;
	}

	for (int i = selIdx; i < pols.size(); ++i) {
		auto p = pols.at(i);
		setDataToModel(m_model, p, i + offset, isRef);
		if (m_group->isSelected(p)) {
			selection.merge(QItemSelection(m_model->index(i + offset, 0), m_model->index(i + offset, 2)), QItemSelectionModel::Select);
		}
	}
	m_selectionModel->select(selection, QItemSelectionModel::ClearAndSelect);

	m_updating = false;
}

void GeoDataPolygonGroupAttributeBrowser::applySelectionToPolygons()
{
	if (m_updating) {return;}

	static QMutex mutex;
	QMutexLocker locker(&mutex);

	m_group->mergeEditTargetPolygon();
	auto& selPols = m_group->impl->m_selectedPolygons;
	selPols.clear();
	auto selRows = m_selectionModel->selectedRows();
	const auto& pols = m_group->allPolygons();

	for (int i = 0; i < selRows.size(); ++i) {
		auto idx = selRows.at(i);
		auto p = pols.at(idx.row());
		selPols.insert(p);
	}
	if (selPols.size() == 1) {
		m_group->impl->setupEditTargetPolygonFromSelectedPolygon();
	}
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

		QModelIndex left, right;
		left = m_model->index(index.row(), 0);
		if (isRef) {
			right = m_model->index(index.row(), 1);
		} else {
			right = m_model->index(index.row(), 2);
		}
		QItemSelection sel(left, right);
		m_selectionModel->select(sel, QItemSelectionModel::ClearAndSelect);
	}
}
