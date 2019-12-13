#include "geodatarivercrosssection.h"
#include "geodatariverpathpoint.h"
#include "geodatariversurvey.h"
#include "geodatariversurveymlitimporter.h"
#include "private/geodatariversurveymlitimporter_problemsdialog.h"

#include <misc/stringtool.h>

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include <vector>

namespace {

bool loadDistanceMarkerData(const QString& filename, std::vector<GeoDataRiverSurveyImporter::RivPathPoint*>* points, QWidget* w)
{
	QFile f(filename);
	QFileInfo finfo(f);
	auto fName = finfo.fileName();
	bool ok = f.open(QIODevice::ReadOnly | QIODevice::Text);
	if (! ok) {
		QMessageBox::critical(w, GeoDataRiverSurveyMlitImporter::tr("Error"), GeoDataRiverSurveyMlitImporter::tr("Opening %1 failed.").arg(QDir::toNativeSeparators(filename)));
		return false;
	}
	// skip header
	f.readLine();
	int linenum = 2;

	while (! f.atEnd()) {
		QString line = f.readLine();

		auto tokens = line.split(",", QString::KeepEmptyParts);
		QString name = tokens.at(0);
		QString lx = tokens.at(1);
		QString ly = tokens.at(2);
		QString rx = tokens.at(3);
		QString ry = tokens.at(4);
		if (name.isEmpty()) {
			QMessageBox::critical(w, GeoDataRiverSurveyMlitImporter::tr("Error"), GeoDataRiverSurveyMlitImporter::tr("%1 Line %2: KP is not input").arg(fName).arg(linenum));
			return false;
		}
		double lxval = lx.toDouble(&ok);
		if (! ok) {
			QMessageBox::critical(w, GeoDataRiverSurveyMlitImporter::tr("Error"), GeoDataRiverSurveyMlitImporter::tr("%1 Line %2 \"%3\": LX value is invalid").arg(fName).arg(linenum).arg(lx));
			return false;
		}
		double lyval = ly.toDouble(&ok);
		if (! ok) {
			QMessageBox::critical(w, GeoDataRiverSurveyMlitImporter::tr("Error"), GeoDataRiverSurveyMlitImporter::tr("%1 Line %2 \"%3\": LY value is invalid").arg(fName).arg(linenum).arg(ly));
			return false;
		}
		double rxval = rx.toDouble(&ok);
		if (! ok) {
			QMessageBox::critical(w, GeoDataRiverSurveyMlitImporter::tr("Error"), GeoDataRiverSurveyMlitImporter::tr("%1 Line %2 \"%3\": RX value is invalid").arg(fName).arg(linenum).arg(rx));
			return false;
		}
		double ryval = ry.toDouble(&ok);
		if (! ok) {
			QMessageBox::critical(w, GeoDataRiverSurveyMlitImporter::tr("Error"), GeoDataRiverSurveyMlitImporter::tr("%1 Line %2 \"%3\": RY value is invalid").arg(fName).arg(linenum).arg(ry));
			return false;
		}
		auto p = new GeoDataRiverSurveyImporter::RivPathPoint();
		p->banksIsSet = true;
		p->strKP = iRIC::toStr(name);
		p->realKP = name.toDouble();
		p->leftBank.setX(lyval);
		p->leftBank.setY(lxval);
		p->rightBank.setX(ryval);
		p->rightBank.setY(rxval);
		points->push_back(p);

		++ linenum;
	}
	f.close();
	return true;
}

bool loadCrossSectionData(const QString& filename, std::vector<GeoDataRiverSurveyImporter::RivPathPoint*>* points, const QString& dmFile, bool* with4Points, std::vector<GeoDataRiverSurveyMlitImporter::ProblemsDialog::Problem>* problems, QWidget* w)
{
	*with4Points = false;

	QFile f(filename);
	QFileInfo finfo(f);
	auto fName = finfo.fileName();
	bool ok = f.open(QIODevice::ReadOnly | QIODevice::Text);
	if (! ok) {
		QMessageBox::critical(w, GeoDataRiverSurveyMlitImporter::tr("Error"), GeoDataRiverSurveyMlitImporter::tr("Opening %1 failed.").arg(QDir::toNativeSeparators(filename)));
		return false;
	}
	QString line = f.readLine();
	auto tokens = line.split(",", QString::KeepEmptyParts);
	double realKP = tokens.at(0).toDouble();
	GeoDataRiverSurveyImporter::RivPathPoint* point = nullptr;

	for (GeoDataRiverSurveyImporter::RivPathPoint* p : *points) {
		if (p->realKP == realKP) {
			point = p;
			break;
		}
	}
	if (point == nullptr) {
		GeoDataRiverSurveyMlitImporter::ProblemsDialog::Problem problem;
		problem.name = tokens.at(0);
		problem.relatedFile = fName;
		problem.problem = GeoDataRiverSurveyMlitImporter::tr("Distance marker data is not found. Will be ignored.").arg(dmFile);
		problems->push_back(problem);
		return true;
	}
	point->crossSectionFileName = fName;
	int linenum = 2;

	std::vector<int> indices;
	while (! f.atEnd()) {
		QString line = f.readLine();

		auto tokens = line.split(",", QString::KeepEmptyParts);
		if (tokens.size() < 3) {break;}

		int flag = tokens.at(0).toInt(&ok);
		if (! ok) {
			QMessageBox::critical(w, GeoDataRiverSurveyMlitImporter::tr("Error"), GeoDataRiverSurveyMlitImporter::tr("%1 line %2: \"%3\": Flag data is invalid.").arg(fName).arg(linenum).arg(tokens.at(0)));
			return false;
		}
		double dist = tokens.at(1).toDouble(&ok);
		if (! ok) {
			QMessageBox::critical(w, GeoDataRiverSurveyMlitImporter::tr("Error"), GeoDataRiverSurveyMlitImporter::tr("%1 line %2: \"%3\": Distance data is invalid.").arg(fName).arg(linenum).arg(tokens.at(1)));
			return false;
		}
		double elev = tokens.at(2).toDouble(&ok);
		if (! ok) {
			QMessageBox::critical(w, GeoDataRiverSurveyMlitImporter::tr("Error"), GeoDataRiverSurveyMlitImporter::tr("%1 line %2: \"%3\": Elevation data is invalid.").arg(fName).arg(linenum).arg(tokens.at(2)));
			return false;
		}
		GeoDataRiverSurveyImporter::Alt alt(dist, elev);
		point->altitudes.push_back(alt);
		if (flag == 13) {
			indices.push_back(point->altitudes.size());
		}
		++ linenum;
	}
	if (indices.size() == 2) {
		point->divIndices[0] = 1;
		point->divIndices[3] = point->altitudes.size();
		point->divIndices[1] = indices[0];
		point->divIndices[2] = indices[1];
		*with4Points = true;
	}
	f.close();

	GeoDataRiverSurveyImporter::sortAlts(&(point->altitudes), &(point->sorted));
	GeoDataRiverSurveyImporter::uniqueAlts(&(point->altitudes), &(point->uniquedDistances));

	return true;
}

bool readMlitRivFile(const QString& filename, std::vector<GeoDataRiverSurveyImporter::RivPathPoint*>* points, bool *with4Points, QWidget* w)
{
	GeoDataRiverSurveyImporter::clearPoints(points);
	bool ok = loadDistanceMarkerData(filename, points, w);
	if (! ok) {return false;}

	QFileInfo finfo(filename);

	QFileDialog dialog(w, GeoDataRiverSurveyMlitImporter::tr("Select folder where cross section data exists"), finfo.absolutePath());
	dialog.setFileMode(QFileDialog::Directory);
	dialog.setOption(QFileDialog::ShowDirsOnly);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return false;}
	auto filePaths = dialog.selectedFiles();

