#include "pointsimporter.h"
#include "pointsimporteri.h"
#include "pointscsvimporter.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QString>

bool PointsImporter::importData(std::vector<GeometryPoint*>* points, QPointF* offset, QWidget* w)
{
	std::vector<PointsImporterI*> importers;
	importers.push_back(new PointsCsvImporter());

	QStringList filters;
	QStringList availableExtensions;
	for (auto i : importers) {
		filters.append(i->fileDialogFilters());
		for (QString ext : i->acceptableExtensions()) {
			availableExtensions << QString("*.").append(ext);
		}
	}
	filters.push_front(tr("All importable files (%1)").arg(availableExtensions.join(" ")));
	QString fname = QFileDialog::getOpenFileName(w, tr("Select file to import"), QString(), filters.join(";;"));

	if (fname.isNull()) {return false;}

	bool result = false;
	QFileInfo finfo(fname);
	for (auto i : importers) {
		QStringList exts = i->acceptableExtensions();
		if (exts.contains(finfo.suffix(), Qt::CaseInsensitive)) {
			result = i->importData(points, offset, fname, w);
			break;
		}
	}

	for (auto i : importers){
		delete i;
	}

	return result;
}

PointsImporter::PointsImporter()
{}
