#ifndef V4GRIDSTRUCTURECHECKERI_H
#define V4GRIDSTRUCTURECHECKERI_H

#include "../guicore_global.h"

class v4Grid;

class GUICOREDLL_EXPORT v4GridStructureCheckerI
{
public:
	virtual ~v4GridStructureCheckerI();

	virtual bool check(v4Grid* grid, QStringList* messages, QTextStream* log);
};

#endif // V4GRIDSTRUCTURECHECKERI_H
