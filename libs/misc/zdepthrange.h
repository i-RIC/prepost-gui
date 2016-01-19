#ifndef ZDEPTHRANGE_H
#define ZDEPTHRANGE_H

#include "misc_global.h"

/// ZDepthRange stores the range of Z (depth) value.
class MISCDLL_EXPORT ZDepthRange
{
public:
	/// Constructor
	ZDepthRange();
	/// Constructor
	ZDepthRange(double min, double max);
	/// Constructor (copy)
	ZDepthRange(const ZDepthRange& range);
	/// Constructor (move)
	ZDepthRange(ZDepthRange&& range);

	/// Destructor
	~ZDepthRange();

	/// Copy operator
	ZDepthRange& operator=(const ZDepthRange& range);
	/// Move operator
	ZDepthRange& operator=(ZDepthRange&& range);

	/// The minimum of the range
	double min() const;
	/// The maximum of the range
	double max() const;
	/// Set minimum of the range
	void setMin(double min);
	/// Set maximum of the range
	void setMax(double max);
	/// Set the range
	void setRange(double min, double max);
	/// The width of the range
	double width() const;
	/// The count of items in the range
	unsigned int itemCount() const;
	/// Set the count of items in the range
	void setItemCount(unsigned int count);

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/zdepthrange_impl.h"
#endif // _DEBUG

#endif // ZDEPTHRANGE_H
