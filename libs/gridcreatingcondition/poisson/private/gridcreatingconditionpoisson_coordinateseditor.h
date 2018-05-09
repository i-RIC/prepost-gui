#ifndef GRIDCREATINGCONDITIONPOISSON_COORDINATESEDITOR_H
#define GRIDCREATINGCONDITIONPOISSON_COORDINATESEDITOR_H

#include "../gridcreatingconditionpoisson.h"

class PolyLineController;

class QString;

class GridCreatingConditionPoisson::CoordinatesEditor
{
public:
	static void edit(PolyLineController* line, const QString& lineName, GridCreatingConditionPoisson* condition);

private:
	static void acceptCallback(const std::vector<QPointF>& coords);
	static void rejectCallback();
	static void applyCallback(const std::vector<QPointF>& coords);
	static std::vector<QPointF> applyOffset(const std::vector<QPointF>& coords);
	static std::vector<QPointF> unapplyOffset(const std::vector<QPointF>& coords);

	static PolyLineController* targetLine;
	static GridCreatingConditionPoisson* targetCondition;
	static QString lineName;
	static bool applied;
};

#endif // GRIDCREATINGCONDITIONPOISSON_COORDINATESEDITOR_H
