#include <vector>
#include "iriclib_polygon.h"
#include "iriclib_riversurvey.h"

static std::vector<iRICLib::Polygon*> polygons;
static std::vector<iRICLib::RiverSurvey*> riversurveys;

extern "C"{

int iRIC_Geo_Polygon_Open(char* filename, int *id){
	iRICLib::Polygon* polygon = new iRICLib::Polygon();
	// currently, iriclib fortran functions supports only data with no dimension.
	int ret = polygon->load(filename, true);
	if (ret != 0){
		delete polygon;
		return -1;
	}
	polygons.push_back(polygon);
	*id = static_cast<int>(polygons.size());
	return 0;
}

static iRICLib::Polygon* iRIC_local_Geo_Polygon_GetPolygon(int id)
{
	if ((unsigned int)(id) > polygons.size()){return 0;}
	if (id < 1){return 0;}
	return polygons[id - 1];
}

int iRIC_Geo_Polygon_Read_IntegerValue(int id, int* value)
{
	iRICLib::Polygon* pol = iRIC_local_Geo_Polygon_GetPolygon(id);
	if (pol == 0) {return -1;}
	int intVal = static_cast<int>(pol->values[0]);
	*value = intVal;
	return 0;
}

int iRIC_Geo_Polygon_Read_RealValue(int id, double* value)
{
	iRICLib::Polygon* pol = iRIC_local_Geo_Polygon_GetPolygon(id);
	if (pol == 0) {return -1;}
	*value = pol->values[0];
	return 0;
}

int iRIC_Geo_Polygon_Read_PointCount(int id, int* size)
{
	iRICLib::Polygon* pol = iRIC_local_Geo_Polygon_GetPolygon(id);
	if (pol == 0) {return -1;}
	*size = pol->polygon->pointCount;
	return 0;
}

int iRIC_Geo_Polygon_Read_Points(int id, double* x, double* y)
{
	iRICLib::Polygon* pol = iRIC_local_Geo_Polygon_GetPolygon(id);
	if (pol == 0) {return -1;}
	iRICLib::InternalPolygon* ipol = pol->polygon;
	for (int i = 0; i < ipol->pointCount; ++i){
		*(x + i) = *(ipol->x + i);
		*(y + i) = *(ipol->y + i);
	}
	return 0;
}

int iRIC_Geo_Polygon_Read_HoleCount(int id, int* count)
{
	iRICLib::Polygon* pol = iRIC_local_Geo_Polygon_GetPolygon(id);
	if (pol == 0) {return -1;}
	*count = static_cast<int>(pol->holes.size());
	return 0;
}

int iRIC_Geo_Polygon_Read_HolePointCount(int id, int holeid, int* count)
{
	iRICLib::Polygon* pol = iRIC_local_Geo_Polygon_GetPolygon(id);
	if (pol == 0){return -1;}
	if ((unsigned int)(holeid) > pol->holes.size()){return -1;}
	iRICLib::InternalPolygon* ipol = pol->holes[holeid - 1];
	*count = ipol->pointCount;
	return 0;
}

int iRIC_Geo_Polygon_Read_HolePoints(int id, int holeid, double* x, double* y)
{
	iRICLib::Polygon* pol = iRIC_local_Geo_Polygon_GetPolygon(id);
	if (pol == 0) {return -1;}
	if ((unsigned int)(holeid) > pol->holes.size()){return -1;}
	iRICLib::InternalPolygon* ipol = pol->holes[holeid - 1];
	for (int i = 0; i < ipol->pointCount; ++i){
		*(x + i) = *(ipol->x + i);
		*(y + i) = *(ipol->y + i);
	}
	return 0;
}

int iRIC_Geo_Polygon_Close(int id)
{
	if (polygons[id - 1] == 0){return -1;}
	delete polygons[id - 1];
	polygons[id - 1] = 0;
	return 0;
}

int iRIC_Geo_RiverSurvey_Open(char* filename, int* id)
{
	iRICLib::RiverSurvey* rs = new iRICLib::RiverSurvey();
	int ret = rs->load(filename);
	if (ret != 0){
		delete rs;
		return -1;
	}
	riversurveys.push_back(rs);
	*id = static_cast<int>(riversurveys.size());
	return 0;
}

static iRICLib::RiverSurvey* iRIC_local_Geo_RiverSurvey_GetRiverSurvey(int id)
{
	if ((unsigned int)(id) > riversurveys.size()){return 0;}
	if (id < 1){return 0;}
	return riversurveys[id - 1];
}

static iRICLib::RiverPathPoint* iRIC_local_Geo_RiverSurvey_GetRiverPathPoint(int id, int pointid)
{
	iRICLib::RiverSurvey* rs = iRIC_local_Geo_RiverSurvey_GetRiverSurvey(id);
	if (rs == 0){return 0;}
	if ((unsigned int)(pointid) > rs->points.size()){return 0;}
	if (pointid < 1){return 0;}
	return rs->points[pointid - 1];
}

int iRIC_Geo_RiverSurvey_Read_Count(int id, int* count)
{
	iRICLib::RiverSurvey* rs = iRIC_local_Geo_RiverSurvey_GetRiverSurvey(id);
	if (rs == 0) {return -1;}
	*count = static_cast<int>(rs->points.size());
	return 0;
}

int iRIC_Geo_RiverSurvey_Read_Position(int id, int pointid, double* x, double* y)
{
	iRICLib::RiverPathPoint* rpp = iRIC_local_Geo_RiverSurvey_GetRiverPathPoint(id, pointid);
	if (rpp == 0){return -1;}
	*x = rpp->positionX;
	*y = rpp->positionY;
	return 0;
}

int iRIC_Geo_RiverSurvey_Read_Direction(int id, int pointid, double* vx, double* vy)
{
	iRICLib::RiverPathPoint* rpp = iRIC_local_Geo_RiverSurvey_GetRiverPathPoint(id, pointid);
	if (rpp == 0){return -1;}
	*vx = rpp->directionX;
	*vy = rpp->directionY;
	return 0;
}

int iRIC_Geo_RiverSurvey_Read_Name(int id, int pointid, const char** name)
{
	iRICLib::RiverPathPoint* rpp = iRIC_local_Geo_RiverSurvey_GetRiverPathPoint(id, pointid);
	if (rpp == 0){return -1;}
	*name = rpp->name.c_str();
	return 0;
}

int iRIC_Geo_RiverSurvey_Read_RealName(int id, int pointid, double* name)
{
	iRICLib::RiverPathPoint* rpp = iRIC_local_Geo_RiverSurvey_GetRiverPathPoint(id, pointid);
	if (rpp == 0){return -1;}
	*name = rpp->nameReal;
	return 0;
}

int iRIC_Geo_RiverSurvey_Read_LeftShift(int id, int pointid, double* shift)
{
	iRICLib::RiverPathPoint* rpp = iRIC_local_Geo_RiverSurvey_GetRiverPathPoint(id, pointid);
	if (rpp == 0){return -1;}
	*shift = rpp->leftShift;
	return 0;
}

int iRIC_Geo_RiverSurvey_Read_AltitudeCount(int id, int pointid, int* count)
{
	iRICLib::RiverPathPoint* rpp = iRIC_local_Geo_RiverSurvey_GetRiverPathPoint(id, pointid);
	if (rpp == 0){return -1;}
	*count = static_cast<int>(rpp->altitudes.size());
	return 0;
}

int iRIC_Geo_RiverSurvey_Read_Altitudes(int id, int pointid, double* position, double* height, int* active)
{
	iRICLib::RiverPathPoint* rpp = iRIC_local_Geo_RiverSurvey_GetRiverPathPoint(id, pointid);
	if (rpp == 0){return -1;}
	for (unsigned int i = 0; i < rpp->altitudes.size(); ++i){
			iRICLib::Altitude alt = rpp->altitudes[i];
			*(position + i) = alt.position;
			*(height + i) = alt.height;
			*(active + i) = alt.active;
	}
	return 0;
}

int iRIC_Geo_RiverSurvey_Read_FixedPointL(int id, int pointid, int* set, double* directionX, double* directionY, int* index){
	iRICLib::RiverPathPoint* rpp = iRIC_local_Geo_RiverSurvey_GetRiverPathPoint(id, pointid);
	if (rpp == 0){return -1;}
	*set = rpp->fixedPointLSet;
	*directionX = rpp->directionLX;
	*directionY = rpp->directionLY;
	*index = rpp->fixedPointLIndex;
	return 0;
}

int iRIC_Geo_RiverSurvey_Read_FixedPointR(int id, int pointid, int* set, double* directionX, double* directionY, int* index)
{
	iRICLib::RiverPathPoint* rpp = iRIC_local_Geo_RiverSurvey_GetRiverPathPoint(id, pointid);
	if (rpp == 0){return -1;}
	*set = rpp->fixedPointRSet;
	*directionX = rpp->directionRX;
	*directionY = rpp->directionRY;
	*index = rpp->fixedPointRIndex;
	return 0;
}

/*
int iRIC_Geo_RiverSurvey_Read_GridSkip(int id, int pointid, int *gridSkip)
{
		iRICLib::RiverPathPoint* rpp = iRIC_local_Geo_RiverSurvey_GetRiverPathPoint(id, pointid);
		if (rpp == 0){return -1;}
		*gridSkip = rpp->gridSkip;
		return 0;
}
*/

int iRIC_Geo_RiverSurvey_Read_WaterSurfaceElevation(int id, int pointid, int *set, double* value)
{
	iRICLib::RiverPathPoint* rpp = iRIC_local_Geo_RiverSurvey_GetRiverPathPoint(id, pointid);
	if (rpp == 0){return -1;}
	*set = rpp->wseSpecified;
	*value = rpp->waterSurfaceElevation;
	return 0;
}

int iRIC_Geo_RiverSurvey_Close(int id)
{
	if (riversurveys[id - 1] == 0){return -1;}
	delete riversurveys[id - 1];
	riversurveys[id - 1] = 0;
	return 0;
}

}
