#ifndef FILESYSTEMFUNCTION_H
#define FILESYSTEMFUNCTION_H

class QString;

#include "misc_global.h"

namespace iRIC {
	/// Mkdir recursively
	bool MISCDLL_EXPORT mkdirRecursively(const QString& dirname);
	/// Remove folder Recursively
	bool MISCDLL_EXPORT rmdirRecursively(const QString& dirname);
	bool MISCDLL_EXPORT copyDirRecursively(const QString& from, const QString& to, int depth = 8);
	bool MISCDLL_EXPORT moveDirContent(const QString& from, const QString& to);
	bool MISCDLL_EXPORT isDirEmpty(const QString& dirname);
	QString MISCDLL_EXPORT getTempFileName(const QString folder);
}
#endif // FILESYSTEMFUNCTION_H
