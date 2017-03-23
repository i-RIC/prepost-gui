#include "polylineimporter.h"
#include "polylineimporteri.h"
#include "polylinecsvimporter.h"

#include <QFileDialog>
#include <QFileInfo>

bool PolyLineImporter::importData(std::vector<QPointF>* line, QPointF* offset, QWidget* w)
{
	std::vector<PolyLineImporterI*> importers;
	importers.push_back(new PolyLineCsvImporter());

	QStringList filters;
	for (auto i : importers) {
		filters.append(i->fileDialogFilters());
	}
	QString fname = QFileDialog::getOpenFileName(w, tr("Select file to import"), QString(), filters.join(";;"));

	if (fname.isNull()) {return false;}

	bool result = false;
	QFileInfo finfo(fname);
	for (auto i : importers) {
		QStringList exts = i->acceptableExtensions();
		if (exts.contains(finfo.suffix(), Qt::CaseInsensitive)) {
			result = i->importData(line, offset, fname, w);
			break;
		}
	}

	for (auto i : importers){
		delete i;
	}

	return result;
}

PolyLineImporter::PolyLineImporter()
{}
