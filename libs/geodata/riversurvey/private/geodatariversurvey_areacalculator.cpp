#include "geodatariversurvey_areacalculator.h"
#include "../geodatariversurveyodnchecker.h"

#include <misc/mathsupport.h>
#include <misc/stringtool.h>

#include <QFile>
#include <QTextStream>

#include <unordered_map>

namespace {

struct AreaSegment {
	double start;
	double end;
	double area;
};

const double DELTA = 1.0E-6;

void addCrossSectionPoints(GeoDataRiverCrosssection::AltitudeList* before, GeoDataRiverCrosssection::AltitudeList* after)
{
	GeoDataRiverCrosssection::AltitudeList before2, after2;

	auto it_before = before->begin();
	auto it_after = after->begin();

	GeoDataRiverCrosssection::Altitude before_prev, after_prev;

	while ((it_before != before->end()) || (it_after != after->end())) {
		if (it_before == before->end()) {
			if (it_after->position() > before_prev.position() && after_prev.position() < before_prev.position()) {
				double r = (before_prev.position() - after_prev.position()) / (it_after->position() - after_prev.position());
				double e = (1 - r) * after_prev.height() + r * it_after->height();
				GeoDataRiverCrosssection::Altitude after_new(before_prev.position(), e);
				after2.push_back(after_new);
				after_prev = after_new;
			}
			after2.push_back(*it_after);
			after_prev = *it_after;
			++ it_after;
			continue;
		}
		if (it_after == after->end()) {
			if (it_before->position() > after_prev.position() && before_prev.position() < after_prev.position()) {
				double r = (after_prev.position() - before_prev.position()) / (it_before->position() - before_prev.position());
				double e = (1 - r) * before_prev.height() + r * it_before->height();
				GeoDataRiverCrosssection::Altitude before_new(after_prev.position(), e);
				before2.push_back(before_new);
				before_prev = before_new;
			}
			before2.push_back(*it_before);
			before_prev = *it_before;
			++ it_before;
			continue;
		}
		if (it_before == before->begin()){
			if (it_before->position() > it_after->position()) {
				after2.push_back(*it_after);
				after_prev = *it_after;
				++ it_after;
			} else if (it_before->position() < it_after->position()) {
				if (it_after != after->begin()) {
					double r = (it_before->position() - after_prev.position()) / (it_after->position() - after_prev.position());
					double e = (1 - r) * after_prev.height() + r * it_after->height();
					GeoDataRiverCrosssection::Altitude after_new(it_before->position(), e);
					after2.push_back(after_new);
					after_prev = after_new;
				}
				before2.push_back(*it_before);
				before_prev = *it_before;
				++ it_before;
			} else {
				before2.push_back(*it_before);
				before_prev = *it_before;
				++ it_before;

				after2.push_back(*it_after);
				after_prev = *it_after;
				++ it_after;
			}
			continue;
		}
		if (it_after == after->begin()){
			if (it_after->position() > it_before->position()) {
				before2.push_back(*it_before);
				before_prev = *it_before;
				++ it_before;
			} else if (it_after->position() < it_before->position()) {
				if (it_before != before->begin()) {
					double r = (it_after->position() - before_prev.position()) / (it_before->position() - before_prev.position());
					double e = (1 - r) * before_prev.height() + r * it_before->height();
					GeoDataRiverCrosssection::Altitude before_new(it_after->position(), e);
					before2.push_back(before_new);
					before_prev = before_new;
				}
				after2.push_back(*it_after);
				after_prev = *it_after;
				++ it_after;
			} else {
				before2.push_back(*it_before);
				before_prev = *it_before;
				++ it_before;

				after2.push_back(*it_after);
				after_prev = *it_after;
				++ it_after;
			}
			continue;
		}

		if (it_before->position() == it_after->position() && it_before->height() == it_after->height()) {
			before2.push_back(*it_before);
			before_prev = *it_before;
			++ it_before;

			after2.push_back(*it_after);
			after_prev = *it_after;
			++ it_after;
		} else {
			// check for xsection
			QPointF p1(before_prev.position(), before_prev.height());
			QPointF p2(it_before->position(), it_before->height());

			QPointF q1(after_prev.position(), after_prev.height());
			QPointF q2(it_after->position(), it_after->height());

			QPointF xsec;
			double r, s;
			bool crosses = iRIC::intersectionPoint(p1, p2, q1, q2, &xsec, &r, &s);

			if (crosses && 0 <= r && r <= 1 && 0 <= s && s <= 1) {
				// cross section found
				GeoDataRiverCrosssection::Altitude common_new(xsec.x(), xsec.y());
				before2.push_back(common_new);
				before_prev = common_new;

				after2.push_back(common_new);
				after_prev = common_new;
			}
			bool prev_equal = (before_prev == after_prev);

			if (it_before->position() < it_after->position()) {
				before2.push_back(*it_before);
				before_prev = *it_before;
				++ it_before;

				if (prev_equal) {
					QPointF leg;
					iRIC::perpendicularLineOfLeg(q1, q2, p2, &leg);
					if (iRIC::length(p2 - leg) < DELTA * DELTA) {
						after2.push_back(before_prev);
						after_prev = before_prev;
					}
				}
			} else if (it_after->position() < it_before->position()) {
				after2.push_back(*it_after);
				after_prev = *it_after;
				++ it_after;

				if (prev_equal) {
					QPointF leg;
					iRIC::perpendicularLineOfLeg(p1, p2, q2, &leg);
					if (iRIC::length(q2 - leg) < DELTA * DELTA) {
						before2.push_back(after_prev);
						before_prev = after_prev;
					}
				}
			} else {
				before2.push_back(*it_before);
				before_prev = *it_before;
				++ it_before;

				after2.push_back(*it_after);
				after_prev = *it_after;
				++ it_after;
			}
		}
	}
	*before = before2;
	*after = after2;
}

double calculateArea(const GeoDataRiverCrosssection::AltitudeList& alist) {
	double area = 0;

	for (int i = 0; i < alist.size() - 1; ++i) {
		auto& a1 = alist.at(i);
		auto& a2 = alist.at(i + 1);

		area += (a2.position() - a1.position()) * (a1.height() + a2.height()) * 0.5;
	}
	return area;
}

double calculateArea(const GeoDataRiverCrosssection::AltitudeList& before, const GeoDataRiverCrosssection::AltitudeList& after) {
	return calculateArea(after) - calculateArea(before);
}

double calculateArea(const GeoDataRiverCrosssection::Altitude& before_start, const GeoDataRiverCrosssection::Altitude& before_end,
										 const GeoDataRiverCrosssection::Altitude& after_start, const GeoDataRiverCrosssection::Altitude& after_end)
{
	GeoDataRiverCrosssection::AltitudeList before, after;
	before.push_back(before_start);
	before.push_back(before_end);
	after.push_back(after_start);
	after.push_back(after_end);

	return calculateArea(before, after);
}

} // namespace

