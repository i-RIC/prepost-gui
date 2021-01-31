#ifndef GEODATAPOLYDATAGROUP_IMPL_H
#define GEODATAPOLYDATAGROUP_IMPL_H

#include"../geodatapolydatagroup.h"

#include <misc/zdepthrange.h>

class GeoDataPolyDataGroupAttributeBrowser;

class QAction;

class GeoDataPolyDataGroup::Impl
{
public:
	Impl(GeoDataPolyDataGroup* group);
	~Impl();

	void clear();

	void updateAttributeBrowser(bool force = false);
	void updateAttributeBrowserSelection();
	void updateActionStatus();

	std::vector<GeoDataPolyDataGroupPolyData*> m_data;
	std::unordered_set<GeoDataPolyDataGroupPolyData*> m_selectedData;

	GeoDataPolyData* m_editTargetData;
	GeoDataPolyDataGroupPolyData* m_editTargetDataBackup;
	int m_editTargetDataIndex;

	GeoDataPolyDataGroupColorSettingDialog::Setting m_colorSetting;

	QAction* m_addAction;
	QAction* m_editNameAction;
	QAction* m_editNameAndValueAction;
	QAction* m_deleteAction;
	QAction* m_mergeAction;

	QAction* m_editColorSettingAction;
	QAction* m_attributeBrowserAction;

	QAction* m_moveToTopAction;
	QAction* m_moveToBottomAction;
	QAction* m_moveUpAction;
	QAction* m_moveDownAction;
	QAction* m_copyAction;

	GeoDataPolyDataGroupAttributeBrowser* m_attributeBrowser;
	bool m_attributeBrowserIsShown;

	Mode m_mode;
	QPoint m_dragStartPoint;
	QMenu* m_rightClickingMenu;

	geos::index::quadtree::Quadtree* m_qTree;
	ZDepthRange m_depthRange;

	GeoDataPolyDataGroup* m_group;
};

#endif // GEODATAPOLYDATAGROUP_IMPL_H

