#include "../../../project/projectcgnsfile.h"
#include "structured15dgridwithcrosssectioncrosssection.h"

#include <misc/stringtool.h>

#include <cgnslib.h>

void Structured15DGridWithCrossSectionCrossSection::loadFromCgnsFile(int fn, int B, int Z, int index)
{
	// Goto "GridCrosssections" node.
	int ier;
	ier = cg_goto(fn, B, "Zone_t", Z, "GridCrosssections", 0, "end");
	if (ier != 0) {
		// the corresponding node does not exists.
		return;
	}

	// load data in the index-th array.
	char arrayName[ProjectCgnsFile::BUFFERLEN];
	DataType_t dt;
	int dataDimension;
	cgsize_t dimensionVector[3];
	ier = cg_array_info(index, arrayName, &dt, &dataDimension, dimensionVector);
	if (ier) { return; }
	int size = dimensionVector[0];
	if (dt == RealSingle) {
		float* data = new float[size * dimensionVector[1]];
		ier = cg_array_read(index, data);
		for (int i = 0; i < size; i++) {
//			double tmpDistance = *(data + i);
//			double tmpAltitude = *(data + i + size);
//			m_distance.push_back(tmpDistance);
//			m_altitude.push_back(tmpAltitude);
			Altitude alt;
			alt.m_position = *(data + i);
			alt.m_height = *(data + i + size);
			m_altitudeInfo.push_back(alt);
		}
		delete[] data;
	} else {
		double* data = new double[size * dimensionVector[1]];
		ier = cg_array_read(index, data);
		for (int i = 0; i < size; i++) {
//			m_distance.push_back(*(data + i));
//			m_altitude.push_back(*(data + i + size));
			Altitude alt;
			alt.m_position = *(data + i);
			alt.m_height = *(data + i + size);
			m_altitudeInfo.push_back(alt);
		}
		delete[] data;
	}
}

void Structured15DGridWithCrossSectionCrossSection::saveToCgnsFile(int fn, int B, int Z, int index)
{
	// Go to "GridCrosssections" node.
	cg_goto(fn, B, "Zone_t", Z, "GridCrosssections", 0, "end");

	QString name("Crosssection");
	name.append(QString::number(index));

	// Delete the array if it already exists.
	cg_delete_node(const_cast<char*>(iRIC::toStr(name).c_str()));
	// Create the array of index-th cross section data.
	cgsize_t dimensions[3];
//	int size = m_distance.size();
	int size = m_altitudeInfo.size();
	dimensions[0] = size;
	dimensions[1] = 2;
	dimensions[2] = 0;
	double* data = new double[size * 2];
	for (int i = 0; i < size; i++) {
//		data[i] = m_distance.at(i);
//		data[size + i] = m_altitude.at(i);
		data[i] = m_altitudeInfo.at(i).m_position;
		data[size + i] = m_altitudeInfo.at(i).m_height;
	}
	cg_array_write(iRIC::toStr(name).c_str(), RealDouble, 2, dimensions, data);
	delete[] data;
}
