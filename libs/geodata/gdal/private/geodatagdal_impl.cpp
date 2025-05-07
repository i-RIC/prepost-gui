#include "geodatagdal_impl.h"

GeoDataGdal::Impl::Impl() :
	m_geoTransformExists {false},
	m_baseAndResolutionExists {false},
	m_base {0},
	m_resolution {0}
{}

GeoDataGdal::Impl::~Impl()
{}
