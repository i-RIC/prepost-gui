#include "crosssection.h"
#include "crosssectionpreprocessorcontroller.h"
#include "crosssectionpreprocessorview.h"

#include <QStandardItem>

CrossSection::CrossSection(DataItem* parent) :
	DataItem {parent},
	m_id {0},
	m_isDefined {false}
{}

CrossSection::~CrossSection()
{}

bool CrossSection::isDefined() const
{
	return m_isDefined;
}

int CrossSection::id() const
{
	return m_id;
}

void CrossSection::setId(int id)
{
	m_id = id;
}

QPointF CrossSection::point1() const
{
	return m_point1;
}

void CrossSection::setPoint1(const QPointF& p)
{
	m_isDefined = true;
	m_point1 = p;
}

QPointF CrossSection::point2() const
{
	return m_point2;
}

void CrossSection::setPoint2(const QPointF& p)
{
	m_isDefined = true;
	m_point2 = p;
}

void CrossSection::getNearestPoint(double x, double y, QPointF* nearestPoint, double* distance, double* pos) const
{

}

void CrossSection::reverseDirection()
{
	auto tmpP = m_point1;
	m_point1 = m_point2;
	m_point2 = tmpP;
}

QStandardItem* CrossSection::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem(tr("Cross Section %1").arg(id() + 1));
	setupStandardItem(item);
	return item;
}

DataItemController* CrossSection::buildPreProcessorDataItemController(Model* model)
{
	return new CrossSectionPreProcessorController(model, this);
}

DataItemView* CrossSection::buildPreProcessorDataItemView(Model* model)
{
	return new CrossSectionPreProcessorView(model, this);
}
