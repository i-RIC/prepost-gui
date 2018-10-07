#ifndef GEODATAPOLYLINE_FINISHPOLYLINEDEFINITIONCOMMAND_H
#define GEODATAPOLYLINE_FINISHPOLYLINEDEFINITIONCOMMAND_H

#include "../geodatapolyline.h"

#include <QUndoCommand>

class GeoDataPolyLine::FinishPolyLineDefiningCommand : public QUndoCommand
{
public:
	FinishPolyLineDefiningCommand(GeoDataPolyLine* polyline);

	void redo() override;
	void undo() override;

private:
	GeoDataPolyLine* m_polyline;
	GeoDataPolyLineAbstractPolyLine* m_targetPolyLine;
};

#endif // GEODATAPOLYLINE_FINISHPOLYLINEDEFINITIONCOMMAND_H
