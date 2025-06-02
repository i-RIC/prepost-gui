#ifndef GEODATARIVERSURVEY_EDITJMKDATABYDRAGCOMMAND_H
#define GEODATARIVERSURVEY_EDITJMKDATABYDRAGCOMMAND_H

#include "geodatariversurvey_editjmkdatacommand.h"

class GeoDataRiverSurvey::EditJmkDataByDragCommand : public EditJmkDataCommand
{
public:
	EditJmkDataByDragCommand(bool dragging, int index, bool right, const GeoDataRiverPathPointJmkData& after, const GeoDataRiverPathPointJmkData& before, GeoDataRiverPathPoint* point, GeoDataRiverSurveyCrosssectionWindow* w);

	int id() const override;
	bool mergeWith(const QUndoCommand *other) override;

private:
	bool m_dragging;
	int m_index;
	bool m_right;
};

#endif // GEODATARIVERSURVEY_EDITJMKDATABYDRAGCOMMAND_H
