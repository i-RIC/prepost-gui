#include "watersurfaceelevationpoints.h"
#include "private/watersurfaceelevationpoints_impl.h"
#include "../project/project.h"
#include "../base/dataitemview.h"

#include <QStandardItem>

WaterSurfaceElevationPoints::Impl::Impl(WaterSurfaceElevationPoints *parent) :
	m_arbitraryHWM {parent},
	m_leftBankHWM {parent},
	m_rightBankHWM {parent}
{}

WaterSurfaceElevationPoints::WaterSurfaceElevationPoints(DataItem *parent) :
	DataItem {parent},
	impl {new Impl {this}}
{}

WaterSurfaceElevationPoints::~WaterSurfaceElevationPoints()
{
	delete impl;
}

const LeftBankHWM& WaterSurfaceElevationPoints::leftBankHWM() const
{
	return impl->m_leftBankHWM;
}

LeftBankHWM& WaterSurfaceElevationPoints::leftBankHWM()
{
	return impl->m_leftBankHWM;
}

const RightBankHWM& WaterSurfaceElevationPoints::rightBankHWM() const
{
	return impl->m_rightBankHWM;
}

RightBankHWM& WaterSurfaceElevationPoints::rightBankHWM()
{
	return impl->m_rightBankHWM;
}

const ArbitraryHWM& WaterSurfaceElevationPoints::arbitraryHWM() const
{
	return impl->m_arbitraryHWM;
}

ArbitraryHWM& WaterSurfaceElevationPoints::arbitraryHWM()
{
	return impl->m_arbitraryHWM;
}

QStandardItem* WaterSurfaceElevationPoints::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem(QIcon(":/images/iconFolder.png"), tr("Water Elevation Points"));
	setupStandardItem(item);
	return item;
}

DataItemView* WaterSurfaceElevationPoints::buildPreProcessorDataItemView(Model* model)
{
	return new DataItemView(model, this);
}
