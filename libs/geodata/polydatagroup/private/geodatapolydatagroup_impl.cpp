#include "geodatapolydatagroup_impl.h"
#include "../geodatapolydatagroupattributebrowser.h"
#include "../geodatapolydatagrouppolydata.h"

#include <geodata/polydata/geodatapolydata.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>

#include <QAction>
#include <QMenu>

#include <geos/index/quadtree/Quadtree.h>

namespace {

bool checkSelectionContinuous(const std::vector<GeoDataPolyDataGroupPolyData*>& data, const std::unordered_set<GeoDataPolyDataGroupPolyData*>& selectedData, GeoDataPolyData* editTarget)
{
	auto selectedDataCopy = selectedData;

	if (selectedData.size() == 0) {
		return (editTarget != nullptr);
	}

	bool sel = false;
	for (int i = 0; i < data.size(); ++i) {
		auto p = data.at(i);
		auto it = selectedDataCopy.find(p);
		if (it != selectedDataCopy.end()) {
			sel = true;
			selectedDataCopy.erase(it);
		} else {
			if (! sel) {continue;}

			return (selectedDataCopy.size() == 0);
		}
	}
	return true;
}

} // namespace

GeoDataPolyDataGroup::Impl::Impl(GeoDataPolyDataGroup* group) :
	m_editTargetData {nullptr},
	m_editTargetDataBackup {nullptr},
	m_editTargetDataIndex {0},

	m_addAction {new QAction(GeoDataPolyDataGroup::tr("&Add New Polygon..."), group)},
	m_editNameAction {new QAction(GeoDataPolyDataGroup::tr("Edit &Name..."), group)},
	m_editNameAndValueAction {new QAction(GeoDataPolyDataGroup::tr("Edit &Name and Value..."), group)},
	m_deleteAction {new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.png"), GeoDataPolyDataGroup::tr("&Delete"), group)},
	m_mergeAction {new QAction(GeoDataPolyDataGroup::tr("&Merge..."), group)},
	m_editColorSettingAction {new QAction(GeoDataPolyDataGroup::tr("Color &Setting..."), group)},
	m_attributeBrowserAction {new QAction(GeoDataPolyDataGroup::tr("Show Attribute &Browser"), group)},
	m_moveToTopAction {new QAction(QIcon(":/libs/guibase/images/iconItemMoveTop.png"), GeoDataPolyDataGroup::tr("Move to top"), group)},
	m_moveToBottomAction {new QAction(QIcon(":/libs/guibase/images/iconItemMoveBottom.png"), GeoDataPolyDataGroup::tr("Move to bottom"), group)},
	m_moveUpAction {new QAction(QIcon(":/libs/guibase/images/iconItemMoveUp.png"), GeoDataPolyDataGroup::tr("Move up"), group)},
	m_moveDownAction {new QAction(QIcon(":/libs/guibase/images/iconItemMoveDown.png"), GeoDataPolyDataGroup::tr("Move down"), group)},
	m_copyAction {new QAction(GeoDataPolyDataGroup::tr("Copy..."), group)},
	m_attributeBrowser {new GeoDataPolyDataGroupAttributeBrowser(group->preProcessorWindow())},
	m_attributeBrowserIsShown {true},
	m_mode {Mode::Normal},
	m_rightClickingMenu {new QMenu(group->preProcessorWindow())},
	m_qTree {new geos::index::quadtree::Quadtree()},
	m_group {group}
{}

GeoDataPolyDataGroup::Impl::~Impl()
{
	clear();

	delete m_editTargetData;
	delete m_editTargetDataBackup;
	delete m_attributeBrowser;
	delete m_rightClickingMenu;
	delete m_qTree;
}

void GeoDataPolyDataGroup::Impl::clear()
{
	for (auto d : m_data) {
		delete d;
	}
	m_data.clear();
}

void GeoDataPolyDataGroup::Impl::updateAttributeBrowser(bool force)
{
	if (! m_attributeBrowser->isVisible() && ! force) {return;}
	m_attributeBrowser->update();
}

void GeoDataPolyDataGroup::Impl::updateAttributeBrowserSelection()
{
	if (! m_attributeBrowser->isVisible()) {return;}
	m_attributeBrowser->updateSelection();
}

void GeoDataPolyDataGroup::Impl::updateActionStatus()
{
	bool selected = m_selectedData.size() > 0 || (m_editTargetData != nullptr);
	bool continuous = checkSelectionContinuous(m_data, m_selectedData, m_editTargetData);

	m_deleteAction->setEnabled(selected);
	m_moveToTopAction->setEnabled(selected);
	m_moveToBottomAction->setEnabled(selected);
	m_moveUpAction->setEnabled(selected && continuous);
	m_moveDownAction->setEnabled(selected && continuous);
}
