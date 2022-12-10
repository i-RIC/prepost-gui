#ifndef COORDINATESYSTEMCONVERTER_H
#define COORDINATESYSTEMCONVERTER_H

#include <vector>

class CoordinateSystem;
class QPointF;

class CoordinateSystemConverter
{
public:
	CoordinateSystemConverter(const CoordinateSystem& src, const CoordinateSystem& tgt);
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
