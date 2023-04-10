#include "geodatapointmap_impl.h"
#include "geodatapointmap_polygonsmanager.h"
#include "geodatapointmap_polygonsmanager_attributeformappingcondition.h"
#include "geodatapointmap_polygonsmanager_mappingsetting.h"

#include <geodata/polygongroup/geodatapolygongroup.h>
#include <geodata/polygongroup/geodatapolygongrouppolygon.h>
#include <geodata/polygongroup/private/geodatapolygongroup_impl.h>
#include <geodata/polygon/geodatapolygon.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/geodata/geodatafactory.h>
#include <misc/mathsupport.h>

#include <vtkProperty.h>

namespace {

const double TINY_DOUBLE = 1.0E-8;
const double MIN_DISTANCE = 1.0E-10;

} // namespace


GeoDataPointmap::PolygonsManager::PolygonsManager(PointsManager* pm, TINManager* tm, GeoDataPointmap* pointmap) :
	m_attribute {new AttributeForMappingCondition()},
	m_polygonGroup {new GeoDataPolygonGroup(pointmap->geoDataDataItem(),
																					GeoDataFactory::instance().creator("integerPolygonGroup"), m_attribute)},
	m_pointsManager {pm},
	m_tinManager {tm},
	m_parent {pointmap}
{}

GeoDataPointmap::PolygonsManager::~PolygonsManager()
{
	delete m_attribute;
	delete m_polygonGroup;
}

void GeoDataPointmap::PolygonsManager::updatePolygonsActorSettings() const
{
	const auto& setting = m_parent->impl->m_displaySetting;
	if (! setting.polygonsVisible) {
		m_polygonGroup->impl->m_paintActor->VisibilityOff();
		m_polygonGroup->impl->m_edgesActor->VisibilityOff();
	} else {
		auto v = m_parent->dataModel()->graphicsView();

		auto col = m_parent->actorCollection();

		m_polygonGroup->impl->m_edgesActor->GetProperty()->SetLineWidth(setting.polygonsLineWidth * v->devicePixelRatioF());
		col->AddItem(m_polygonGroup->impl->m_edgesActor);

		m_polygonGroup->impl->m_paintActor->GetProperty()->SetOpacity(setting.polygonsOpacity);
		col->AddItem(m_polygonGroup->impl->m_paintActor);
	}
}

void GeoDataPointmap::PolygonsManager::loadPolygons(const QString& filename)
{
	m_polygonGroup->loadExternalData(filename);
}

void GeoDataPointmap::PolygonsManager::savePolygons(const QString& filename)
{
	m_polygonGroup->saveExternalData(filename);
}

void GeoDataPointmap::PolygonsManager::updatePolygonOrder()
{
	m_polygonGroup->updateOrder();
}

bool GeoDataPointmap::PolygonsManager::map(double x, double y, double* value)
{
	auto pols = m_polygonGroup->polygonsInBoundingBox(x - TINY_DOUBLE, x + TINY_DOUBLE, y - TINY_DOUBLE, y + TINY_DOUBLE);
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

void GeoDataPointmap::PolygonsManager::addActionsToMenu(QMenu* menu)
{
	menu->addAction(m_polygonGroup->addAction());
	menu->addSeparator();
	menu->addAction(m_polygonGroup->impl->m_dummyPolygonForMenu->editNameAndValueAction());
	menu->addSeparator();
	menu->addAction(m_polygonGroup->impl->m_dummyPolygonForMenu->addVertexAction());
	menu->addAction(m_polygonGroup->impl->m_dummyPolygonForMenu->removeVertexAction());
	menu->addAction(m_polygonGroup->impl->m_dummyPolygonForMenu->coordEditAction());
	menu->addSeparator();
	menu->addAction(m_polygonGroup->impl->m_dummyPolygonForMenu->holeModeAction());
	menu->addAction(m_polygonGroup->impl->m_dummyPolygonForMenu->deleteAction());
	menu->addSeparator();
	auto sortMenu = menu->addMenu(GeoDataPointmap::tr("&Sort"));
	sortMenu->addAction(m_polygonGroup->moveToTopAction());
	sortMenu->addAction(m_polygonGroup->moveUpAction());
	sortMenu->addAction(m_polygonGroup->moveDownAction());
	sortMenu->addAction(m_polygonGroup->moveToBottomAction());
}

void GeoDataPointmap::PolygonsManager::activate(VTK2DGraphicsView* v)
{

}

void GeoDataPointmap::PolygonsManager::deactivate(VTK2DGraphicsView* v)
{

}

void GeoDataPointmap::PolygonsManager::handleKeyPressEvent(QKeyEvent* event, VTK2DGraphicsView* v)
{
	m_polygonGroup->keyPressEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void GeoDataPointmap::PolygonsManager::handleKeyReleaseEvent(QKeyEvent* event, VTK2DGraphicsView* v)
{
	m_polygonGroup->keyReleaseEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void GeoDataPointmap::PolygonsManager::handleMouseDoubleClickEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	m_polygonGroup->mouseDoubleClickEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void GeoDataPointmap::PolygonsManager::handleMouseMoveEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	m_polygonGroup->mouseMoveEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void GeoDataPointmap::PolygonsManager::handleMousePressEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	m_polygonGroup->mousePressEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}

void GeoDataPointmap::PolygonsManager::handleMouseReleaseEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	if (event->button() == Qt::RightButton) {return;}
	m_polygonGroup->mouseReleaseEvent(event, dynamic_cast<PreProcessorGraphicsViewInterface*>(v));
}
