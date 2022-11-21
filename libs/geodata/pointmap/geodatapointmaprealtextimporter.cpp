#include "geodatapointmaprealtextimporter.h"
#include "geodatapointmapt.h"
#include "private/geodatapointmaprealtextimporter_lineparser.h"
#include "private/geodatapointmaprealtextimporter_settingdialog.h"
#include "private/geodatapointmaprealtextimporter_values.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QPointF>
#include <QTextStream>

#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>

#include <set>

namespace {

int PREVIEW_LINES = 100;

} // namespace

bool operator <(const QPointF& a, const QPointF& b) {
	if (a.x() == b.x()) {
		return a.y() < b.y();
	} else {
		return a.x() < b.x();
	}
}

GeoDataPointmapRealTextImporter::GeoDataPointmapRealTextImporter(GeoDataCreator* creator) :
	GeoDataImporter("text", tr("Text files (CSV, TSV, etc.)"), creator)
{}

bool GeoDataPointmapRealTextImporter::importData(GeoData *data, int /*index*/, QWidget *w)
{
	std::vector<QByteArray> lines;
	QFile file_preview(filename());
	if (! file_preview.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(w, tr("Error"), tr("File open error occured while opening %1.").arg(QDir::toNativeSeparators(filename())));
		return false;
	}
	int linesRead = 0;
	while (! file_preview.atEnd() && linesRead < PREVIEW_LINES) {
		auto l = file_preview.readLine();
		lines.push_back(l);
		++ linesRead;
	}
	file_preview.close();

	SettingDialog dialog(w);
	dialog.setFileName(QDir::toNativeSeparators(filename()));
	dialog.setIsCsv(filename().contains(".csv"));
	dialog.setPreviewData(lines);

	if (dialog.exec() != QDialog::Accepted) {return false;}

	bool ok;
	QString error;
	LineParser parser = dialog.buildParser(&ok, &error);

	QFile file(filename());
	if (! file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::critical(w, tr("Error"), tr("File open error occured while opening %1.").arg(QDir::toNativeSeparators(filename())));
		return false;
	}
	int lineNo = 1;
	QTextStream stream(&file);
	for (int i = 0; i < parser.headerLines(); ++i) {
		// skip header lines
		stream.readLine();
		if (stream.atEnd()) {
			QMessageBox::critical(w, tr("Error"), tr("Not enough header lines found in the file.").arg(QDir::toNativeSeparators(filename())));
			return false;
		}
		++ lineNo;
	}

	auto points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	auto values = vtkSmartPointer<vtkDoubleArray>::New();

	std::set<QPointF> pointSet;
	int duplicatePoints = 0;
	int dataId = 0;
	do {
		auto line = stream.readLine();
		if (dataId % parser.skipRate() == 0) {
			Values vals = parser.parseToValues(line, &ok, &error);
			if (! ok) {
				QMessageBox::critical(w, tr("Error"), tr("Line %1: %2").arg(lineNo).arg(error));
				return false;
			}
			QPointF p(vals.x, vals.y);
			auto it = pointSet.find(p);
			if (it == pointSet.end()) {
				points->InsertNextPoint(vals.x, vals.y, 0);
				values->InsertNextValue(vals.value);
				pointSet.insert(p);
			} else {
				++ duplicatePoints;
			}
		}
		++ dataId;
		++ lineNo;
	} while (! stream.atEnd());

	if (duplicatePoints != 0) {
		QMessageBox::warning(w, tr("Warning"), tr("%1 duplicate points found. If points with same x and y values found, the first point is imported").arg(duplicatePoints));
	}

	auto pmap = dynamic_cast<GeoDataPointMapT<double, vtkDoubleArray>*>(data);
	pmap->setPoints(points, values);
	pmap->doDelaunay();
	return true;
}

const QStringList GeoDataPointmapRealTextImporter::fileDialogFilters()
{
	QStringList	ret;
	ret << tr("Text file (*.tpo *.csv *.txt *.xyz)");
	return ret;
}

const QStringList GeoDataPointmapRealTextImporter::acceptableExtensions()
{
	QStringList ret;
	ret << "tpo" << "csv" << "txt" << "xyz";
	return ret;
}
