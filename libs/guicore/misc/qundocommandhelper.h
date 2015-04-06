#ifndef QUNDOCOMMANDHELPER_H
#define QUNDOCOMMANDHELPER_H

#include "../guicore_global.h"
class QString;

namespace iRIC {
	int GUICOREDLL_EXPORT generateCommandId(const QString& commandName);
}

#endif // QUNDOCOMMANDHELPER_H
