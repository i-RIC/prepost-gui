#ifndef STRINGTOOL_H
#define STRINGTOOL_H

#include "misc_global.h"
#include <string>
#include <QList>

class QString;
class QColor;

namespace iRIC {
	/// convert QString into std::string
	std::string MISCDLL_EXPORT toStr(const QString& str);
	bool MISCDLL_EXPORT isAscii(const QString& str);
	QColor MISCDLL_EXPORT QColorFromString(QString qstring);
	QString MISCDLL_EXPORT QColorToString(QColor qcolor);
	QList<QString> MISCDLL_EXPORT parseCSVLine(const QString& line);
}

#endif // STRINGTOOL_H
