#include "crosssectionsimporter.h"
#include "crosssectionscsvimporter.h"

#include <QFileDialog>
#include <QFileInfo>

bool CrossSectionsImporter::importData(std::vector<CrossSection*>* sections, QPointF* offset, QWidget* w)
{
	std::vector<CrossSectionsImporterI*> importers;
	importers.push_back(new CrossSectionsCsvImporter());

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
			result = i->importData(sections, offset, fname, w);
			break;
		}
	}

	for (auto i : importers){
		delete i;
	}

	return result;
}

CrossSectionsImporter::CrossSectionsImporter()
{}
