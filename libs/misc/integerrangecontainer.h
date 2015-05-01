#ifndef INTEGERRANGECONTAINER_H
#define INTEGERRANGECONTAINER_H

#include "misc_global.h"
#include <QList>

class MISCDLL_EXPORT IntegerRangeContainer
{
public:
	class Range
	{
	public:
		unsigned int from;
		unsigned int to;
	};

	void append(unsigned int newval);
	void appendNewRange(unsigned int newval);
	const QList<Range>& ranges() const {return m_ranges;}
	void clear();

private:
	QList<Range> m_ranges;

};

#endif // INTEGERRANGECONTAINER_H
