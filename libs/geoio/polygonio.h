#ifndef POLYGONIO_H
#define POLYGONIO_H

#include "geoio_global.h"

class QPolygonF;
class QWidget;

class GEOIODLL_EXPORT PolygonIO
{
public:
	static QPolygonF importData(QWidget* w);
	static void exportData(const QPolygonF& polygon, QWidget* w);

private:
	PolygonIO();
};

#endif // POLYGONIO_H
