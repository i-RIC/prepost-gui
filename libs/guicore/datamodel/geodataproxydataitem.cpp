#include "geodataproxydataitem.h"

#include <QIcon>

GeoDataProxyDataItem::GeoDataProxyDataItem(GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem("", QIcon(":/libs/guibase/images/iconPaper.png"), parent)
{}

GeoDataProxyDataItem::~GeoDataProxyDataItem()
{}