	QDir xsDir(filePaths.at(0));
	QStringList filters;
	filters << "*.csv";
	auto names = xsDir.entryList(filters, QDir::Files, QDir::Name);
	*with4Points = true;
	std::vector<GeoDataRiverSurveyMlitImporter::ProblemsDialog::Problem> problems;
	for (auto name : names) {
		bool tmp4Points;
		ok = loadCrossSectionData(xsDir.absoluteFilePath(name), points, finfo.fileName(), &tmp4Points, &problems, w);
		if (! ok) {return false;}

		*with4Points = *with4Points && tmp4Points;
	}
	for (GeoDataRiverSurveyImporter::RivPathPoint* p : *points) {
		if (p->altitudes.size() == 0) {
			GeoDataRiverSurveyMlitImporter::ProblemsDialog::Problem problem;
			problem.name = p->strKP.c_str();
			problem.relatedFile = finfo.fileName();
			problem.problem = GeoDataRiverSurveyMlitImporter::tr("Cross Section data is not found. Will be ignored.");
			problems.push_back(problem);
		} else {
			if (p->uniquedDistances.size() > 0) {
				auto distances = GeoDataRiverSurveyImporter::distListString(p->uniquedDistances);
				GeoDataRiverSurveyMlitImporter::ProblemsDialog::Problem problem;
				problem.name = p->strKP.c_str();
				problem.relatedFile = p->crossSectionFileName;
				problem.problem = GeoDataRiverSurveyImporter::tr("Cross Section data contained data with same distances. Data with lowest elevation is used.: %1").arg(distances);
				problems.push_back(problem);
			} else if (p->sorted) {
				GeoDataRiverSurveyMlitImporter::ProblemsDialog::Problem problem;
				problem.name = p->strKP.c_str();
				problem.relatedFile = p->crossSectionFileName;
				problem.problem = GeoDataRiverSurveyMlitImporter::tr("Cross Section data is not correctly ordered. Will be sorted automatically.");
				problems.push_back(problem);
			}
		}
	}
	if (problems.size() > 0) {
		GeoDataRiverSurveyMlitImporter::ProblemsDialog dialog(w);
		dialog.setProblems(problems);
		int result = dialog.exec();
		if (result == QDialog::Rejected) {return false;}
	}
	GeoDataRiverSurveyImporter::removePointsWithoutAltitudes(points);

