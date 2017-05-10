#include "polylinecsvimporter.h"
#include "private/csvhelper.h"

#include <misc/standarderrormessages.h>

#include <QFile>
#include <QMessageBox>
#include <QTextStream>

std::vector<QPointF> PolylineCsvImporter::importData(const QString& filename, const QString& /*selectedFilter*/, QWidget* w)
{
	std::vector<QPointF> ret, errorRet;

	QFile file(filename);
	bool ok = file.open(QFile::ReadOnly);
	if (! ok) {
		QMessageBox::critical(w, tr("Warning"), StandardErrorMessages::fileOpenError(filename));
		return ret;
	}
	QTextStream s(&file);

	double x, y;

	// first line may be header.
	QString line = s.readLine();
	CsvHelper::ErrorCode ec = CsvHelper::getXY(line, &x, &y);

	switch (ec) {
	case CsvHelper::NotTwoColumns:
		QMessageBox::critical(w, tr("Error"), tr("Data need two columns."));
		return errorRet;
		break;

	case CsvHelper::NotRealNumbers:
		// Header. Skip this line.
		break;

	case CsvHelper::NoError:
		ret.push_back(QPointF(x, y));
		break;
	}

	do {
		line = s.readLine();
		if (! line.isEmpty()) {
			QStringList pieces = line.split(',');

			CsvHelper::ErrorCode ec = CsvHelper::getXY(line, &x, &y);

			switch (ec) {
			case CsvHelper::NotTwoColumns:
				QMessageBox::critical(w, tr("Error"), tr("Data need two columns."));
				return errorRet;
				break;

			case CsvHelper::NotRealNumbers:
				QMessageBox::critical(w, tr("Error"), tr("Data must be real numbers."));
				return errorRet;
				break;

			case CsvHelper::NoError:
				ret.push_back(QPointF(x, y));
				break;
			}
		}
	} while (! line.isEmpty());

	return ret;
}

QStringList PolylineCsvImporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("CSV file (*.csv)");

	return ret;
}

QStringList PolylineCsvImporter::acceptableExtensions()
{
	QStringList ret;
	ret << "csv";

	return ret;
}
