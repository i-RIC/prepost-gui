#ifndef ZDEPTHRANGE_H
#define ZDEPTHRANGE_H

#include "misc_global.h"

/// ZDepthRange stores the range of Z (depth) value.
class MISCDLL_EXPORT ZDepthRange
{
public:
	ZDepthRange();
	ZDepthRange(double min, double max);

	/// The minimum of the range
	double min() const {return m_min;}
	/// The maximum of the range
	double max() const {return m_max;}
	/// Set minimum of the range
	void setMin(double min) {m_min = min;}
	/// Set maximum of the range
	void setMax(double max) {m_max = max;}
	/// Set the range
	void setRange(double min, double max) {m_min = min; m_max = max;}
	/// The width of the range
	double width() const {return m_max - m_min;}
	/// The count of items in the range
	unsigned int itemCount() const {return m_itemCount;}
	/// Set the count of items in the range
	void setItemCount(unsigned int count) {m_itemCount = count;}

private:
	double m_min;
	double m_max;
	unsigned int m_itemCount;
};

#endif // ZDEPTHRANGE_H
