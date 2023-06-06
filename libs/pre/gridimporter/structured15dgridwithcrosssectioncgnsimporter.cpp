#include "structured15dgridwithcrosssectioncgnsimporter.h"

#include <guicore/project/projectcgnsfile.h>

#include <h5cgnszone.h>
#include <h5groupcloser.h>
#include <h5util.h>
#include <iriclib_errorcodes.h>

Structured15DGridWithCrossSectionCgnsImporter::Structured15DGridWithCrossSectionCgnsImporter() :
	CgnsGridImporter()
{}

SolverDefinitionGridType::GridType Structured15DGridWithCrossSectionCgnsImporter::supportedGridType() const
{
	return SolverDefinitionGridType::gtNormal1_5DGridWithCrosssection;
}

bool Structured15DGridWithCrossSectionCgnsImporter::isZoneCompatible(const iRICLib::H5CgnsZone& zone)
{
	if (zone.type() != iRICLib::H5CgnsZone::Type::Unstructured) {return false;}

	auto size = zone.size();
	int riversize = size.at(0);

	std::unordered_set<std::string> names;
	int ier = iRICLib::H5Util::getGroupNames(zone.groupId(), &names);
	if (ier != IRIC_NO_ERROR) {return false;}
	if (names.find("GridCrosssections") == names.end()) {return false;}

	hid_t csGroup;

	ier = iRICLib::H5Util::openGroup(zone.groupId(), "GridCrosssections", iRICLib::H5Util::userDefinedDataLabel(), &csGroup);
	if (ier != IRIC_NO_ERROR) {return false;}

	iRICLib::H5GroupCloser closer(csGroup);

	std::vector<std::string> childNames;
	ier = iRICLib::H5Util::getGroupNames(csGroup, &childNames);
	if (ier != IRIC_NO_ERROR) {return false;}

	if (riversize != static_cast<int>(childNames.size())) {return false;}

	return true;
}
