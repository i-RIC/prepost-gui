#include "crosssectionscsvimporter.h"
#include "../../data/crosssection/crosssection.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QPointF>
#include <QStringList>
#include <QTextStream>

QStringList CrossSectionsCsvImporter::fileDialogFilters()
{
	QStringList ret;

	ret << tr("CSV file (*.csv)");

	return ret;
}

QStringList CrossSectionsCsvImporter::acceptableExtensions()
{
	QStringList ret;
	ret << "csv";
	return ret;
}

bool CrossSectionsCsvImporter::importData(std::vector<CrossSection*>* crossSections, QPointF* offset, const QString& filename, QWidget* w)
{
	QFile file(filename);
	if (! file.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(w, tr("Error"), tr("%1 could not be opened.").arg(QDir::toNativeSeparators(filename)));
		return false;
	}

	QTextStream stream(&file);
	bool first = true;

	int id = 0;
	do {
		QString str = stream.readLine();
		str.replace(",", " ");

		auto frags = str.split(QRegExp("\\s+"), QString::SkipEmptyParts);
		if (frags.size() < 4) {continue;}

		QTextStream strstream(&str, QIODevice::ReadOnly);
		double x1, y1, x2, y2;

		if (frags.size() == 4){
			strstream >> x1 >> y1 >> x2 >> y2;
		} else {
			continue;
		}

		if (first && offset->x() == 0 && offset->y() == 0) {
			offset->setX(x1);
			offset->setY(y1);
		}
		auto cs = new CrossSection(nullptr);
		cs->setName(QString("X%1").arg(id + 1));
		cs->setPoint1(QPointF(x1 - offset->x(), y1 - offset->y()));
		cs->setPoint2(QPointF(x2 - offset->x(), y2 - offset->y()));
		crossSections->push_back(cs);

		first = false;
		++ id;
	} while (! stream.atEnd());

	return true;
}
