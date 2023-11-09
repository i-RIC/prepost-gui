#ifndef WINDOWWITHZINDEXI_H
#define WINDOWWITHZINDEXI_H

#include "../guicore_global.h"

/// class that has Z Index value.
/**
 * Z Index is the show order in QMdiArea.
 * Window with larger Z Index is shown front.
 */
class GUICOREDLL_EXPORT WindowWithZIndexI
{

public:
	/// constructor
	WindowWithZIndexI();
	unsigned int zindex() const;
	void setZindex(unsigned int i);

private:
	unsigned int m_zindex;
};

#endif // WINDOWWITHZINDEXI_H
