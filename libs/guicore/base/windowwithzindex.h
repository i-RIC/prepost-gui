#ifndef WINDOWWITHZINDEX_H
#define WINDOWWITHZINDEX_H

#include "../guicore_global.h"

/// class that has Z Index value.
/**
 * Z Index is the show order in QMdiArea.
 * Window with larger Z Index is shown front.
 */
class GUICOREDLL_EXPORT WindowWithZIndex
{
public:
	/// constructor
	WindowWithZIndex();
	unsigned int zindex() const {return m_zindex;}
	void setZindex(unsigned int i){m_zindex = i;}

protected:
	unsigned int m_zindex;
};

#endif // WINDOWWITHZINDEX_H
