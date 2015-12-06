#include "../project/projectcgnsfile.h"
#include "postsolutioninfo.h"
#include "postzonepointseriesdatacontainer.h"

#include <misc/stringtool.h>

#include <QRegExp>

#include <cgnslib.h>
#include <cmath>

PostZonePointSeriesDataContainer::PostZonePointSeriesDataContainer(PostSolutionInfo::Dimension dim, const std::string& zoneName, const QString& pName, int pointIndex, ProjectDataItem* parent) :
	PostSeriesDataContainer(dim, parent),
	m_zoneName (zoneName),
	m_zoneId {0},
	m_physName {pName},
	m_pointIndex {pointIndex}
{}

bool PostZonePointSeriesDataContainer::setZoneId(const int fn)
{
	// if m_zoneID is already set, we do not have to do it again.
	if (m_zoneId != 0) {return true;}

	int ier;
	int numZones;
	ier = cg_nzones(fn, m_baseId, &numZones);
	if (ier != 0) {return false;}
	for (int Z = 1; Z <= numZones; ++Z) {
		char zonename[32];
		ier = cg_zone_read(fn, m_baseId, Z, zonename, m_sizes);
		if (ier != 0) {return false;}
		if (m_zoneName == zonename) {
			m_zoneId = Z;
			return true;
		}
	}
	return false;
}

bool PostZonePointSeriesDataContainer::loadData(const int fn)
{
	bool ret;
	int ier;
	int numSols;
	ret = setZoneId(fn);
	if (! ret) {return false;}

	ier = cg_nsols(fn, m_baseId, m_zoneId, &numSols);
	if (ier != 0) {return false;}
	m_data.clear();
	QRegExp rx("^(.+) \\(magnitude\\)$");
	bool magnitude = (rx.indexIn(m_physName) != -1);
	QString tmpPhysName = m_physName;
	if (magnitude) {
		// shortname
		tmpPhysName = rx.cap(1);
	}
	for (int i = 1; i <= numSols; ++i) {
		ier = cg_goto(fn, m_baseId, "Zone_t", m_zoneId, "FlowSolution_t", i, "end");
		if (ier != 0) {return false;}
		int numArrays;
		ier = cg_narrays(&numArrays);
		if (ier != 0) {return false;}
		double value = 0;
		for (int j = 1; j <= numArrays; ++j) {
			DataType_t datatype;
			int dimension;
			cgsize_t dimVector[3];
			char arrayname[30];
			ier = cg_array_info(j, arrayname, &datatype, &dimension, dimVector);
			if (ier != 0) {return false;}
			QString name(arrayname);
			int dataLen = 1;
			for (int dim = 1; dim <= dimension; ++dim) {
				dataLen = dataLen * dimVector[dim - 1];
			}
			std::vector<double> buffer(dataLen);
			ier = cg_array_read_as(j, RealDouble, buffer.data());
			if (magnitude) {
				if (
					name == QString(tmpPhysName).append("X") ||
					name == QString(tmpPhysName).append("Y") ||
					name == QString(tmpPhysName).append("Z")
				) {
					double currVal = buffer[m_pointIndex];
					value += currVal * currVal;
				}
			} else {
				if (tmpPhysName == name) {
					value = buffer[m_pointIndex];
				}
			}
		}
		if (magnitude) {
			value = std::sqrt(value);
		}
		m_data.append(value);
	}
	return true;
}
