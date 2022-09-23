#include "geodataproxydataitem.h"

#include <QIcon>

GeoDataProxyDataItem::GeoDataProxyDataItem(GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem("", QIcon(":/libs/guibase/images/iconPaper.svg"), parent)
{}

GeoDataProxyDataItem::~GeoDataProxyDataItem()
{}