GeoDataRiverSurvey::AreaCalculator::AreaCalculator(GeoDataRiverSurvey* before, GeoDataRiverSurvey* after, const QString fileName, bool statistic) :
	m_before {before},
	m_after {after},
	m_fileName {fileName},
	m_statistic {statistic}
{}

bool GeoDataRiverSurvey::AreaCalculator::calculate(QWidget* w)
{
	bool check = GeoDataRiverSurveyOdnChecker::check(m_after, w, true);
	if (! check) {return false;}

	QFile f(m_fileName);
	bool ok = f.open(QIODevice::WriteOnly | QIODevice::Text);
	if (! ok) {
		QMessageBox::critical(w, GeoDataRiverSurvey::tr("Error"), GeoDataRiverSurvey::tr("Opening %1 failed.").arg(QDir::toNativeSeparators(m_fileName)));
		return false;
	}

	QTextStream s(&f);
	s.setCodec("UTF-8");
	s.setGenerateByteOrderMark(true);

	if (m_statistic) {
		calculateStatistic(w, &s);
	} else {
		calculateEach(w, &s);
	}

	return true;
}

void GeoDataRiverSurvey::AreaCalculator::calculateEach(QWidget* w, QTextStream* s)
{
	*s << GeoDataRiverSurvey::tr("Cross Section") << "," << GeoDataRiverSurvey::tr("Start position") << "," << GeoDataRiverSurvey::tr("End position") << "," << GeoDataRiverSurvey::tr("Type") << "," << GeoDataRiverSurvey::tr("Area") << "," << GeoDataRiverSurvey::tr("Notice") << "\n";

	std::unordered_map<QString, GeoDataRiverPathPoint*> before_points;
	GeoDataRiverPathPoint* p = m_before->headPoint()->nextPoint();
	while (p != nullptr) {
		before_points.insert({p->name(), p});

		p = p->nextPoint();
	}
	auto after_p = m_after->headPoint()->nextPoint();
	while (after_p != nullptr) {
		auto it = before_points.find(after_p->name());
		if (it == before_points.end()) {
			*s << after_p->name() << ",,,,," << GeoDataRiverSurvey::tr("%1 does not have a cross section named \"%2\"").arg(m_before->caption()).arg(after_p->name()) << "\n";
		} else {
			auto name = iRIC::toStr(after_p->name());
			auto before_p = it->second;
			calculateEach(before_p, after_p, s);
		}

		after_p = after_p->nextPoint();
	}
}

