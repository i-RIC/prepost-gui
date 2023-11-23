#include "csviohelper.h"

QStringList CsvIoHelper::fromCsv(const QString& line)
{
	// TODO fix this. this is a simple and insufficient implementation.

	return line.split(",");
}

QString CsvIoHelper::toCsv(QStringList values)
{
	// TODO fix this. this is a simple and insufficient implementation.

	return values.join(",");
}

CsvIoHelper::CsvIoHelper()
{}
