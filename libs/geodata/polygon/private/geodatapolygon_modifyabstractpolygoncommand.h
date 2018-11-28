#ifndef GEODATAPOLYGON_MODIFYABSTRACTPOLYGONCOMMAND_H
#define GEODATAPOLYGON_MODIFYABSTRACTPOLYGONCOMMAND_H

#include "../geodatapolygon.h"

#include <QPolygonF>
#include <QUndoCommand>

class GeoDataPolygon::ModifyAbstractPolygonCommand : public QUndoCommand
{
protected:
	ModifyAbstractPolygonCommand(GeoDataPolygonAbstractPolygon* polygon, GeoDataPolygon* p, const QString& text);
	virtual ~ModifyAbstractPolygonCommand();

	void setNewPolygon(const QPolygonF& polygon);
	bool mergeWith(const QUndoCommand *other) override;

public:
	void redo() override;
	void undo() override;

protected:
	QPolygonF m_newPolygon;
	QPolygonF m_oldPolygon;

	bool m_wasMapped;

	GeoDataPolygonAbstractPolygon* m_targetPolygon;
	GeoDataPolygon* m_polygon;
};

#endif // GEODATAPOLYGON_MODIFYABSTRACTPOLYGONCOMMAND_H
