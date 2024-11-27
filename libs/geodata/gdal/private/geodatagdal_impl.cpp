#include "geodatanetcdf_impl.h"

GeoDataNetcdf::Impl::Impl() :
	m_geoTransformExists {false},
	m_baseAndResolutionExists {false},
	m_base {0},
	m_resolution {0}
{}

GeoDataNetcdf::Impl::~Impl()
{}
