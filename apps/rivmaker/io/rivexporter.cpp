#include "rivexporter.h"
#include "../data/crosssections/crosssections.h"

#include <QFileDialog>

bool RivExporter::exportData(const CrossSections& crossSections, QWidget* w)
{
	QString fname = QFileDialog::getSaveFileName(w, tr("Input File name to export"), "", tr("River Survey Data(*.riv)"));
	if (fname.isNull()) {return false;}

	return crossSections.exportRiverSurveyData(fname);
}

RivExporter::RivExporter()
{}
