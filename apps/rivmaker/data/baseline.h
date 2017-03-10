#ifndef BASELINE_H
#define BASELINE_H

#include <QVector2D>

#include <vector>

class CrossSection;

class QPointF;
class BaseLine
{
public:
	BaseLine();
	~BaseLine();

	const std::vector<QPointF>& polyLine() const;
	void setPolyLine(const std::vector<QPointF>& line);

	bool checkIfCrosses(const CrossSection& cs) const;
	void getCrossingPoint(const CrossSection& cs, bool* crosses, double* x, double* y, double* pos);
	void normalizeDirection(CrossSection* cs) const;

	void reverseDirection();

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/baseline_impl.h"
#endif // _DEBUG

#endif // BASELINE_H
