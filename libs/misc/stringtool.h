#ifndef STRINGTOOL_H
#define STRINGTOOL_H

#include "misc_global.h"
#include <string>
#include <QList>

class QString;
class QColor;

namespace iRIC
{
	/// Convert QString into std::string
	std::string MISCDLL_EXPORT toStr(const QString& str);
	/// Returns true when the specified string consists of only ASCII characters
	bool MISCDLL_EXPORT isAscii(const QString& str);
	/// Convert string into color
	QColor MISCDLL_EXPORT QColorFromString(QString qstring);
	/// Convert color into string
	QString MISCDLL_EXPORT QColorToString(QColor qcolor);
	/// Parse csv line and returns the list of column strings
	QList<QString> MISCDLL_EXPORT parseCSVLine(const QString& line);
}

#endif // STRINGTOOL_H