void GeoDataRiverSurvey::AreaCalculator::calculateEach(GeoDataRiverPathPoint* before_p, GeoDataRiverPathPoint* after_p, QTextStream* s)
{
	GeoDataRiverCrosssection::AltitudeList before = before_p->crosssection().AltitudeInfo();
	GeoDataRiverCrosssection::AltitudeList after = after_p->crosssection().AltitudeInfo();

	if (after_p->crosssection().leftShift() != before_p->crosssection().leftShift()) {
		double offset = after_p->crosssection().leftShift() - before_p->crosssection().leftShift();
		for (auto& alt : before) {
			alt.setPosition(alt.position() - offset);
		}
	}

	addCrossSectionPoints(&before, &after);
	bool output = false;

	auto it_before = before.begin();
	auto it_after = after.begin();

	bool working = false;
	GeoDataRiverCrosssection::AltitudeList before_work, after_work;
	bool prev_equal = false;

	auto ls = after_p->crosssection().leftShift();

	while ((it_before != before.end()) || (it_after != after.end())) {
		if (it_before == before.end()) {
			++ it_after;
			continue;
		}
		if (it_after == after.end()) {
			++ it_before;
			continue;
		}
		if (it_before->position() == it_after->position() && it_before->height() == it_after->height()) {
			if (! prev_equal) {
				working = ! working;

				if (! working) {
					before_work.push_back(*it_before);
					after_work.push_back(*it_after);
					double area = calculateArea(before_work, after_work);
					QString type = GeoDataRiverSurvey::tr("Embankment");
					if (area < 0) {
						area = - area;
						type = GeoDataRiverSurvey::tr("Cutting");
					}
					double startPos = after_work.begin()->position() + ls;
					double endPos = after_work.rbegin()->position() + ls;
					*s << after_p->name() << "," << startPos << "," << endPos << "," << type << "," << area << "," << "\n";

					output = true;
				} else {
					before_work.clear();
					after_work.clear();
					before_work.push_back(*it_before);
					after_work.push_back(*it_after);
				}
			}
			++it_before;
			++it_after;

			prev_equal = true;
		} else {
			if (it_before->position() == it_after->position()) {
				if (it_before == before.begin() || it_after == after.begin()) {
					working = true;

					before_work.clear();
					after_work.clear();
					before_work.push_back(*it_before);
					after_work.push_back(*it_after);
				} else if ((it_before + 1 == before.end() || it_after + 1 == after.end()) && working) {
					working = false;

					before_work.push_back(*it_before);
					after_work.push_back(*it_after);

					double area = calculateArea(before_work, after_work);
					QString type = GeoDataRiverSurvey::tr("Embankment");
					if (area < 0) {
						area = - area;
						type = GeoDataRiverSurvey::tr("Cutting");
					}
					double startPos = after_work.begin()->position() + ls;
					double endPos = after_work.rbegin()->position() + ls;
					*s << after_p->name() << "," << startPos << "," << endPos << "," << type << "," << area << "," << "\n";

					output = true;
				} else if (prev_equal) {
					working = true;

					before_work.clear();
					after_work.clear();
					before_work.push_back(*(it_before - 1));
					before_work.push_back(*it_before);
					after_work.push_back(*(it_after - 1));
					after_work.push_back(*it_after);
				} else if (working) {
					before_work.push_back(*it_before);
					after_work.push_back(*it_after);
				}
				++it_before;
				++it_after;
			} else if (it_before->position() < it_after->position()) {
				if (prev_equal) {
					working = true;

					before_work.clear();
					after_work.clear();
					before_work.push_back(*(it_before - 1));
					after_work.push_back(*(it_after - 1));
				}
				if (working) {
					before_work.push_back(*it_before);
				}
				++ it_before;
			} else {
				if (prev_equal) {
					working = true;

					before_work.clear();
					after_work.clear();
					before_work.push_back(*(it_before - 1));
					after_work.push_back(*(it_after - 1));
				}
				if (working) {
					after_work.push_back(*it_after);
				}
				++ it_after;
			}
			prev_equal = false;
		}
	}

	if (! output) {
		*s << after_p->name() << ",,,,," << GeoDataRiverSurvey::tr("No difference found") << "\n";
	}
}

