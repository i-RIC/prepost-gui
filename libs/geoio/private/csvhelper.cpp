#include "csvhelper.h"

#include <QString>
#include <QStringList>

CsvHelper::ErrorCode CsvHelper::getXY(const QString& line, double *x, double *y)
{
	QStringList pieces = line.split(',');

	if (pieces.size() < 2) {
		return NotTwoColumns;
	}
	bool x_ok, y_ok;
	*x = pieces.at(0).toDouble(&x_ok);
	*y = pieces.at(1).toDouble(&y_ok);

	if (! x_ok || ! y_ok) {
		return NotRealNumbers;
	}

	return NoError;
}

int CsvHelper::realNumberPrecision()
{
	return 12;
}

CsvHelper::CsvHelper()
{}
