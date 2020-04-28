#include "graph2dhybridwindowgridpolylinegrouppolylineresultdataitem.h"

#include <geodata/polylinegroup/geodatapolylinegrouppolyline.h>

#include <geos/geom/LineString.h>

Graph2dHybridWindowGridPolyLineGroupPolyLineResultDataItem::Graph2dHybridWindowGridPolyLineGroupPolyLineResultDataItem(const Graph2dHybridWindowResultSetting::Setting& setting, int index, Graph2dWindowDataItem* parent) :
	Graph2dHybridWindowGridAbstractPolylineResultDataItem(setting, index, parent)
{
	auto model = dataModel();
	Q_ASSERT(model);
	const Graph2dHybridWindowResultSetting& s = model->setting();
	Q_ASSERT(s.xAxisMode() == Graph2dHybridWindowResultSetting::xaPolyLineGroup);
	const GeoDataPolyLineGroupPolyLine* line = s.targetPolyLineGroupPolyLine();

	m_connections.push_back(connect(line, SIGNAL(modified()), model, SLOT(applySettingsSlot())));
	m_connections.push_back(connect(line, SIGNAL(destroyed()), model, SLOT(targetPolyLineDestroyed())));
}

Graph2dHybridWindowGridPolyLineGroupPolyLineResultDataItem::~Graph2dHybridWindowGridPolyLineGroupPolyLineResultDataItem()
{
	for (const auto& c : m_connections) {
		disconnect(c);
	}
}

std::vector<QPointF> Graph2dHybridWindowGridPolyLineGroupPolyLineResultDataItem::getPolyLine()
{
	const Graph2dHybridWindowResultSetting& s = dataModel()->setting();
	const GeoDataPolyLineGroupPolyLine *line = s.targetPolyLineGroupPolyLine();

	auto* ls = line->getGeosLineString();
	std::vector<QPointF> ret;
	ret.reserve(ls->getNumPoints());

	for (int i = 0; i < ls->getNumPoints(); ++i) {
		const auto& coords = ls->getCoordinateN(i);
		ret.push_back(QPointF(coords.x, coords.y));
	}
	return ret;
}
