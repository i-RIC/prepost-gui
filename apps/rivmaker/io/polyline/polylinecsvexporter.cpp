#include "polylinecsvexporter.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QPointF>
#include <QStringList>
#include <QTextStream>

QStringList PolyLineCsvExporter::fileDialogFilters()
{
	QStringList ret;

	ret << tr("CSV file (*.csv)");

	return ret;
}

QStringList PolyLineCsvExporter::acceptableExtensions()
{
	QStringList ret;
	ret << "csv";
	return ret;
}

bool PolyLineCsvExporter::exportData(const std::vector<QPointF>& polyline, const QPointF& offset, const QString& filename, QWidget* w)
{
	QFile file(filename);
	if (! file.open(QIODevice::WriteOnly)) {
		QMessageBox::critical(w, tr("Error"), tr("%1 could not be opened.").arg(QDir::toNativeSeparators(filename)));
		return false;
	}

	QTextStream stream(&file);

	for (QPointF p : polyline) {
		stream << p.x() + offset.x() << "," << p.y() + offset.y() << endl;
	}

	return true;
}
