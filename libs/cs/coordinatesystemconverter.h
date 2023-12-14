#ifndef COORDINATESYSTEMCONVERTER_H
#define COORDINATESYSTEMCONVERTER_H

#include "cs_api.h"

#include <vector>

class CoordinateSystem;
class QPointF;

class CS_API CoordinateSystemConverter
{
public:
	CoordinateSystemConverter(CoordinateSystem* src, CoordinateSystem* tgt);
	~CoordinateSystemConverter();

	QPointF convert(const QPointF& point);
	std::vector<QPointF> convert(const std::vector<QPointF>& points);

private:
	class Impl;
	Impl* impl;
};

#if _DEBUG
	#include "private/coordinatesystemconverter_impl.h"
#endif // _DEBUG

#endif // COORDINATESYSTEMCONVERTER_H