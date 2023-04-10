#include "geodatapointmap_impl.h"

#include <QAction>

GeoDataPointmap::Impl::Impl(GeoDataPointmap* pointmap) :
	m_pointsManager {pointmap},
	m_tinManager {&m_pointsManager, pointmap},
	m_polygonsManager {&m_pointsManager, &m_tinManager, pointmap},
	m_activeController {&m_tinManager},
	m_tinNodeMapper {pointmap},
	m_tinCellMapper {pointmap},
	m_templateNodeMapper {pointmap},
	m_templateCellMapper {pointmap},
	m_polygonsNodeMapper {pointmap},
	m_polygonsCellMapper {pointmap},
	m_removeTrianglesWithLongEdgeAction {new QAction(GeoDataPointmap::tr("Remove Triangles &with Long edge..."), pointmap)},
	m_remeshAction {new QAction(GeoDataPointmap::tr("Remesh &TINs"), pointmap)},
	m_modeMenu {new QMenu(GeoDataPointmap::tr("Switch &Mode"))},
	m_pointEditModeAction {new QAction("&Points Edit Mode", pointmap)},
	m_tinEditModeAction {new QAction("&TIN Edit Mode", pointmap)},
	m_polygonsEditModeAction {new QAction("&Polyons Edit Mode", pointmap)},
	m_mergeAction {new QAction(GeoDataPointmap::tr("Mer&ge..."), pointmap)},
	m_displaySettingAction {new QAction(GeoDataPointmap::tr("Display &Setting..."), pointmap)},
	m_mappingSettingAction {new QAction(GeoDataPointmap::tr("&Mapping Setting..."), pointmap)},
	m_rightClickingMenu {new QMenu()},
	m_parent {pointmap}
{
	m_pointsManager.setTinManager(&m_tinManager);
}

GeoDataPointmap::Impl::~Impl()
{
	delete m_rightClickingMenu;
}

void GeoDataPointmap::Impl::setupActions()
{

	GeoDataPointmap::connect(m_removeTrianglesWithLongEdgeAction, &QAction::triggered, [=](bool){m_parent->removeTrianglesWithLongEdgeStart();});
	GeoDataPointmap::connect(m_displaySettingAction, &QAction::triggered, [=](bool){m_parent->openDisplaySettingDialog();});
	GeoDataPointmap::connect(m_mappingSettingAction, &QAction::triggered, [=](bool){m_parent->openMappingSettingDialog();});
	GeoDataPointmap::connect(m_remeshAction, &QAction::triggered, [=](bool){m_parent->remeshTin();});

	m_modeMenu->addAction(m_pointEditModeAction);
	m_modeMenu->addAction(m_tinEditModeAction);
	m_modeMenu->addAction(m_polygonsEditModeAction);

	m_pointEditModeAction->setCheckable(true);
	connect(m_pointEditModeAction, &QAction::triggered, m_parent, &GeoDataPointmap::togglePointsEditMode);

	m_tinEditModeAction->setCheckable(true);
	connect(m_tinEditModeAction, &QAction::triggered, m_parent, &GeoDataPointmap::toggleTinEditMode);

	m_polygonsEditModeAction->setCheckable(true);
	connect(m_polygonsEditModeAction, &QAction::triggered, m_parent, &GeoDataPointmap::togglePolyonsEditMode);

	m_tinEditModeAction->setChecked(true);

	GeoDataPointmap::connect(m_mergeAction, SIGNAL(triggered()), m_parent, SLOT(mergePointmaps()));
}
