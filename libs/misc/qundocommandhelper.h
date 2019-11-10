#ifndef QUNDOCOMMANDHELPER_H
#define QUNDOCOMMANDHELPER_H

#include "misc_global.h"
class QString;

namespace iRIC
{
	int MISCDLL_EXPORT generateCommandId(const QString& commandName);
}

#endif // QUNDOCOMMANDHELPER_H
