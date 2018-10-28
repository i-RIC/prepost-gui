#ifndef GEODATAPOINTMAP_ADDPOINTSETREFERENCECOMMAND_H
#define GEODATAPOINTMAP_ADDPOINTSETREFERENCECOMMAND_H

#include "../geodatapointmap.h"

#include <QUndoCommand>

class GeoDataPointmap::AddPointSetReferenceCommand : public QUndoCommand
{
public:
	AddPointSetReferenceCommand(double value, GeoDataPointmap* p);

	void redo() override;
	void undo() override;

private:
	GeoDataPointmap* m_pointMap;
	double m_value;
};

#endif // GEODATAPOINTMAP_ADDPOINTSETREFERENCECOMMAND_H
