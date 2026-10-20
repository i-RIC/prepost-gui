#include "geodatarivercrosssectionlogic.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <set>

namespace {

const double NAME_DELTA = 1.0E-6;

bool toNumber(const QString& name, double* value)
{
	bool ok = false;
	*value = name.toDouble(&ok);
	return ok;
}

int findSamePosition(const GeoDataRiverCrosssection::AltitudeList& list, double position)
{
	auto it = std::lower_bound(list.begin(), list.end(), GeoDataRiverCrosssection::Altitude(position, 0));
	if (it == list.end() || it->position() != position) {return -1;}

	return static_cast<int> (it - list.begin());
}

} // namespace

bool GeoDataRiverCrosssectionLogic::isSameCrosssectionName(const QString& n1, const QString& n2)
{
	double v1, v2;
	if (toNumber(n1, &v1) && toNumber(n2, &v2)) {
		return std::abs(v1 - v2) <= NAME_DELTA;
	}

	return n1 == n2;
}

QList<QString> GeoDataRiverCrosssectionLogic::buildCrosssectionNames(const std::vector<QString>& names)
{
	std::set<QString> nonNumericNames;
	std::multimap<double, QString> numericNames;
	bool allNumerical = true;

	for (const QString& name : names) {
		double value;
		if (toNumber(name, &value)) {
			auto it = numericNames.lower_bound(value - NAME_DELTA);
			if (it != numericNames.end() && it->first <= value + NAME_DELTA) {continue;}

			numericNames.insert({value, name});
		} else {
			allNumerical = false;
			nonNumericNames.insert(name);
		}
	}

	QList<QString> ret;
	if (allNumerical) {
		for (const auto& pair : numericNames) {
			ret.push_back(pair.second);
		}
	} else {
		std::set<QString> all = nonNumericNames;
		for (const auto& pair : numericNames) {
			all.insert(pair.second);
		}
		for (const QString& name : all) {
			ret.push_back(name);
		}
	}
	return ret;
}

GeoDataRiverPathPointOdnData GeoDataRiverCrosssectionLogic::remapOdn(const GeoDataRiverCrosssection::AltitudeList& before, const GeoDataRiverCrosssection::AltitudeList& after, const GeoDataRiverPathPointOdnData& odn)
{
	GeoDataRiverPathPointOdnData ret = odn;
	int beforeSize = static_cast<int> (before.size());
	int afterSize = static_cast<int> (after.size());

	for (int i = 0; i < 6; ++i) {
		int index = odn.nb(i);
		if (index < 0) {continue;}

		if (afterSize == 0) {
			ret.setNb(i, -1);
			continue;
		}
		if (index >= beforeSize) {
			ret.setNb(i, std::min(index, afterSize - 1));
			continue;
		}

		double odnPos = before.at(index).position();
		if (index < afterSize && after.at(index).position() == odnPos) {
			ret.setNb(i, index);
			continue;
		}
		int index2 = index + afterSize - beforeSize;
		if (index2 >= 0 && index2 < afterSize && after.at(index2).position() == odnPos) {
			ret.setNb(i, index2);
			continue;
		}
		int found = findSamePosition(after, odnPos);
		if (found >= 0) {
			ret.setNb(i, found);
			continue;
		}
		if (afterSize == beforeSize) {
			// the point was moved. points correspond one by one.
			ret.setNb(i, index);
			continue;
		}
		// the point was removed.
		if (i < 3) {
			ret.setNb(i, 0);
		} else {
			ret.setNb(i, afterSize - 1);
		}
	}
	return ret;
}

GeoDataRiverCrosssection::AltitudeList GeoDataRiverCrosssectionLogic::editFromPoint(const GeoDataRiverCrosssection::AltitudeList& before, int selectedIndex, const GeoDataRiverCrosssection::Altitude& newAlt, int* newIndex)
{
	auto after = before;
	if (selectedIndex < 0 || selectedIndex >= static_cast<int> (after.size())) {
		*newIndex = -1;
		return after;
	}

	const double selectedPos = after.at(selectedIndex).position();
	int eraseBegin, eraseEnd;
	if (newAlt.position() > selectedPos) {
		// remove points whose position is in (selectedPos, newAlt.position()]
		eraseBegin = selectedIndex + 1;
		eraseEnd = eraseBegin;
		while (eraseEnd < static_cast<int> (after.size()) && after.at(eraseEnd).position() <= newAlt.position()) {
			++ eraseEnd;
		}
		*newIndex = selectedIndex + 1;
	} else {
		// remove points whose position is in [newAlt.position(), selectedPos)
		eraseEnd = selectedIndex;
		eraseBegin = eraseEnd;
		while (eraseBegin > 0 && after.at(eraseBegin - 1).position() >= newAlt.position()) {
			-- eraseBegin;
		}
		*newIndex = selectedIndex - (eraseEnd - eraseBegin);
	}
	after.erase(after.begin() + eraseBegin, after.begin() + eraseEnd);
	after.insert(after.begin() + *newIndex, newAlt);

	return after;
}

std::vector<int> GeoDataRiverCrosssectionLogic::sortedUniqueIndices(const std::vector<int>& indices)
{
	auto ret = indices;
	std::sort(ret.begin(), ret.end());
	ret.erase(std::unique(ret.begin(), ret.end()), ret.end());
	return ret;
}
