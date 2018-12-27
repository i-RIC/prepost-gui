#include "geodatarivercrosssection.h"
#include "geodatariverpathpoint.h"
#include "geodatariversurvey.h"
#include "geodatariversurveyimporter.h"
#include "geodatariversurveyimportersettingdialog.h"

#include <misc/errormessage.h>
#include <misc/stringtool.h>

#include <QFile>
#include <QMessageBox>
#include <QMultiMap>
#include <QTextStream>
#include <QVector2D>

#include <algorithm>
#include <cmath>
#include <vector>

namespace {

const double WRONG_KP = -9999;

class Point2D
{
public:
	Point2D() {x = 0; y = 0;}
	Point2D(double x, double y) {
		this->x = x; this->y = y;
	}

	double x = 0;
	double y = 0;
};

class Alt
{
public:
	Alt() :
		Alt(0, 0)
	{}
	Alt(double dis, double e) {
		this->distance = dis; this->elevation = e;
	}

	double distance;
	double elevation;
};

class RivPathPoint
{
public:
	Point2D banksCenter() const
	{
		return Point2D((leftBank.x + rightBank.x) * 0.5, (leftBank.y + rightBank.y) * 0.5);
	}
	double banksDistance() const
	{
		double dx = rightBank.x - leftBank.x;
		double dy = rightBank.y - leftBank.y;

		return std::sqrt(dx * dx + dy * dy);
	}
	QVector2D leftToRight() const
	{
		return QVector2D(rightBank.x - leftBank.x, rightBank.y - leftBank.y);
	}

	std::string strKP; // name in string
	double realKP;     // name in real

	bool banksIsSet = false;
	Point2D leftBank;
	Point2D rightBank;

	std::vector<Alt> altitudes;
	int divIndices[4];
};

RivPathPoint* getPoint(double kp, std::string& kpStr, std::vector<RivPathPoint*>* points)
{
	for (RivPathPoint* p : *points) {
		if (kp == WRONG_KP) {
			if (p->strKP == kpStr) {
				return p;
			}
		} else {
			if (p->realKP == kp) {
				return p;
			}
		}
	}
	RivPathPoint* newp = new RivPathPoint();
	newp->realKP = kp;
	newp->strKP = kpStr;
	points->push_back(newp);
	return newp;
}

void setBanks(RivPathPoint* p, double lx, double ly, double rx, double ry)
{
	p->banksIsSet = true;
	p->leftBank.x = lx;
	p->leftBank.y = ly;
	p->rightBank.x = rx;
	p->rightBank.y = ry;
}

void removeIncompleteData(std::vector<RivPathPoint*>* points)
{
	int idx = 0;
	while (idx < points->size()) {
		auto it = points->begin() + idx;
		RivPathPoint* p = *it;
		if (! p->banksIsSet || p->altitudes.size() == 0) {
			// remove this point;
			points->erase(it);
		} else {
			++ idx;
		}
	}
}

void sortReverse(std::vector<RivPathPoint*>* points)
{
	std::reverse(points->begin(), points->end());
}

bool lessKP(RivPathPoint* a1, RivPathPoint* a2)
{
	return a1->realKP > a2->realKP;
}

void sortByKP(std::vector<RivPathPoint*>* points)
{
	std::sort(points->begin(), points->end(), lessKP);
}

void parseKP(const QString& tok, double* realKP, std::string* strKP, bool* allNumber)
{
	bool ok;

	*realKP = tok.toDouble(&ok);
	*strKP = iRIC::toStr(tok);

	if (! ok){
		*allNumber = false;
		*realKP = WRONG_KP;
	}
}

bool readRivFile(const QString& fname, std::vector<RivPathPoint*>* points, bool* with4points, bool* allNamesAreNumber)
{
	QFile f(fname);

	*allNamesAreNumber = true;
	int mode;
	*with4points = true;

	// open file
	f.open(QIODevice::ReadOnly | QIODevice::Text);
	while (! f.atEnd()) {
		QString line = f.readLine();
		QRegExp sep("\\s+");
		QStringList tokens = line.split(sep, QString::SkipEmptyParts);

		if (tokens.length() >= 1) {
			if (tokens.at(0) == "#survey") {
				mode = 1;
				continue;
			} else if (tokens.at(0) == "#x-section") {
				mode = 2;
				continue;
			}
			std::string strKP;
			double realKP;
			RivPathPoint* p;
			int np;
			std::vector<Alt> alts;
			int divIndices[4];

			if (mode == 1) {
				parseKP(tokens.at(0), &realKP, &strKP, allNamesAreNumber);

				double leftx = tokens.at(1).toDouble();
				double lefty = tokens.at(2).toDouble();
				double rightx = tokens.at(3).toDouble();
				double righty = tokens.at(4).toDouble();

				p = getPoint(realKP, strKP, points);
				setBanks(p, leftx, lefty, rightx, righty);
			} else if (mode == 2) {
				parseKP(tokens.at(0), &realKP, &strKP, allNamesAreNumber);

				if (tokens.length() < 2) {break;}
				np = static_cast<int>(tokens.at(1).toDouble());
				// Tries to read four indices values.
				if (tokens.length() >= 6) {
					for (int k = 0; k < 4; ++k) {
						divIndices[k] = tokens.at(k + 2).toInt();
					}
				} else {
					*with4points = false;
				}
				p = getPoint(realKP, strKP, points);
				int i = 0;
				while (i < np) {
					if (f.atEnd()) {
						break;
					}
					line = f.readLine();
					tokens = line.split(sep, QString::SkipEmptyParts);

					int k = 0;
					while (k < tokens.length() - 1) {
						Alt alt;
						alt.distance = tokens.at(k++).toDouble();
						alt.elevation = tokens.at(k++).toDouble();
						alts.push_back(alt);
						++ i;
					}
				}
				if (*with4points) {
					for (int i = 0; i < 4; ++i) {
						p->divIndices[i] = divIndices[i];
					}
				}
				p->altitudes = alts;
			}
		}
	}
	f.close();
	int idx = 0;
	while (idx < points->size()) {
		if (points->at(idx)->altitudes.size() == 0) {
			points->erase(points->begin() + idx);
			continue;
		}
		++ idx;
	}

	return (mode == 2);
}

} // namespace

