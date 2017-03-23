#include "polylineexporter.h"
#include "polylineexporteri.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QString>

bool PolyLineExporter::exportData(const std::vector<QPointF>& line, const QPointF& offset, QWidget* w)
{
	std::vector<PolyLineExporterI*> exporters;
	// exporters.push_back(new PolyLineCsvImporter());

	QStringList filters;
	for (auto e : exporters) {
		filters.append(e->fileDialogFilters());
	}
	QString fname = QFileDialog::getSaveFileName(w, tr("Input file name to export"), QString(), filters.join(";;"));

	if (fname.isNull()) {return false;}

	bool result = false;
	QFileInfo finfo(fname);
	for (auto e : exporters) {
		QStringList exts = e->acceptableExtensions();
		if (exts.contains(finfo.suffix(), Qt::CaseInsensitive)) {
			result = e->exportData(line, offset, fname, w);
			break;
		}
	}

	for (auto e : exporters){
		delete e;
	}

	return result;
}

PolyLineExporter::PolyLineExporter()
{}
