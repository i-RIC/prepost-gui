#ifndef COORDINATESEDIT_H
#define COORDINATESEDIT_H

#include "geoio_global.h"

#include <vector>

class QPointF;

using accept_callback = void (*)(const std::vector<QPointF>& coords);
using reject_callback = void (*)();
using apply_callback = void (*)(const std::vector<QPointF>& coords);

class QString;
class QWidget;

class GEOIODLL_EXPORT CoordinatesEdit
{
public:
	static void edit(const std::vector<QPointF>& coords, QWidget* parent,
							accept_callback acceptCallback,
							reject_callback rejectCallback,
							apply_callback applyCallback,
							const QString& dialogTitle);
	static std::vector<QPointF> applyOffset(const std::vector<QPointF>& coords, const QPointF& offset);
	static std::vector<QPointF> unapplyOffset(const std::vector<QPointF>& coords, const QPointF& offset);

private:
	CoordinatesEdit();
};

#endif // COORDINATESEDIT_H