GeoDataRiverSurveyImporter::GeoDataRiverSurveyImporter(GeoDataCreator* creator) :
	GeoDataImporter("riversurvey", tr("River Survey data (*.riv)"), creator)
{}

bool GeoDataRiverSurveyImporter::doInit(const QString& filename, const QString& /*selectedFilter*/, int* count, SolverDefinitionGridAttribute* /*condition*/, PreProcessorGeoDataGroupDataItemInterface* /*item*/, QWidget* w)
{
	if (! readRivFile(filename, &m_points, &m_with4Points, &m_allNamesAreNumber)) {return false;}

	GeoDataRiverSurveyImporterSettingDialog dialog(w);
	dialog.setWith4Points(m_with4Points);
	dialog.setAllNamesAreNumber(m_allNamesAreNumber);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {
		clearPoints();
		return false;
	}

	m_cpSetting = dialog.centerPointSetting();
	if (m_allNamesAreNumber) {
		sortByKP(&m_points);
	} else if (dialog.reverseOrder()) {
		sortReverse(&m_points);
	}
	*count = 1;
	return true;
}

bool GeoDataRiverSurveyImporter::importData(GeoData* data, int /*index*/, QWidget* w)
{
	GeoDataRiverPathPoint *tail;

	GeoDataRiverSurvey* rs = dynamic_cast<GeoDataRiverSurvey*>(data);
	tail = rs->m_headPoint;

	double left = 0, right = 0;
	double minpos = 0, minval = 0;
	bool ok = true;

	for (int i = 0; i < m_points.size(); ++i) {
		RivPathPoint* p = m_points[i];

		auto leftp = p->leftBank;
		auto newPoint = new GeoDataRiverPathPoint(leftp.x, leftp.y);

		newPoint->setName(p->strKP.c_str());
		newPoint->InhibitInterpolatorUpdate = true;
		newPoint->setCrosssectionDirection(p->leftToRight().normalized());

		if (m_with4Points) {
			GeoDataRiverCrosssection::Altitude prevAlt(0, 0);
			for (int j = 0; j < p->altitudes.size(); ++j) {
				const auto& a = p->altitudes.at(j);
				GeoDataRiverCrosssection::Altitude alt(a.distance, a.elevation);
				if (j + 1 < p->divIndices[0] || j + 1 > p->divIndices[3]) {continue;}

				newPoint->crosssection().AltitudeInfo().push_back(alt);
				if (j >= p->divIndices[0] && prevAlt.position() > alt.position() && ok) {
					QMessageBox::warning(w, tr("Warning"), tr("Crosssection data is not ordered correctly at %1.").arg(newPoint->name()));
					ok = false;
				}
				prevAlt = alt;
			}
			for (int j = p->divIndices[1] - 1; j <= p->divIndices[2] - 1; ++j) {
				const auto& a = p->altitudes.at(j);
				if (j == p->divIndices[1] - 1 || a.elevation < minval) {
					minpos = a.distance;
					minval = a.elevation;
				}
			}
			left  = p->altitudes.at(p->divIndices[1] - 1).distance;
			right = p->altitudes.at(p->divIndices[2] - 1).distance;
			double shiftValue = 0;
			if (m_cpSetting == GeoDataRiverSurveyImporterSettingDialog::cpMiddle) {
				shiftValue = (left + right) * 0.5;
			} else if (m_cpSetting == GeoDataRiverSurveyImporterSettingDialog::cpElevation) {
				shiftValue = minpos;
			}
			double leftPoint = (left - shiftValue) /
				(newPoint->crosssection().leftBank().position() - shiftValue);
			double rightPoint = (right - shiftValue) /
				(newPoint->crosssection().rightBank().position() - shiftValue);
			newPoint->shiftCenter(shiftValue);
			newPoint->CenterToLeftCtrlPoints.push_back(leftPoint);
			newPoint->CenterToRightCtrlPoints.push_back(rightPoint);
		} else {
			GeoDataRiverCrosssection::Altitude prevAlt(0, 0);
			for (int j = 0; j < p->altitudes.size(); ++j) {
				const auto& a = p->altitudes.at(j);
				if (j == 0 || a.elevation < minval) {
					minpos = a.distance;
					minval = a.elevation;
				}
				GeoDataRiverCrosssection::Altitude alt(a.distance, a.elevation);
				newPoint->crosssection().AltitudeInfo().push_back(alt);
				if (j > 0 && prevAlt.position() > alt.position() && ok) {
					QMessageBox::warning(w, tr("Warning"), tr("Crosssection data is not ordered correctly at %1.").arg(newPoint->name()));
					ok = false;
				}
				prevAlt = alt;
			}
			double shiftValue = 0;
			if (m_cpSetting == GeoDataRiverSurveyImporterSettingDialog::cpMiddle) {
				left = newPoint->crosssection().leftBank().position();
				right = newPoint->crosssection().rightBank().position();
				shiftValue = (left + right) * 0.5;
			} else if (m_cpSetting == GeoDataRiverSurveyImporterSettingDialog::cpElevation) {
				shiftValue = minpos;
			}
			newPoint->shiftCenter(shiftValue);
		}
		newPoint->InhibitInterpolatorUpdate = false;

		tail->addPathPoint(newPoint);
		tail = newPoint;
	}
	if (ok) {
		rs->updateInterpolators();
	}
	clearPoints();
	return ok;
}

const QStringList GeoDataRiverSurveyImporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("River Survey data (*.riv)");
	return ret;
}

const QStringList GeoDataRiverSurveyImporter::acceptableExtensions()
{
	QStringList ret;
	ret << "riv";
	return ret;
}

void GeoDataRiverSurveyImporter::clearPoints()
{
	for (auto p : m_points)
	{
		delete p;
	}
	m_points.clear();
}
