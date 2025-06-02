#ifndef GEODATARIVERSURVEY_SETODNPOINTCOMMAND_H
#define GEODATARIVERSURVEY_SETODNPOINTCOMMAND_H

#include "../geodatariversurvey.h"

#include <QUndoCommand>

class GeoDataRiverSurveyCrosssectionWindow;
class PreProcessorGeoDataGroupDataItemI;

class GeoDataRiverSurvey::SetOdnPointCommand : public QUndoCommand
{
public:
	SetOdnPointCommand(GeoDataRiverPathPoint* point, int pointId, int index, GeoDataRiverSurveyCrosssectionWindow* w);

	void redo();
	void undo();

private:
	void apply(int index);

	int m_newIndex;
	int m_oldIndex;

	GeoDataRiverPathPoint* m_point;
	int m_pointId;
	PreProcessorGeoDataGroupDataItemI* m_groupDataItem;
};

#endif // GEODATARIVERSURVEY_SETODNPOINTCOMMAND_H
