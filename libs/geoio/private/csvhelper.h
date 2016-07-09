#ifndef CSVHELPER_H
#define CSVHELPER_H

#include <QObject>

class QString;
class QWidget;

class CsvHelper : public QObject
{
	Q_OBJECT

public:
	enum ErrorCode {
		NoError,
		NotTwoColumns,
		NotRealNumbers
	};

	static ErrorCode getXY(const QString& line, double *x, double *y);
	static int realNumberPrecision();

private:
	CsvHelper();
};

#endif // CSVHELPER_H
