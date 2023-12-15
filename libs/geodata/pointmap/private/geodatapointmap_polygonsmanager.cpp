#include "geodatapointmap_impl.h"
#include "geodatapointmap_polygonsmanager.h"
#include "geodatapointmap_polygonsmanager_attributeformappingcondition.h"
#include "geodatapointmap_polygonsmanager_geodatadataitem.h"
#include "geodatapointmap_polygonsmanager_geodatagroupdataitem.h"
#include "geodatapointmap_polygonsmanager_geodatatopdataitem.h"
#include "geodatapointmap_polygonsmanager_gridtypedataitem.h"
#include "geodatapointmap_polygonsmanager_mappingsetting.h"
#include "geodatapointmap_polygonsmanager_rootdataitem.h"

#include <geodata/polygongroup/geodatapolygongroup.h>
#include <geodata/polygongroup/geodatapolygongrouppolygon.h>
#include <geodata/polygongroup/private/geodatapolygongroup_impl.h>
#include <geodata/polygon/geodatapolygon.h>
#include <guicore/image/imagesettingcontainer.h>
#include <guicore/pre/base/preprocessorgeodatadataitemi.h>
#include <guicore/pre/base/preprocessorgraphicsviewi.h>
#include <guicore/pre/base/preprocessorwindowi.h>
#include <guicore/pre/geodata/geodatafactory.h>
#include <guicore/scalarstocolors/colormaplegendsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <misc/mathsupport.h>

#include <vtkActor2D.h>
#include <vtkProperty.h>

namespace {

const double TINY_DOUBLE = 1.0E-8;
const double MIN_DISTANCE = 1.0E-10;

} // namespace

GeoDataPointmap::PolygonsManager::PolygonsManager(PointsManager* pm, TINManager* tm, GeoDataPointmap* pointmap) :
	m_attribute {new AttributeForMappingCondition()},
	m_rootDataItem {new RootDataItem {m_attribute, pointmap->dataModel(), pointmap->geoDataDataItem()}},
	m_polygonsColorMap {m_attribute->createColorMapSettingContainer()},
	m_polygonsColorMapActor {vtkActor2D::New()},
	m_pointsManager {pm},
	m_tinManager {tm},
	m_parent {pointmap}
{
	m_polygonsColorMap->legendSetting()->setTitle(tr("Mapping Setting"));

	auto dataItem = m_rootDataItem->gridTypeDataItem()->geoDataTopDataItem()->geoDataGroupDataItem()->geoDataDataItem();
	dataItem->setColorMapSettingContainer(m_polygonsColorMap);

	pointmap->renderer()->AddActor2D(m_polygonsColorMapActor);
	m_polygonsColorMap->legendSetting()->imgSetting()->setActor(m_polygonsColorMapActor);
	m_polygonsColorMap->legendSetting()->imgSetting()->controller()->setItem(dataItem);
}

GeoDataPointmap::PolygonsManager::~PolygonsManager()
{
	m_parent->renderer()->RemoveActor2D(m_polygonsColorMapActor);

	delete m_attribute;
	delete m_rootDataItem;
	delete m_polygonsColorMap;
	m_polygonsColorMapActor->Delete();
}

void GeoDataPointmap::PolygonsManager::mergeEditTargetPolygon()
{
	polygonGroup()->mergeEditTargetData();
}

void GeoDataPointmap::PolygonsManager::updatePolygonsActorSettings() const
{
	auto pg = polygonGroup();
	pg->updateActorSetting();
	auto v = m_parent->graphicsView();
	m_polygonsColorMap->legendSetting()->imgSetting()->apply(v);

	const auto& setting = m_parent->impl->m_displaySetting;
	auto da = m_rootDataItem->gridTypeDataItem()->geoDataTopDataItem()->geoDataGroupDataItem()->geoDataDataItem();
	da->setCustomVisibility(setting.polygonsVisible);
}

void GeoDataPointmap::PolygonsManager::loadPolygons(const QString& filename)
{
	polygonGroup()->loadExternalData(filename);
}

void GeoDataPointmap::PolygonsManager::savePolygons(const QString& filename)
{
	polygonGroup()->saveExternalData(filename);
}

void GeoDataPointmap::PolygonsManager::updatePolygonOrder()
{
	polygonGroup()->updateOrder();
}