void GeoDataRiverSurvey::AreaCalculator::calculateStatistic(QWidget* w, QTextStream* s)
{
	*s << GeoDataRiverSurvey::tr("Cross Section") << "," << GeoDataRiverSurvey::tr("Span name") << "," << GeoDataRiverSurvey::tr("Start position") << "," << GeoDataRiverSurvey::tr("End position") << "," << GeoDataRiverSurvey::tr("Embankment")  << "," << GeoDataRiverSurvey::tr("Cutting") << "," << GeoDataRiverSurvey::tr("Total") << "," << GeoDataRiverSurvey::tr("Notice") << "\n";

	std::unordered_map<QString, GeoDataRiverPathPoint*> before_points;
	GeoDataRiverPathPoint* p = m_before->headPoint()->nextPoint();
	while (p != nullptr) {
		before_points.insert({p->name(), p});

		p = p->nextPoint();
	}
	auto after_p = m_after->headPoint()->nextPoint();
	while (after_p != nullptr) {
		auto it = before_points.find(after_p->name());
		if (it == before_points.end()) {
			*s << after_p->name() << ",,,,,," << GeoDataRiverSurvey::tr("%1 does not have a cross section named \"%2\"").arg(m_before->caption()).arg(after_p->name());
		} else {
			auto before_p = it->second;
			calculateStatistic(before_p, after_p, s);
		}

		after_p = after_p->nextPoint();
	}
}

