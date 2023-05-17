#ifndef INTEGERRANGECONTAINER_H
#define INTEGERRANGECONTAINER_H

#include "misc_global.h"
#include <vector>

/// Container class to store integer value ranges
class MISCDLL_EXPORT IntegerRangeContainer
{
public:

	class Range
	{
	public:
		unsigned int from;
		unsigned int to;
	};

	const std::vector<Range>& ranges() const;

	void append(unsigned int newval);
	void clear();

private:
	void appendNewRange(unsigned int newval);

	std::vector<Range> m_ranges;
};

#endif // INTEGERRANGECONTAINER_H