	return true;
}

} // namespace

GeoDataRiverSurveyMlitImporter::GeoDataRiverSurveyMlitImporter(GeoDataCreator* creator) :
	GeoDataImporter("mlit_csv", tr("MLIT River Survey Data(*.csv)"), creator)
{}

bool GeoDataRiverSurveyMlitImporter::importData(GeoData* data, int /* index */, QWidget* w)
{
	auto rs = dynamic_cast<GeoDataRiverSurvey*> (data);

	return GeoDataRiverSurveyImporter::importData(rs, &m_points, m_cpSetting, m_with4Points, w);
}

const QStringList GeoDataRiverSurveyMlitImporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("Japan MLIT river survey data (*.csv)");
	return ret;
}

const QStringList GeoDataRiverSurveyMlitImporter::acceptableExtensions()
{
	QStringList ret;
	ret << "csv";
	return ret;
}

bool GeoDataRiverSurveyMlitImporter::doInit(const QString& filename, const QString& /*selectedFilter*/, int* count, SolverDefinitionGridAttribute* /*condition*/, PreProcessorGeoDataGroupDataItemInterface* /*item*/, QWidget* w)
{
	if (! readMlitRivFile(filename, &m_points, &m_with4Points, w)) {return false;}

	GeoDataRiverSurveyImporterSettingDialog dialog(w);
	dialog.setWith4Points(m_with4Points);
	dialog.setAllNamesAreNumber(true);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {
		GeoDataRiverSurveyImporter::clearPoints(&m_points);
		return false;
	}

	m_cpSetting = dialog.centerPointSetting();
	GeoDataRiverSurveyImporter::sortByKP(&m_points);
	*count = 1;
	return true;
}
