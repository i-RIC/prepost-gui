#ifndef INTEGERRANGECONTAINER_H
#define INTEGERRANGECONTAINER_H

#include "misc_global.h"
#include <QList>

/// Container class to store integer value ranges
class MISCDLL_EXPORT IntegerRangeContainer
{

public:
	/// Container to store integer range
	class Range
	{

	public:
		/// Range minimum value
		unsigned int from;
		/// Range maximum value
		unsigned int to;
	};

	/// Append value to be stored in the container
	void append(unsigned int newval);
	/// Return the list of Range object
	const QList<Range>& ranges() const {return m_ranges;}
	/// Clear the contained values
	void clear();

private:
	void appendNewRange(unsigned int newval);

	QList<Range> m_ranges;

};

#endif // INTEGERRANGECONTAINER_H
