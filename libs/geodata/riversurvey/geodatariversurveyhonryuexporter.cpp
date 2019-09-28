#include "geodatariversurvey.h"
#include "geodatariversurveyhonryuexporter.h"
#include "geodatariversurveyhonryuexportersettingdialog.h"

#include <misc/mathsupport.h>

#include <QFile>
#include <QString>
#include <QTextStream>

namespace {

QString numStr(double v)
{
	return QString::number(v, 'f', 3);
}

void writeHeader(QTextStream* stream, GeoDataRiverSurveyHonryuExporterSettingDialog::Columns columns, const QString& separator)
{
	QStringList vals;
	if (columns == GeoDataRiverSurveyHonryuExporterSettingDialog::Name_LongDistance_Distance_Elevation) {
		vals << "Name" << "LongitudinalDistance" << "Distance" << "Elevation";
	} else if (columns == GeoDataRiverSurveyHonryuExporterSettingDialog::LongDistance_Name_Distance_Elevation) {
		vals << "LongitudinalDistance" << "Name" << "Distance" << "Elevation";
	} else if (columns == GeoDataRiverSurveyHonryuExporterSettingDialog::Distance_Elevation_Name_LongDistance) {
		vals << "Distance" << "Elevation" << "Name" << "LongitudinalDistance";
	} else if (columns == GeoDataRiverSurveyHonryuExporterSettingDialog::Distance_Elevation_LongDistance_Name) {
		vals << "Distance" << "Elevation" << "LongitudinalDistance" << "Name";
	}
	*stream << vals.join(separator) << endl;
}

QStringList sortValues(const QStringList& vals, GeoDataRiverSurveyHonryuExporterSettingDialog::Columns columns)
{
	QStringList ret;
	if (columns == GeoDataRiverSurveyHonryuExporterSettingDialog::Name_LongDistance_Distance_Elevation) {
		ret << vals[0] << vals[1] << vals[2] << vals[3];
	} else if (columns == GeoDataRiverSurveyHonryuExporterSettingDialog::LongDistance_Name_Distance_Elevation) {
		ret << vals[1] << vals[0] << vals[2] << vals[3];
	} else if (columns == GeoDataRiverSurveyHonryuExporterSettingDialog::Distance_Elevation_Name_LongDistance) {
		ret << vals[2] << vals[3] << vals[0] << vals[1];
	} else if (columns == GeoDataRiverSurveyHonryuExporterSettingDialog::Distance_Elevation_LongDistance_Name) {
		ret << vals[2] << vals[3] << vals[1] << vals[0];
	}
	return ret;
}

} // namespace

GeoDataRiverSurveyHonryuExporter::GeoDataRiverSurveyHonryuExporter(GeoDataCreator* creator) :
	GeoDataExporter {tr("RiverSurvey data HONRYU export"), creator}
{}

bool GeoDataRiverSurveyHonryuExporter::doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd)
{
	GeoDataRiverSurveyHonryuExporterSettingDialog dialog(w);
	dialog.setCsvMode(selectedFilter == tr("CSV File (*.csv)"));
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return false;}

	auto separator = dialog.separator();
	bool addHeader = dialog.addHeader();
	auto ldistance = dialog.longitudinalDistance();
	auto columns = dialog.columns();

	GeoDataRiverSurvey* rsurvey = dynamic_cast<GeoDataRiverSurvey*> (data);

	QFile f(filename);
	bool ok = f.open(QFile::WriteOnly);
	if (! ok) {return false;}

	QTextStream stream(&f);

	auto p = rsurvey->headPoint()->nextPoint();
	auto prevP = p;

	QString sepStr = ",";
	if (separator == GeoDataRiverSurveyHonryuExporterSettingDialog::Space) {
		sepStr = " ";
	} else if (separator == GeoDataRiverSurveyHonryuExporterSettingDialog::Tab) {
		sepStr = "\t";
	}

	if (addHeader) {
		writeHeader(&stream, columns, sepStr);
	}

	double lenFromFirst = 0;
	while (p != nullptr) {
		double len = iRIC::distance(prevP->position(), p->position());
		lenFromFirst += len;

		double leftPos = p->crosssection().leftBank(true).position();
		const auto& alist = p->crosssection().AltitudeInfo();

		if (ldistance == GeoDataRiverSurveyHonryuExporterSettingDialog::From_First) {
			len = lenFromFirst;
		}

		for (int i = 0; i < alist.size(); ++i) {
			const auto& alt = alist.at(i);
			if (! alt.active()) {continue;}

			QStringList values;

			values << p->name();
			values << numStr(len);
			values << numStr(alt.position() - leftPos);
			values << numStr(alt.height());

			values = sortValues(values, columns);
			auto line = values.join(sepStr);
			stream << line << endl;
		}
		stream << endl;

		prevP = p;
		p = p->nextPoint();
	}

	f.close();
	return true;
}

const QStringList GeoDataRiverSurveyHonryuExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("Text File (*.txt)");
	ret << tr("CSV File (*.csv)");
	return ret;
}
