#include "geodatapointgroup.h"
#include "geodatapointgroupcsvimporter.h"
#include "geodatapointgrouppoint.h"

#include <cs/coordinatesystembuilder.h>
#include <cs/coordinatesystemconvertdialog.h>
#include <cs/coordinatesystemconverter.h>
#include <cs/gdalutil.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

GeoDataPointGroupCsvImporter::GeoDataPointGroupCsvImporter(GeoDataCreator* creator) :
	GeoDataImporter {"csv_point_group", tr("CSV file (Points)"), creator},
	m_converter {nullptr}
{}

GeoDataPointGroupCsvImporter::~GeoDataPointGroupCsvImporter()
{
	delete m_converter;
}

const QStringList GeoDataPointGroupCsvImporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("CSV file (Points) (*.csv)"));
	return ret;
}

const QStringList GeoDataPointGroupCsvImporter::acceptableExtensions()
{
	QStringList ret;
	ret.append("csv");
	return ret;
}

bool GeoDataPointGroupCsvImporter::importData(GeoData* data, int /*index*/, QWidget* w)
{
	auto group = dynamic_cast<GeoDataPointGroup*>(data);

	QFile file(filename());
	if (! file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::critical(w, tr("Error"), tr("Error occured while opening %1").arg(QDir::toNativeSeparators(filename())));
		return false;
	}
	QTextStream stream(&file);
	stream.setCodec("UTF-8");
	bool isRef = data->gridAttribute()->isReferenceInformation();

	// check header
	QString buffer;
	bool ok = stream.readLineInto(&buffer);
	if (! ok) {
		QMessageBox::critical(w, tr("Error"), tr("Error occured while reading header line. Make sure the file is saved in UTF-8 encoding."));
		return false;
	}
	auto frags = buffer.split(",");
	QStringList headers;
	headers << "x" << "y" << "name";
	if (isRef) {
		if (frags.size() < 3) {
			QMessageBox::critical(w, tr("Error"), tr("Error occured while reading header line. Header should have 3 columns at least."));
			return false;
		}
	} else {
		if (frags.size() < 4) {
			QMessageBox::critical(w, tr("Error"), tr("Error occured while reading header line. Header should have 4 columns at least."));
			return false;
		}
		headers << "value";
	}
	for (int i = 0; i < headers.size(); ++i) {
		QString f = frags.at(i).trimmed();
		if (f != headers.at(i)) {
			QMessageBox::critical(w, tr("Error"), tr("Error occured while reading header line. Column %1 should be \"%2\", but \"%3\" is given.").arg(i + 1).arg(headers.at(i)).arg(f));
			return false;
		}
	}
	int lineno = 2;
	while (true) {
		bool ok = stream.readLineInto(&buffer);
		if (! ok) {
			if (stream.atEnd()) {
				// successfully read all data
				group->setupDataItem();
				group->updateAttributeBrowser(true);
				group->updateVtkObjects();
				group->updateIndex();
				return true;
			}
			// error occured
			QMessageBox::critical(w, tr("Error"), tr("Error occured while reading line %1. Make sure the file is saved in UTF-8 encoding.").arg(lineno));
			return false;
		}
		auto frags = buffer.split(",");
		if (isRef) {
			if (frags.size() < 3) {
				QMessageBox::critical(w, tr("Error"), tr("Error occured while reading line %1. Data should have 3 columns at least.").arg(lineno));
				return false;
			}
		} else {
			if (frags.size() < 4) {
				QMessageBox::critical(w, tr("Error"), tr("Error occured while reading line %1. Data should have 4 columns at least.").arg(lineno));
				return false;
			}
			headers << "value";
		}
		double x, y, value = 0;
		QString name;
		x = frags.at(0).toDouble(&ok);
		if (! ok) {
			QMessageBox::critical(w, tr("Error"), tr("Error occured while reading \"x\" value in line %1. \"x\" should be real value but \"%2\" is specified.").arg(lineno).arg(frags.at(0)));
			return false;
		}
		y = frags.at(1).toDouble(&ok);
		if (! ok) {
			QMessageBox::critical(w, tr("Error"), tr("Error occured while reading \"y\" value in line %1. \"y\" should be real value but \"%2\" is specified.").arg(lineno).arg(frags.at(1)));
			return false;
		}
		name = frags.at(2);
		if (! isRef) {
			value = frags.at(3).toDouble(&ok);
			if (! ok) {
				QMessageBox::critical(w, tr("Error"), tr("Error occured while reading \"value\" value in line %1. \"value\" should be real value but \"%2\" is specified.").arg(lineno).arg(frags.at(3)));
				return false;
			}
		}
		QPointF p(x, y);

		if (m_converter != nullptr) {
			p = m_converter->convert(p);
		}

		auto point = new GeoDataPointGroupPoint(p, group);
		point->setName(name);
		point->setValue(value);

		group->addData(point);

		++ lineno;
	}
}

bool GeoDataPointGroupCsvImporter::doInit(const QString& filename, const QString& /*selectedFilter*/, int* /*count*/, SolverDefinitionGridAttribute* /*condition*/, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w)
{
	auto projectCs = item->projectData()->mainfile()->coordinateSystem();
	if (projectCs == nullptr) {return true;}

	auto csBuilder = item->projectData()->mainWindow()->coordinateSystemBuilder();
	CoordinateSystemConvertDialog dialog(w);
	dialog.setBuilder(csBuilder);
	dialog.setEnabled(true);

	auto prjFilename = filename;
	prjFilename.replace(QRegExp("\\.csv$"), ".prj");
	if (QFile::exists(prjFilename)) {
		// read and get EPSG code
		QFile f(prjFilename);
		f.open(QFile::ReadOnly);
		auto wkt = f.readAll().toStdString();
		int epsgCode = GdalUtil::wkt2Epsg(wkt.c_str());
		auto cs = csBuilder->system(QString("EPSG:%1").arg(epsgCode));
		dialog.setCoordinateSystem(cs);
	} else {
		dialog.setCoordinateSystem(projectCs);
	}

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {
		return false;
	}
	auto cs = dialog.coordinateSystem();

	if (projectCs != cs) {
		m_converter = new CoordinateSystemConverter(cs, projectCs);
	}
	return true;
}
