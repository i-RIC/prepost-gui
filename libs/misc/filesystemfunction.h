#ifndef FILESYSTEMFUNCTION_H
#define FILESYSTEMFUNCTION_H

class QString;

#include "misc_global.h"

namespace iRIC {
	/// Mkdir recursively
	bool MISCDLL_EXPORT mkdirRecursively(const QString& dirname);
	/// Remove folder recursively
	bool MISCDLL_EXPORT rmdirRecursively(const QString& dirname);
	/// Copy folder recursively
	bool MISCDLL_EXPORT copyDirRecursively(const QString& from, const QString& to, int depth = 8);
	/// Move folder content between two folders
	bool MISCDLL_EXPORT moveDirContent(const QString& from, const QString& to);
	/// Returns true if there is now file in the dir folder.
	bool MISCDLL_EXPORT isDirEmpty(const QString& dirname);
	/// Returns full path name of a temporary file (that does not exist) in the specified folder
	QString MISCDLL_EXPORT getTempFileName(const QString folder);
}
#endif // FILESYSTEMFUNCTION_H
