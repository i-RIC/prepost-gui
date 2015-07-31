#ifndef ARROWSETTINGCONTAINER_H
#define ARROWSETTINGCONTAINER_H

#include "misc_global.h"
#include "compositecontainer.h"
#include "intcontainer.h"

/// Container for arrow shape settings
class MISCDLL_EXPORT ArrowSettingContainer : public CompositeContainer
{
	static const int DEFAULT_ARROWSIZE;
	static const int DEFAULT_LINEWIDTH;

public:
	/// Constructor
	ArrowSettingContainer();
	/// Constructor (copy)
	ArrowSettingContainer(const ArrowSettingContainer& c);
	virtual ~ArrowSettingContainer() {}
	/// The size of cone of arrow (in pixels)
	int arrowSize() const {return m_arrowSize;}
	/// The width of arrow axis (in pixels)
	int lineWidth() const {return m_lineWidth;}
	/// Set the size of cone of arrow (in pixels)
	void setArrowSize(int size) {m_arrowSize = size;}
	/// Set the width of arrow axis (in pixels)
	void setLineWidth(int width) {m_lineWidth = width;}

private:
	IntContainer m_arrowSize;
	IntContainer m_lineWidth;
};

#endif // ARROWSETTINGCONTAINER_H
