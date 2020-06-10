#include "rivcsvexporter.h"
#include "../data/crosssection/crosssection.h"
#include "../data/crosssections/crosssections.h"
#include "../data/project/project.h"

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QPointF>
#include <QTextStream>
#include <QVector2D>

RivCsvExporter::RivCsvExporter()
{}

bool RivCsvExporter::exportData(Project* project, QWidget* w)
{
	QString folderName = QFileDialog::getExistingDirectory(w, tr("Select directory to save CSV files"), project->rivCsvFolderName());
	if (folderName.isNull()) {return false;}

	project->setRivCsvFolderName(folderName);
	QDir dir(folderName);

	const auto& cs = project->crossSections();
	for (CrossSection* s : cs.crossSectionVector()) {
		auto fname = dir.absoluteFilePath(QString("%1.csv").arg(s->name()));
		QFile f(fname);
		if (! f.open(QFile::WriteOnly)) {
			QMessageBox::critical(w, tr("Error"), tr("%1 could not be opened.").arg(QDir::toNativeSeparators(fname)));
			return false;
		}
		QTextStream ts(&f);
		ts.setRealNumberPrecision(6);
		auto points = s->mappedPoints();
		for (const auto& p : points) {
			ts << p.x() << "," << p.y() << endl;
		}
		f.close();
	}
}
