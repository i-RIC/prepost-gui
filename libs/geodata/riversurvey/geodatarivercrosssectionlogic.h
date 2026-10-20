#ifndef GEODATARIVERCROSSSECTIONLOGIC_H
#define GEODATARIVERCROSSSECTIONLOGIC_H

#include "gd_riversurvey_global.h"
#include "geodatarivercrosssection.h"
#include "geodatariverpathpointodndata.h"

#include <QList>
#include <QString>

#include <vector>

/// Pure functions used by the cross-section window and related commands.
/// They do not depend on GUI objects, so that they can be tested by unit tests.
namespace GeoDataRiverCrosssectionLogic {

/// Returns true when the two cross-section names should be treated as the same one.
/// Names that are both numbers are compared numerically, with a tolerance of 1.0E-6.
GD_RIVERSURVEY_EXPORT bool isSameCrosssectionName(const QString& n1, const QString& n2);

/// Builds the list of cross-section names to show in the combo box.
/// Names that are the same in terms of isSameCrosssectionName() appear only once (the first one is used).
/// When all names are numbers, names are sorted by the value. Otherwise, sorted as strings.
GD_RIVERSURVEY_EXPORT QList<QString> buildCrosssectionNames(const std::vector<QString>& names);

/// Calculates the ODN data after the altitude list is modified from "before" to "after".
/// ODN points that are not set (negative index) are kept unset.
GD_RIVERSURVEY_EXPORT GeoDataRiverPathPointOdnData remapOdn(const GeoDataRiverCrosssection::AltitudeList& before, const GeoDataRiverCrosssection::AltitudeList& after, const GeoDataRiverPathPointOdnData& odn);

/// Edits the cross section by adding newAlt, starting from the point at selectedIndex.
/// The points between the selected point and newAlt are removed.
/// newIndex is set to the index of newAlt in the returned list.
GD_RIVERSURVEY_EXPORT GeoDataRiverCrosssection::AltitudeList editFromPoint(const GeoDataRiverCrosssection::AltitudeList& before, int selectedIndex, const GeoDataRiverCrosssection::Altitude& newAlt, int* newIndex);

/// Returns the indices sorted in ascending order without duplicates.
GD_RIVERSURVEY_EXPORT std::vector<int> sortedUniqueIndices(const std::vector<int>& indices);

} // namespace GeoDataRiverCrosssectionLogic

#endif // GEODATARIVERCROSSSECTIONLOGIC_H
