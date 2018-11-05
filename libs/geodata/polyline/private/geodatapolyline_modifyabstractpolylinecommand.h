#ifndef GEODATAPOLYLINE_MODIFYABSTRACTPOLYLINECOMMAND_H
#define GEODATAPOLYLINE_MODIFYABSTRACTPOLYLINECOMMAND_H

#include "../geodatapolyline.h"

#include <QPointF>
#include <QUndoCommand>

#include <vector>

class GeoDataPolyLine::ModifyAbstractPolyLineCommand : public QUndoCommand
{
protected:
	ModifyAbstractPolyLineCommand(GeoDataPolyLine* p, const QString& text);
	virtual ~ModifyAbstractPolyLineCommand();

	void setNewPolyLine(const std::vector<QPointF>& polyline);
	bool mergeWith(const QUndoCommand *other) override;

public:
	void redo() override;
	void undo() override;

protected:
	std::vector<QPointF> m_newPolyLine;
	std::vector<QPointF> m_oldPolyLine;

	bool m_wasMapped;

	GeoDataPolyLineAbstractPolyLine* m_targetPolyLine;
	GeoDataPolyLine* m_polyline;
};

#endif // GEODATAPOLYLINE_MODIFYABSTRACTPOLYLINECOMMAND_H
