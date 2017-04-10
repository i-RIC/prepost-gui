#include "crosssectionsexporter.h"
#include "crosssectionsexporteri.h"
#include "crosssectionscsvexporter.h"

#include <QFileDialog>
#include <QFileInfo>

bool CrossSectionsExporter::exportData(const std::vector<CrossSection*> crossSections, const QPointF& offset, QWidget* w)
{
	std::vector<CrossSectionsExporterI*> exporters;
	exporters.push_back(new CrossSectionsCsvExporter());

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
			result = e->exportData(crossSections, offset, fname, w);
			break;
		}
	}

	for (auto e : exporters){
		delete e;
	}

	return result;
}

CrossSectionsExporter::CrossSectionsExporter()
{}
