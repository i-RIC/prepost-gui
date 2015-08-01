#ifndef WINDOWWITHZINDEXINTERFACE_H
#define WINDOWWITHZINDEXINTERFACE_H

#include "../guicore_global.h"

/// class that has Z Index value.
/**
 * Z Index is the show order in QMdiArea.
 * Window with larger Z Index is shown front.
 */
class GUICOREDLL_EXPORT WindowWithZIndexInterface
{

public:
	/// constructor
	WindowWithZIndexInterface();
	unsigned int zindex() const;
	void setZindex(unsigned int i);

private:
	unsigned int m_zindex;
};

#endif // WINDOWWITHZINDEXINTERFACE_H
