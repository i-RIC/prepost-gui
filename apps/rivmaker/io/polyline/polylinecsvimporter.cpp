#include "polylinecsvimporter.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QPointF>
#include <QStringList>
#include <QTextStream>

QStringList PolyLineCsvImporter::fileDialogFilters()
{
	QStringList ret;

	ret << tr("CSV file (*.csv)");

	return ret;
}

QStringList PolyLineCsvImporter::acceptableExtensions()
{
	QStringList ret;
	ret << "csv";
	return ret;
}

bool PolyLineCsvImporter::importData(std::vector<QPointF>* polyline, QPointF* offset, const QString& filename, QWidget* w)
{
	QFile file(filename);
	if (! file.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(w, tr("Error"), tr("%1 could not be opened.").arg(QDir::toNativeSeparators(filename)));
		return false;
	}

	QTextStream stream(&file);
	bool first = true;

	do {
		QString str = stream.readLine();
		str.replace(",", " ");

		auto frags = str.split(QRegExp("\\s+"), QString::SkipEmptyParts);
		if (frags.size() < 2) {continue;}

		QTextStream strstream(&str, QIODevice::ReadOnly);
		double x, y;

		if (frags.size() == 2){
			strstream >> x >> y;
		} else {
			continue;
		}

		if (first && offset->x() == 0 && offset->y() == 0) {
			offset->setX(x);
			offset->setY(y);
		}

		QPointF p(x - offset->x(), y - offset->y());
		polyline->push_back(p);

		first = false;
	} while (! stream.atEnd());

	return true;
}
