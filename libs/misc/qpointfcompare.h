#ifndef QPOINTFCOMPARE_H
#define QPOINTFCOMPARE_H

#include "misc_global.h"

class QPointF;

class MISCDLL_EXPORT QPointFCompare
{
public:
	QPointFCompare();

	bool operator()(const QPointF& a, const QPointF& b);
};

#endif // QPOINTFCOMPARE_H
