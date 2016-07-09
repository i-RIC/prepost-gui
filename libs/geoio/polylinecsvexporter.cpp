#include "polylinecsvexporter.h"
#include "private/csvhelper.h"

#include <misc/standarderrormessages.h>

#include <QFile>
#include <QMessageBox>
#include <QTextStream>

void PolylineCsvExporter::exportData(const QVector<QPointF>& data, const QString& filename, const QString& /*selectedFilter*/, QWidget* w)
{
	QFile file(filename);
	bool ok = file.open(QFile::WriteOnly);

	if (! ok) {
		QMessageBox::critical(w, tr("Error"), StandardErrorMessages::fileOpenError(filename));
		return;
	}

	QTextStream stream(&file);
	stream.setRealNumberPrecision(CsvHelper::realNumberPrecision());

	stream << "X,Y" << "\r\n";
	for (const QPointF& p : data) {
		stream << p.x() << "," << p.y() << "\r\n";
	}

	file.close();
}

QStringList PolylineCsvExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("CSV file (*.csv)");

	return ret;
}
