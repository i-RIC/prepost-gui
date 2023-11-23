#ifndef CSVIOHELPER_H
#define CSVIOHELPER_H

#include "misc_global.h"

#include <QString>
#include <QStringList>

class MISCDLL_EXPORT CsvIoHelper
{
public:
	static QStringList fromCsv(const QString& line);
	static QString toCsv(QStringList values);

private:
	CsvIoHelper();
};

#endif // CSVIOHELPER_H
