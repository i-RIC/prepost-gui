#include "geodatariversurvey_areacalculator.h"

#include <misc/mathsupport.h>

#include <QFile>
#include <QTextStream>

#include <unordered_map>

namespace {

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
				GeoDataRiverCrosssection::Altitude after_new(it_before->position(), e);
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
				GeoDataRiverCrosssection::Altitude before_new(it_after->position(), e);
				after2.push_back(before_new);
				after_prev = before_new;
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

} // namespace

GeoDataRiverSurvey::AreaCalculator::AreaCalculator(GeoDataRiverSurvey* before, GeoDataRiverSurvey* after, const QString fileName) :
	m_before {before},
	m_after {after},
	m_fileName {fileName}
{}

bool GeoDataRiverSurvey::AreaCalculator::calculate(QWidget* w)
{
	QFile f(m_fileName);
	bool ok = f.open(QIODevice::WriteOnly | QIODevice::Text);
	if (! ok) {
		QMessageBox::critical(w, GeoDataRiverSurvey::tr("Error"), GeoDataRiverSurvey::tr("Opening %1 failed.").arg(QDir::toNativeSeparators(m_fileName)));
		return false;
	}

	QTextStream s(&f);
	s << GeoDataRiverSurvey::tr("Cross Section") << "," << GeoDataRiverSurvey::tr("Start position") << "," << GeoDataRiverSurvey::tr("End position") << "," << GeoDataRiverSurvey::tr("Type") << "," << GeoDataRiverSurvey::tr("Area") << "," << GeoDataRiverSurvey::tr("Notice") << "\n";

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
			s << after_p->name() << ",,,,," << GeoDataRiverSurvey::tr("%1 does not have a cross section named \"%2\"").arg(m_before->caption()).arg(after_p->name());
		} else {
			auto before_p = it->second;
			calculate(before_p, after_p, &s);
		}

		after_p = after_p->nextPoint();
	}

	return true;
}

void GeoDataRiverSurvey::AreaCalculator::calculate(GeoDataRiverPathPoint* before_p, GeoDataRiverPathPoint* after_p, QTextStream* s)
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
	bool prev_equal = true;

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
					double startPos = after_work.begin()->position() + after_p->crosssection().leftShift();
					double endPos = after_work.rbegin()->position() + after_p->crosssection().leftShift();
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
					before_work.push_back(*it_after);
				} else if ((it_before + 1 == before.end() || it_after + 1 == after.end()) && working) {
					working = false;
					before_work.push_back(*it_before);
					before_work.push_back(*it_after);

					double area = calculateArea(before_work, after_work);
					QString type = GeoDataRiverSurvey::tr("Embankment");
					if (area < 0) {
						area = - area;
						type = GeoDataRiverSurvey::tr("Cutting");
					}
					double startPos = after_work.begin()->position() + after_p->crosssection().leftShift();
					double endPos = after_work.rbegin()->position() + after_p->crosssection().leftShift();
					*s << after_p->name() << "," << startPos << "," << endPos << "," << type << "," << area << "," << "\n";

					output = true;
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

