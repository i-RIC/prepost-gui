#include "geodatarivercrosssection.h"
#include "geodatariverpathpoint.h"
#include "geodatariversurvey.h"
#include "geodatariversurveyimporter.h"
#include "geodatariversurveyimportersettingdialog.h"
#include "private/geodatariversurveyimporter_problemsdialog.h"

#include <misc/errormessage.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>

#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QMultiMap>
#include <QTextStream>
#include <QVector2D>

#include <algorithm>
#include <cmath>
#include <vector>

namespace {

const double WRONG_KP = -9999;
const double SHIFT_LEN = 0.001;

GeoDataRiverSurveyImporter::RivPathPoint* getPoint(double kp, std::string& kpStr, std::vector<GeoDataRiverSurveyImporter::RivPathPoint*>* points)
{
	for (auto p : *points) {
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
	auto newp = new GeoDataRiverSurveyImporter::RivPathPoint();
	newp->banksIsSet = false;
	newp->realKP = kp;
	newp->strKP = kpStr;
	points->push_back(newp);
	return newp;
}

void setBanks(GeoDataRiverSurveyImporter::RivPathPoint* p, double lx, double ly, double rx, double ry)
{
	p->banksIsSet = true;
	p->leftBank.setX(lx);
	p->leftBank.setY(ly);
	p->rightBank.setX(rx);
	p->rightBank.setY(ry);
}

bool lessKP(GeoDataRiverSurveyImporter::RivPathPoint* a1, GeoDataRiverSurveyImporter::RivPathPoint* a2)
{
	return a1->realKP > a2->realKP;
}

bool largerAlt(const GeoDataRiverSurveyImporter::Alt& a1, const GeoDataRiverSurveyImporter::Alt& a2)
{
	return a1.distance < a2.distance;
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

bool readRivFile(const QString& fname, std::vector<GeoDataRiverSurveyImporter::RivPathPoint*>* points, bool* with4points, bool* allNamesAreNumber, QWidget* w)
{
	QFile f(fname);
	QFileInfo finfo(f);

	*allNamesAreNumber = true;
	int mode;
	*with4points = true;

	// open file
	int linenum = 1;
	f.open(QIODevice::ReadOnly | QIODevice::Text);
	while (! f.atEnd()) {
		QString line = f.readLine();
		QRegExp sep("\\s+");
		QStringList tokens = line.split(sep, QString::SkipEmptyParts);

		if (tokens.length() >= 1) {
			if (tokens.at(0) == "#survey") {
				mode = 1;
				++linenum;
				continue;
			} else if (tokens.at(0) == "#x-section") {
				mode = 2;
				++linenum;
				continue;
			}
			std::string strKP;
			double realKP;
			GeoDataRiverSurveyImporter::RivPathPoint* p = nullptr;
			int np;
			std::vector<GeoDataRiverSurveyImporter::Alt> alts;
			int divIndices[4];

			bool ok;
			if (mode == 1) {
				parseKP(tokens.at(0), &realKP, &strKP, allNamesAreNumber);
				double leftx = tokens.at(1).toDouble(&ok);
				if (! ok) {
					QMessageBox::critical(w, GeoDataRiverSurveyImporter::tr("Error"), GeoDataRiverSurveyImporter::tr("%1 line %2 \"%3\": LX value is invalid")
																.arg(finfo.fileName()).arg(linenum).arg(tokens.at(1)));
					return false;
				}
				double lefty = tokens.at(2).toDouble(&ok);
				if (! ok) {
					QMessageBox::critical(w, GeoDataRiverSurveyImporter::tr("Error"), GeoDataRiverSurveyImporter::tr("%1 line %2 \"%3\": LY value is invalid")
																.arg(finfo.fileName()).arg(linenum).arg(tokens.at(2)));
					return false;
				}
				double rightx = tokens.at(3).toDouble(&ok);
				if (! ok) {
					QMessageBox::critical(w, GeoDataRiverSurveyImporter::tr("Error"), GeoDataRiverSurveyImporter::tr("%1 line %2 \"%3\": RX value is invalid")
																.arg(finfo.fileName()).arg(linenum).arg(tokens.at(3)));
					return false;
				}
				double righty = tokens.at(4).toDouble(&ok);
				if (! ok) {
					QMessageBox::critical(w, GeoDataRiverSurveyImporter::tr("Error"), GeoDataRiverSurveyImporter::tr("%1 line %2 \"%3\": RY value is invalid")
																.arg(finfo.fileName()).arg(linenum).arg(tokens.at(4)));
					return false;
				}

				p = getPoint(realKP, strKP, points);
				setBanks(p, leftx, lefty, rightx, righty);
			} else if (mode == 2) {
				parseKP(tokens.at(0), &realKP, &strKP, allNamesAreNumber);

				if (tokens.length() < 2) {break;}
				np = static_cast<int>(tokens.at(1).toDouble(&ok));
				if (! ok) {
					QMessageBox::critical(w, GeoDataRiverSurveyImporter::tr("Error"), GeoDataRiverSurveyImporter::tr("%1 line %2 \"%3\": Point count is invalid")
																.arg(finfo.fileName()).arg(linenum).arg(tokens.at(1)));
					return false;
				}
				// Tries to read four indices values.
				if (tokens.length() >= 6) {
					for (int k = 0; k < 4; ++k) {
						divIndices[k] = tokens.at(k + 2).toInt(&ok);
						if (! ok) {
							QMessageBox::critical(w, GeoDataRiverSurveyImporter::tr("Error"), GeoDataRiverSurveyImporter::tr("%1 line %2 \"%3\": Invalid index value")
																		.arg(finfo.fileName()).arg(linenum).arg(tokens.at(k + 2)));
							return false;
						}
					}
				} else {
					*with4points = false;
				}
				p = getPoint(realKP, strKP, points);
				int i = 0;
				while (i < np) {
					if (f.atEnd()) {
						QMessageBox::critical(w, GeoDataRiverSurveyImporter::tr("Error"), GeoDataRiverSurveyImporter::tr("%1 line %2: cross section data for %3 not enough")
																	.arg(finfo.fileName()).arg(linenum).arg(strKP.c_str()));
						return false;
					}
					line = f.readLine();
					++linenum;
					tokens = line.split(sep, QString::SkipEmptyParts);

					int k = 0;
					while (k < tokens.length() - 1) {
						GeoDataRiverSurveyImporter::Alt alt;
						alt.distance = tokens.at(k++).toDouble(&ok);
						if (! ok) {
							QMessageBox::critical(w, GeoDataRiverSurveyImporter::tr("Error"), GeoDataRiverSurveyImporter::tr("%1 line %2 \"%3\": Distance value is invalid")
																		.arg(finfo.fileName()).arg(linenum).arg(tokens.at(k - 1)));
							return false;
						}
						alt.elevation = tokens.at(k++).toDouble(&ok);
						if (! ok) {
							QMessageBox::critical(w, GeoDataRiverSurveyImporter::tr("Error"), GeoDataRiverSurveyImporter::tr("%1 line %2 \"%3\": Elevation value is invalid")
																		.arg(finfo.fileName()).arg(linenum).arg(tokens.at(k - 1)));
							return false;
						}
						alts.push_back(alt);
						++ i;
					}
				}
				GeoDataRiverSurveyImporter::shiftUniqueAlts(&alts, &(p->shifted));
				GeoDataRiverSurveyImporter::sortAlts(&alts, &(p->sorted));
				GeoDataRiverSurveyImporter::uniqueAlts(&alts, &(p->uniquedDistances));
				if (*with4points) {
					for (int i = 0; i < 4; ++i) {
						p->divIndices[i] = divIndices[i];
					}
				}
				p->altitudes = alts;
			}
		}
		++linenum;
	}
	f.close();

	std::vector<GeoDataRiverSurveyImporter::ProblemsDialog::Problem> problems;
	for (GeoDataRiverSurveyImporter::RivPathPoint* p : *points) {
		GeoDataRiverSurveyImporter::ProblemsDialog::Problem problem;
		problem.name = p->strKP.c_str();
		if (! p->banksIsSet) {
			problem.problem = GeoDataRiverSurveyImporter::tr("#survey data is not found. Will be ignored.");
			problems.push_back(problem);
		} else if (p->altitudes.size() == 0) {
			problem.problem = GeoDataRiverSurveyImporter::tr("#x-section data is not found. Will be ignored.");
			problems.push_back(problem);
		} else {
			if (p->uniquedDistances.size() > 0) {
				auto distances = GeoDataRiverSurveyImporter::distListString(p->uniquedDistances);
				problem.problem = GeoDataRiverSurveyImporter::tr("#x-section data contained data with same distances. Data with lowest elevation is used.: %1").arg(distances);
				problems.push_back(problem);
			} else if (p->sorted) {
				problem.problem = GeoDataRiverSurveyImporter::tr("#x-section data is not correctly ordered. Will be sorted automatically.");
				problems.push_back(problem);
			} else if (p->shifted) {
				problem.problem = GeoDataRiverSurveyImporter::tr("#x-section data contained data with same distances. Data distance is shifted slightly so that you can import both points.");
				problems.push_back(problem);
			}
		}
	}
	if (problems.size() > 0) {
		GeoDataRiverSurveyImporter::ProblemsDialog dialog(w);
		dialog.setProblems(problems);
		int result = dialog.exec();
		if (result == QDialog::Rejected) {return false;}
	}

	GeoDataRiverSurveyImporter::removePointsWithoutBanks(points);
	GeoDataRiverSurveyImporter::removePointsWithoutAltitudes(points);
	return (mode == 2);
}

GeoDataRiverSurveyImporter::Alt lowestAlt(const std::vector<GeoDataRiverSurveyImporter::Alt>& alts)
{
	auto ret = alts[0];
	for (int i = 1; i < alts.size(); ++i) {
		const auto& a = alts.at(i);
		if (a.elevation < ret.elevation) {
			ret = a;
		}
	}
	return ret;
}

} // namespace

GeoDataRiverSurveyImporter::Alt::Alt() :
	Alt(0, 0)
{}

GeoDataRiverSurveyImporter::Alt::Alt(double dis, double e) :
	distance {dis},
	elevation {e}
{}

bool GeoDataRiverSurveyImporter::Alt::operator==(const Alt& other) const
{
	return (this->distance == other.distance) && (this->elevation == other.elevation);
}

QPointF GeoDataRiverSurveyImporter::RivPathPoint::banksCenter() const
{
	return QPointF((leftBank.x() + rightBank.x()) * 0.5, (leftBank.y() + rightBank.y()) * 0.5);
}

double GeoDataRiverSurveyImporter::RivPathPoint::banksDistance() const
{
	double dx = rightBank.x() - leftBank.x();
	double dy = rightBank.y() - leftBank.y();

	return std::sqrt(dx * dx + dy * dy);
}

QPointF GeoDataRiverSurveyImporter::RivPathPoint::leftToRight() const
{
	return QPointF(rightBank.x() - leftBank.x(), rightBank.y() - leftBank.y());
}

GeoDataRiverSurveyImporter::GeoDataRiverSurveyImporter(GeoDataCreator* creator) :
	GeoDataImporter("riversurvey", tr("River Survey data (*.riv)"), creator)
{}

bool GeoDataRiverSurveyImporter::doInit(const QString& filename, const QString& /*selectedFilter*/, int* count, SolverDefinitionGridAttribute* /*condition*/, PreProcessorGeoDataGroupDataItemInterface* /*item*/, QWidget* w)
{
	clearPoints(&m_points);
	if (! readRivFile(filename, &m_points, &m_with4Points, &m_allNamesAreNumber, w)) {return false;}

	GeoDataRiverSurveyImporterSettingDialog dialog(w);
	dialog.setWith4Points(m_with4Points);
	dialog.setAllNamesAreNumber(m_allNamesAreNumber);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {
		clearPoints(&m_points);
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
	GeoDataRiverSurvey* rs = dynamic_cast<GeoDataRiverSurvey*>(data);
	rs->setEditMode();

	return importData(rs, &m_points, m_cpSetting, m_with4Points, w);
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

bool GeoDataRiverSurveyImporter::importData(GeoDataRiverSurvey* rs, std::vector<RivPathPoint*>* points, GeoDataRiverSurveyImporterSettingDialog::CenterPointSetting cpSetting, bool with4Points, QWidget* w)
{
	GeoDataRiverPathPoint *tail = rs->m_headPoint;

	double left = 0, right = 0;
	double minpos = 0, minval = 0;
	bool ok = true;

	for (int i = 0; i < points->size(); ++i) {
		RivPathPoint* p = points->at(i);

		auto leftp = p->leftBank;
		auto newPoint = new GeoDataRiverPathPoint(leftp.x(), leftp.y());

		newPoint->setName(p->strKP.c_str());
		newPoint->InhibitInterpolatorUpdate = true;
		newPoint->setCrosssectionDirection(iRIC::normalize(p->leftToRight()));

		if (with4Points) {
			GeoDataRiverCrosssection::Altitude prevAlt(0, 0);
			for (int j = 0; j < p->altitudes.size(); ++j) {
				const auto& a = p->altitudes.at(j);
				GeoDataRiverCrosssection::Altitude alt(a.distance, a.elevation);
				if (j + 1 < p->divIndices[0] || j + 1 > p->divIndices[3]) {continue;}

				newPoint->crosssection().AltitudeInfo().push_back(alt);
				if (j >= p->divIndices[0] && prevAlt.position() > alt.position() && ok) {
					QMessageBox::warning(w, tr("Warning"), tr("Cross Section data is not ordered correctly at %1.").arg(newPoint->name()));
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
			if (cpSetting == GeoDataRiverSurveyImporterSettingDialog::cpMiddle) {
				shiftValue = (left + right) * 0.5;
			} else if (cpSetting == GeoDataRiverSurveyImporterSettingDialog::cpElevation) {
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
			if (cpSetting == GeoDataRiverSurveyImporterSettingDialog::cpMiddle) {
				left = newPoint->crosssection().leftBank().position();
				right = newPoint->crosssection().rightBank().position();
				shiftValue = (left + right) * 0.5;
			} else if (cpSetting == GeoDataRiverSurveyImporterSettingDialog::cpElevation) {
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
	clearPoints(points);
	return ok;
}

void GeoDataRiverSurveyImporter::removePointsWithoutBanks(std::vector<RivPathPoint*>* points)
{
	int idx = 0;
	while (idx < points->size()) {
		if (! points->at(idx)->banksIsSet) {
			delete points->at(idx);
			points->erase(points->begin() + idx);
			continue;
		}
		++ idx;
	}
}

void GeoDataRiverSurveyImporter::removePointsWithoutAltitudes(std::vector<RivPathPoint*>* points)
{
	int idx = 0;
	while (idx < points->size()) {
		if (points->at(idx)->altitudes.size() == 0) {
			delete points->at(idx);
			points->erase(points->begin() + idx);
			continue;
		}
		++ idx;
	}
}

void GeoDataRiverSurveyImporter::clearPoints(std::vector<RivPathPoint*>* points)
{
	for (auto p : *points) {
		delete p;
	}
	points->clear();
}

void GeoDataRiverSurveyImporter::sortReverse(std::vector<GeoDataRiverSurveyImporter::RivPathPoint*>* points)
{
	std::reverse(points->begin(), points->end());
}

void GeoDataRiverSurveyImporter::sortByKP(std::vector<GeoDataRiverSurveyImporter::RivPathPoint*>* points)
{
	std::sort(points->begin(), points->end(), lessKP);
}

void GeoDataRiverSurveyImporter::shiftUniqueAlts(std::vector<GeoDataRiverSurveyImporter::Alt>* altitudes, bool* shifted)
{
	*shifted = false;
	for (int i = 1; i < altitudes->size(); ++i) {
		Alt& a = altitudes->at(i);
		const Alt& a_prev = altitudes->at(i - 1);
		if (a.distance != a_prev.distance) {continue;}

		if (i < altitudes->size() - 1) {
			const Alt& a_next = altitudes->at(i + 1);
			if (a_next.distance > a.distance && a_next.distance < a.distance + SHIFT_LEN) {
				a.distance = (a.distance + a_next.distance) * 0.5;
			} else {
				a.distance += SHIFT_LEN;
			}
		} else {
			a.distance += SHIFT_LEN;
		}
		*shifted = true;
	}
}

void GeoDataRiverSurveyImporter::sortAlts(std::vector<GeoDataRiverSurveyImporter::Alt>* altitudes, bool* sorted)
{
	auto origAltitudes = *altitudes;
	std::sort(altitudes->begin(), altitudes->end(), largerAlt);

	*sorted = (*altitudes != origAltitudes);
}

void GeoDataRiverSurveyImporter::uniqueAlts(std::vector<Alt>* altitudes, std::vector<double>* distlist)
{
	std::vector<Alt> ret;
	auto it = altitudes->begin();
	std::vector<Alt> alts;
	while (it != altitudes->end()) {
		if (alts.size() == 0 || alts[0].distance == it->distance) {
			alts.push_back(*it);
		} else {
			if (alts.size() > 1) {
				distlist->push_back(alts[0].distance);
			}
			ret.push_back(lowestAlt(alts));
			alts.clear();
			alts.push_back(*it);
		}
		++ it;
	}
	if (alts.size() == 1) {
		ret.push_back(alts[0]);
	} else if (alts.size() > 1) {
		distlist->push_back(alts[0].distance);
		ret.push_back(lowestAlt(alts));
	}
	*altitudes = ret;
}

QString GeoDataRiverSurveyImporter::distListString(const std::vector<double>& distlist)
{
	QStringList frags;
	for (double d : distlist) {
		frags.append(QString::number(d));
	}
	return frags.join(", ");
}
