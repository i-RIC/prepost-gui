#ifndef GEODATAPOINTMAP_IMPL_H
#define GEODATAPOINTMAP_IMPL_H

#include "../geodatapointmap.h"

#include <QCursor>
#include <QPixmap>

#include "geodatapointmap_displaysetting.h"
#include "geodatapointmap_mappingsetting.h"
#include "geodatapointmap_pointsmanager.h"
#include "geodatapointmap_polygonscellmapper.h"
#include "geodatapointmap_polygonsnodemapper.h"
#include "geodatapointmap_polygonsmanager.h"
#include "geodatapointmap_riter3dsetting.h"
#include "geodatapointmap_templatecellmapper.h"
#include "geodatapointmap_templatenodemapper.h"
#include "geodatapointmap_tincellmapper.h"
#include "geodatapointmap_tinmanager.h"
#include "geodatapointmap_tinnodemapper.h"

class GeoDataPointmapRiter3dProjectWatcher;

class GeoDataPointmap::Impl
{
public:
	Impl(GeoDataPointmap* pointmap);
	~Impl();

	void setupActions();
	void setupRiter3dWatcher();

	DisplaySetting m_displaySetting;
	MappingSetting m_mappingSetting;
	Riter3dSetting m_riter3dSetting;

	PointsManager m_pointsManager;
	TINManager m_tinManager;
	PolygonsManager m_polygonsManager;
	MouseEventController* m_activeController;

	TinNodeMapper m_tinNodeMapper;
	TinCellMapper m_tinCellMapper;
	TemplateNodeMapper m_templateNodeMapper;
	TemplateCellMapper m_templateCellMapper;
	PolygonsNodeMapper m_polygonsNodeMapper;
	PolygonsCellMapper m_polygonsCellMapper;

	GeoDataPointmapRiter3dProjectWatcher* m_riter3dWatcher;

	// menu related resources
	QAction* m_removeTrianglesWithLongEdgeAction;

	QAction* m_remeshAction;

	QMenu* m_modeMenu;
	QAction* m_pointEditModeAction;
	QAction* m_tinEditModeAction;
	QAction* m_polygonsEditModeAction;

	QAction* m_mergeAction;
	QAction* m_displaySettingAction;
	QAction* m_mappingSettingAction;

	QMenu* m_rightClickingMenu;

private:
	GeoDataPointmap* m_parent;
};

#endif // GEODATAPOINTMAP_IMPL_H