void GeoDataRiverSurvey::AreaCalculator::calculateStatistic(GeoDataRiverPathPoint* before_p, GeoDataRiverPathPoint* after_p, QTextStream* s)
{
	GeoDataRiverCrosssection::AltitudeList before = before_p->crosssection().AltitudeInfo();
	GeoDataRiverCrosssection::AltitudeList after = after_p->crosssection().AltitudeInfo();

	if (after_p->crosssection().leftShift() != before_p->crosssection().leftShift()) {
		double offset = after_p->crosssection().leftShift() - before_p->crosssection().leftShift();
		for (auto& alt : before) {
			alt.setPosition(alt.position() - offset);
		}
	}
	addCrossSectionPoints(&before, &after);

	auto it_before = before.begin();
	auto it_after = after.begin();

	double segmentPositions[8];
	segmentPositions[0] = -1E10;
	segmentPositions[7] = 1E10;

	const auto& alist = after_p->crosssection().AltitudeInfo();
	for (int i = 0; i < 6; ++i) {
		auto idx = after_p->odn().nb(i);
		segmentPositions[i + 1] = alist.at(idx).position();
	}
	QStringList segmentNames;
	segmentNames.push_back(tr("Left than Left Start Point"));
	segmentNames.push_back(tr("Between Left Start Point and Left Middle Point"));
	segmentNames.push_back(tr("Between Left Middle Point and Left Low Point"));
	segmentNames.push_back(tr("Between Left Low Point and Right Low Point"));
	segmentNames.push_back(tr("Between Right Low Point and Right Middle Point"));
	segmentNames.push_back(tr("Between Right Middle Point and Right Start Point"));
	segmentNames.push_back(tr("Right than Right Start Point"));

	std::vector<AreaSegment> areaSegments;

	bool before_prev_set = false, after_prev_set = false, before_start_set = false, after_start_set = false;
	GeoDataRiverCrosssection::Altitude before_prev, after_prev, before_start, after_start, before_end, after_end;

	while ((it_before != before.end()) || (it_after != after.end())) {
		if (it_before == before.end()) {
			++ it_after;
			continue;
		}
		if (it_after == after.end()) {
			++ it_before;
			continue;
		}

		if (it_before->position() < it_after->position()) {
			before_end = *it_before;
			if (after_prev_set) {
				after_end.setPosition(it_before->position());
				double r = (it_before->position() - after_prev.position()) / (it_after->position() - after_prev.position());
				double h = (1 - r) * after_prev.height() + r * it_after->height();
				after_end.setHeight(h);
				if (before_start_set && after_start_set) {
					double area = calculateArea(before_start, before_end, after_start, after_end);
					if (area != 0) {
						AreaSegment segment {before_start.position(), before_end.position(), area};
						areaSegments.push_back(segment);
					}
				}
				before_start = before_end;
				before_start_set = true;
				after_start = after_end;
				after_start_set = true;
			}

			before_prev = *it_before;
			before_prev_set = true;
			++ it_before;
		} else if (it_before->position() > it_after->position()) {
			after_end = *it_after;
			if (before_prev_set) {
				before_end.setPosition(it_after->position());
				double r = (it_after->position() - before_prev.position()) / (it_before->position() - before_prev.position());
				double h = (1 - r) * before_prev.height() + r * it_before->height();
				before_end.setHeight(h);
				if (before_start_set && after_start_set) {
					double area = calculateArea(before_start, before_end, after_start, after_end);
					if (area != 0) {
						AreaSegment segment {before_start.position(), before_end.position(), area};
						areaSegments.push_back(segment);
					}
				}
				before_start = before_end;
				before_start_set = true;
				after_start = after_end;
				after_start_set = true;
			}

			after_prev = *it_after;
			after_prev_set = true;
			++ it_after;
		} else {
			// position is equal
			before_end = *it_before;
			after_end = *it_after;

			if (before_start_set && after_start_set) {
				double area = calculateArea(before_start, before_end, after_start, after_end);
				if (area != 0) {
					AreaSegment segment {before_start.position(), before_end.position(), area};
					areaSegments.push_back(segment);
				}
			}
			before_start = before_end;
			before_start_set = true;
			after_start = after_end;
			after_start_set = true;

			before_prev = *it_before;
			before_prev_set = true;
			++ it_before;

			after_prev = *it_after;
			after_prev_set = true;
			++ it_after;
		}
	}

	bool output = false;
	auto ls = after_p->crosssection().leftShift();
	for (int i = 0; i < 7; ++i) {
		double start = segmentPositions[i];
		double end = segmentPositions[i + 1];
		double add = 0, sub = 0;
		for (const auto& areaSegment : areaSegments) {
			if (start <= areaSegment.start && areaSegment.end <= end) {
				if (areaSegment.area > 0) {
					add += areaSegment.area;
				} else {
					sub -= areaSegment.area;
				}
			}
		}
		if (add != 0 || sub != 0) {
			*s << after_p->name() << "," << segmentNames.at(i) << "," << start + ls << "," << end + ls << ",";
			*s << add << "," << - sub << "," << add - sub << ",\n";
			output = true;
		}
	}

	if (! output) {
		*s << after_p->name() << ",,,,,,," << GeoDataRiverSurvey::tr("No difference found") << "\n";
	}
}
