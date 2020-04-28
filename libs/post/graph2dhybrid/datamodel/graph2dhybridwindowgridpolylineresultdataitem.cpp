#include "graph2dhybridwindowgridpolylineresultdataitem.h"

#include <geodata/polyline/geodatapolyline.h>
#include <geodata/polyline/geodatapolylineimplpolyline.h>

Graph2dHybridWindowGridPolylineResultDataItem::Graph2dHybridWindowGridPolylineResultDataItem(const Graph2dHybridWindowResultSetting::Setting& setting, int index, Graph2dWindowDataItem* parent) :
	Graph2dHybridWindowGridAbstractPolylineResultDataItem(setting, index, parent)
{
	auto model = dataModel();
	Q_ASSERT(model);
	const Graph2dHybridWindowResultSetting& s = model->setting();
	Q_ASSERT(s.xAxisMode() == Graph2dHybridWindowResultSetting::xaPolyLine);
	const GeoDataPolyLine* line = s.targetPolyLine();

	m_connections.push_back(connect(line, SIGNAL(modified()), model, SLOT(applySettingsSlot())));
	m_connections.push_back(connect(line, SIGNAL(destroyed()), model, SLOT(targetPolyLineDestroyed())));
}

Graph2dHybridWindowGridPolylineResultDataItem::~Graph2dHybridWindowGridPolylineResultDataItem()
{
	for (const auto& c : m_connections) {
		disconnect(c);
	}
}

std::vector<QPointF> Graph2dHybridWindowGridPolylineResultDataItem::getPolyLine()
{
	const Graph2dHybridWindowResultSetting& s = dataModel()->setting();
	const GeoDataPolyLine *line = s.targetPolyLine();

	GeoDataPolyLineImplPolyLine* impl = line->polyLine();
	return impl->polyLine();
}
