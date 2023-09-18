#include "geodatapointmaprealtextimporter.h"
#include "geodatapointmap.h"
#include "private/geodatapointmaprealtextimporter_lineparser.h"
#include "private/geodatapointmaprealtextimporter_settingdialog.h"
#include "private/geodatapointmaprealtextimporter_values.h"

#include <cs/coordinatesystembuilder.h>
#include <cs/coordinatesystemconvertdialog.h>
#include <cs/coordinatesystemconverter.h>
#include <cs/gdalutil.h>
#include <guibase/widget/waitdialog.h>
#include <guicore/base/iricmainwindowi.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

#include <QApplication>
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

bool countLines(const QString& fileName, int* lines)
{
	QFile f(fileName);
	bool ok = f.open(QIODevice::ReadOnly | QIODevice::Text);
	if (! ok) {return false;}

	int l = 0;
	while (! f.atEnd()) {
		f.readLine();
		++ l;
	}
	*lines = l;

	return true;
}

} // namespace

bool operator <(const QPointF& a, const QPointF& b) {
	if (a.x() == b.x()) {
		return a.y() < b.y();
	} else {
		return a.x() < b.x();
	}
}

GeoDataPointmapRealTextImporter::GeoDataPointmapRealTextImporter(GeoDataCreator* creator) :
	GeoDataImporter("text", tr("Text files (CSV, TSV, etc.)"), creator),
	m_parser {nullptr},
	m_converter {nullptr}
{}

GeoDataPointmapRealTextImporter::~GeoDataPointmapRealTextImporter()
{
	delete m_parser;
	delete m_converter;
}

bool GeoDataPointmapRealTextImporter::importData(GeoData *data, int /*index*/, QWidget *w)
{
	bool ok;
	int lineCount;

	ok = countLines(filename(), &lineCount);

	QFile file(filename());
	if (! file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::critical(w, tr("Error"), tr("File open error occured while opening %1.").arg(QDir::toNativeSeparators(filename())));
		return false;
	}
	int lineNo = 1;
	QTextStream stream(&file);
	for (int i = 0; i < m_parser->headerLines(); ++i) {
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

	WaitDialog wDialog(w);
	wDialog.showProgressBar();
	wDialog.setRange(0, lineCount);
	wDialog.show();
	connect(&wDialog, &WaitDialog::canceled, this, &GeoDataPointmapRealTextImporter::cancel);
	m_canceled = false;

	qApp->processEvents();

	do {
		auto line = stream.readLine();
		if (dataId % m_parser->skipRate() == 0) {
			QString error;
			Values vals = m_parser->parseToValues(line, &ok, &error);
			if (! ok) {
				QMessageBox::critical(w, tr("Error"), tr("Line %1: %2").arg(lineNo).arg(error));
				return false;
			}
			QPointF p(vals.x, vals.y);

			if (m_converter != nullptr) {
				p = m_converter->convert(p);
			}

			auto it = pointSet.find(p);
			if (it == pointSet.end()) {
				points->InsertNextPoint(p.x(), p.y(), 0);
				values->InsertNextValue(vals.value);
				pointSet.insert(p);
			} else {
				++ duplicatePoints;
			}
		}
		++ dataId;
		++ lineNo;

		if (lineNo % 1000 == 0) {
			wDialog.setProgress(lineNo);
			qApp->processEvents();
		}
		if (m_canceled) {
			return false;
		}
	} while (! stream.atEnd());

	if (duplicatePoints != 0) {
		QMessageBox::warning(w, tr("Warning"), tr("%1 duplicate points found. If points with same x and y values found, the first point is imported").arg(duplicatePoints));
	}

	auto pmap = dynamic_cast<GeoDataPointmap*>(data);
	pmap->setPoints(points, values);
	pmap->rebuildTinFromPoints(false);
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

void GeoDataPointmapRealTextImporter::cancel()
{
	m_canceled = true;
}


bool GeoDataPointmapRealTextImporter::doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w)
{
	std::vector<QByteArray> lines;
	QFile file_preview(filename);
	if (! file_preview.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(w, tr("Error"), tr("File open error occured while opening %1.").arg(QDir::toNativeSeparators(filename)));
		return false;
	}
	int linesRead = 0;
	while (! file_preview.atEnd() && linesRead < PREVIEW_LINES) {
		auto l = file_preview.readLine();
		lines.push_back(l);
		++ linesRead;
	}
	file_preview.close();

	auto projectCs = item->projectData()->mainfile()->coordinateSystem();
	auto csBuilder = item->projectData()->mainWindow()->coordinateSystemBuilder();


	SettingDialog dialog(w);
	dialog.setFileName(QDir::toNativeSeparators(filename));
	dialog.setIsCsv(filename.contains(".csv"));
	dialog.setPreviewData(lines);
	dialog.setCsEnabled(projectCs != nullptr);
	dialog.setBuilder(csBuilder);

	auto prjFilename = filename;
	prjFilename.replace(QRegExp("\\.([a-z]+)$"), ".prj");
	if (QFile::exists(prjFilename)) {
			// read and get EPSG code
			QFile f(prjFilename);
			f.open(QFile::ReadOnly);
			auto wkt = f.readAll().toStdString();
			int epsgCode = GdalUtil::wkt2Epsg(wkt.c_str());
			if (epsgCode != 0) {
				auto cs = csBuilder->system(QString("EPSG:%1").arg(epsgCode));
				dialog.setCoordinateSystem(cs);
			} else {
				dialog.setCoordinateSystem(projectCs);
			}
	} else {
			dialog.setCoordinateSystem(projectCs);
	}

	if (dialog.exec() != QDialog::Accepted) {return false;}

	bool ok;
	QString error;

	m_parser = dialog.buildParser(&ok, &error);
	auto cs = dialog.coordinateSystem();
	if (projectCs != nullptr && projectCs != cs) {
		m_converter = new CoordinateSystemConverter(cs, projectCs);
	}

	return true;
}
