#include "integerrangecontainer.h"

const std::vector<IntegerRangeContainer::Range>& IntegerRangeContainer::ranges() const
{
	return m_ranges;
}

void IntegerRangeContainer::append(unsigned int newval)
{
	if (m_ranges.size() == 0) {
		appendNewRange(newval);
		return;
	}
	Range& lastRange = *m_ranges.rbegin();

	if (lastRange.to == newval - 1) {
		lastRange.to = newval;
	} else {
		appendNewRange(newval);
	}
}

void IntegerRangeContainer::appendNewRange(unsigned int newval)
{
	Range newRange;
	newRange.from = newval;
	newRange.to = newval;
	m_ranges.push_back(newRange);
}

void IntegerRangeContainer::clear()
{
	m_ranges.clear();
}
