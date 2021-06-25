#include "../../../project/projectcgnsfile.h"
#include "structured15dgridwithcrosssectioncrosssection.h"

#include <misc/stringtool.h>

#include <h5cgnszone.h>
#include <h5util.h>
#include <iriclib_errorcodes.h>

#include <sstream>

Structured15DGridWithCrossSectionCrossSection::Structured15DGridWithCrossSectionCrossSection(QString name, Grid* grid) :
	QObject(grid),
	m_grid {grid},
	m_name {name}
{}

Grid* Structured15DGridWithCrossSectionCrossSection::grid() const
{
	return m_grid;
}

int Structured15DGridWithCrossSectionCrossSection::loadFromCgnsFile(hid_t groupId, int index)
{
	std::vector<double> data;
	std::ostringstream ss;

	// read data
	ss << "Crosssection" << index;
	int ier = iRICLib::H5Util::readDataArrayValue(groupId, ss.str(), &data);
	if (ier != IRIC_NO_ERROR) {return ier;}

	auto count = data.size() / 2;
	for (unsigned int i = 0; i < count; ++i) {
		Altitude alt;
		alt.m_position = data[i];
		alt.m_height = data[i + count];
		m_altitudeInfo.push_back(alt);
	}

	return IRIC_NO_ERROR;
}

int Structured15DGridWithCrossSectionCrossSection::saveToCgnsFile(hid_t groupId, int index)
{
	std::ostringstream ss;
	ss << "Crosssection" << index;

	std::vector<hsize_t> dims(2);
	int size = m_altitudeInfo.size();
	dims[0] = 2;
	dims[1] = size;
	std::vector<double> data(size * 2);
	for (int i = 0; i < size; i++) {
		data[i] = m_altitudeInfo.at(i).m_position;
		data[size + i] = m_altitudeInfo.at(i).m_height;
	}
	return iRICLib::H5Util::createDataArray(groupId, ss.str(), data, dims);
}

const QString& Structured15DGridWithCrossSectionCrossSection::name() const
{
	return m_name;
}

const QVector<Structured15DGridWithCrossSectionCrossSection::Altitude>& Structured15DGridWithCrossSectionCrossSection::altitudeInfo() const
{
	return m_altitudeInfo;
}

QVector<Structured15DGridWithCrossSectionCrossSection::Altitude>& Structured15DGridWithCrossSectionCrossSection::altitudeInfo()
{
	return m_altitudeInfo;
}

void Structured15DGridWithCrossSectionCrossSection::setAltitudeInfo(const QVector<Altitude>& alt)
{
	m_altitudeInfo = alt;
}