bool GeoDataPointmap::PolygonsManager::map(double x, double y, double* value)
{
	auto pols = polygonGroup()->polygonsInBoundingBox(x - TINY_DOUBLE, x + TINY_DOUBLE, y - TINY_DOUBLE, y + TINY_DOUBLE);
	std::map<unsigned int, GeoDataPolygonGroupPolygon*> polsMap;
	for (auto p : pols) {
		if (p->isInside(QPointF(x, y))) {
			polsMap.insert({p->order(), p});
		}
	}
	if (polsMap.size() > 0) {
		auto polygon = polsMap.begin()->second;
		MappingSetting ms;
		ms.fromVariant(polygon->value());
		if (ms.type == MappingSetting::Type::TIN) {
			return m_parent->getTinValueAt(x, y, value);
		} else {
			auto points = m_pointsManager->points();
			auto vals = m_pointsManager->values();
			auto ids = m_pointsManager->pointsWithinRadius(QPointF(x, y), ms.radius);
			if (ids.size() > 0) {
				if (ms.type == MappingSetting::Type::Maximum) {
					// max
					double max = 0;
					for (unsigned int i = 0; i < ids.size(); ++i) {
						double v = vals->GetValue(ids.at(i));
						if (i == 0 || v > max) {
							max = v;
						}
					}
					*value = max;
					return true;
				} else if (ms.type == MappingSetting::Type::Minimum) {
					// min
					double min = 0;
					for (unsigned int i = 0; i < ids.size(); ++i) {
						double v = vals->GetValue(ids.at(i));
						if (i == 0 || v < min) {
							min = v;
						}
					}
					*value = min;
					return true;
				} else if (ms.type == MappingSetting::Type::WeightedAverage) {
					// weighted average
					double sum = 0;
					double weightsum = 0;
					double p[3];
					double v;
					for (unsigned int i = 0; i < ids.size(); ++i) {
						points->GetPoint(ids.at(i), p);
						v = vals->GetValue(ids.at(i));
						double distance = iRIC::distance(QPointF(x, y), QPointF(p[0], p[1]));
						if (distance < MIN_DISTANCE) {
							*value = v;
							return true;
						}
						auto distPowered = std::pow(distance, ms.exponent);
						sum += v / distPowered;
						weightsum += 1 / distPowered;
					}
					*value = sum / weightsum;
					return true;
				}
			}
		}
	}
	// map with tin
	return m_parent->getTinValueAt(x, y, value);
}

SolverDefinitionGridAttribute* GeoDataPointmap::PolygonsManager::attribute() const
{
	return m_attribute;
}

GeoDataPolygonGroup* GeoDataPointmap::PolygonsManager::polygonGroup() const
{
	auto geoData = m_rootDataItem->gridTypeDataItem()->geoDataTopDataItem()->geoDataGroupDataItem()->geoDataDataItem()->geoData();
	return dynamic_cast<GeoDataPolygonGroup*> (geoData);
}

ColorMapSettingContainerI* GeoDataPointmap::PolygonsManager::polygonsColorMap() const
{
	return m_polygonsColorMap;
}

void GeoDataPointmap::PolygonsManager::addActionsToMenu(QMenu* menu)
{
	auto pg = polygonGroup();
	menu->addAction(pg->addAction());
	menu->addSeparator();
	menu->addAction(pg->impl->m_dummyPolygonForMenu->editNameAndValueAction());
	menu->addSeparator();
	menu->addAction(pg->impl->m_dummyPolygonForMenu->addVertexAction());
	menu->addAction(pg->impl->m_dummyPolygonForMenu->removeVertexAction());
	menu->addAction(pg->impl->m_dummyPolygonForMenu->coordEditAction());
	menu->addSeparator();
	menu->addAction(pg->impl->m_dummyPolygonForMenu->holeModeAction());
	menu->addAction(pg->impl->m_dummyPolygonForMenu->deleteAction());
	menu->addSeparator();
	auto sortMenu = menu->addMenu(GeoDataPointmap::tr("&Sort"));
	sortMenu->addAction(pg->moveToTopAction());
	sortMenu->addAction(pg->moveUpAction());
	sortMenu->addAction(pg->moveDownAction());
	sortMenu->addAction(pg->moveToBottomAction());
	menu->addSeparator();
	menu->addAction(pg->deleteAction());
}

void GeoDataPointmap::PolygonsManager::activate(VTK2DGraphicsView* /*v*/)
{}

void GeoDataPointmap::PolygonsManager::deactivate(VTK2DGraphicsView* /*v*/)
{}

void GeoDataPointmap::PolygonsManager::handleKeyPressEvent(QKeyEvent* event, VTK2DGraphicsView* v)
{
	polygonGroup()->keyPressEvent(event, dynamic_cast<PreProcessorGraphicsViewI*>(v));
}

void GeoDataPointmap::PolygonsManager::handleKeyReleaseEvent(QKeyEvent* event, VTK2DGraphicsView* v)
{
	polygonGroup()->keyReleaseEvent(event, dynamic_cast<PreProcessorGraphicsViewI*>(v));
}

void GeoDataPointmap::PolygonsManager::handleMouseDoubleClickEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	polygonGroup()->mouseDoubleClickEvent(event, dynamic_cast<PreProcessorGraphicsViewI*>(v));
}

void GeoDataPointmap::PolygonsManager::handleMouseMoveEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	polygonGroup()->mouseMoveEvent(event, dynamic_cast<PreProcessorGraphicsViewI*>(v));
}

void GeoDataPointmap::PolygonsManager::handleMousePressEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	polygonGroup()->mousePressEvent(event, dynamic_cast<PreProcessorGraphicsViewI*>(v));
}

void GeoDataPointmap::PolygonsManager::handleMouseReleaseEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	if (event->button() == Qt::RightButton) {return;}

	polygonGroup()->mouseReleaseEvent(event, dynamic_cast<PreProcessorGraphicsViewI*>(v));
}
