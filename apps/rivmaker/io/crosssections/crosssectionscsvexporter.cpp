#include "crosssectionscsvexporter.h"
#include "../../data/crosssection/crosssection.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QPointF>
#include <QStringList>
#include <QTextStream>

QStringList CrossSectionsCsvExporter::fileDialogFilters()
{
	QStringList ret;

	ret << tr("CSV file (*.csv)");

	return ret;
}

QStringList CrossSectionsCsvExporter::acceptableExtensions()
{
	QStringList ret;
	ret << "csv";
	return ret;
}

bool CrossSectionsCsvExporter::exportData(const std::vector<CrossSection*>& crossSections, const QPointF& offset, const QString& filename, QWidget* w)
{
	QFile file(filename);
	if (! file.open(QIODevice::WriteOnly)) {
		QMessageBox::critical(w, tr("Error"), tr("%1 could not be opened.").arg(QDir::toNativeSeparators(filename)));
		return false;
	}

	QTextStream stream(&file);

	for (CrossSection* cs : crossSections) {
		stream << cs->point1().x() + offset.x() << ",";
		stream << cs->point1().y() + offset.y() << ",";
		stream << cs->point2().x() + offset.x() << ",";
		stream << cs->point2().y() + offset.y() << endl;
	}

	return true;
}
