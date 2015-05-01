#ifndef ZDEPTHRANGE_H
#define ZDEPTHRANGE_H

#include "misc_global.h"

/// ZDepthRange stores the range of Z (depth) value.
class MISCDLL_EXPORT ZDepthRange
{
public:
	ZDepthRange();
	ZDepthRange(double min, double max);

	double min() const {return m_min;}
	double max() const {return m_max;}
	void setMin(double min) {m_min = min;}
	void setMax(double max) {m_max = max;}
	void setRange(double min, double max) {m_min = min; m_max = max;}
	double width() const {return m_max - m_min;}
	unsigned int itemCount() const {return m_itemCount;}
	void setItemCount(unsigned int count) {m_itemCount = count;}

private:
	double m_min;
	double m_max;
	unsigned int m_itemCount;
};

#endif // ZDEPTHRANGE_H
