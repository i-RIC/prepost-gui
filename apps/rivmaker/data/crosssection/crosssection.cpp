#include "crosssection.h"
#include "crosssectionpreprocessorcontroller.h"
#include "crosssectionpreprocessorview.h"

#include <QIcon>
#include <QStandardItem>

CrossSection::CrossSection(DataItem* parent) :
	DataItem {parent},
	m_id {0},
	m_isDefined {false}
{
	setDeletable(true);
}

CrossSection::~CrossSection()
{}

bool CrossSection::isDefined() const
{
	return m_isDefined;
}

QString CrossSection::name() const
{
	return tr("X%1").arg(id() + 1);
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

QPointF CrossSection::point(int index) const
{
	if (index % 2 == 0) {
		return m_point1;
	} else {
		return m_point2;
	}
}

void CrossSection::setPoint(int index, const QPointF& p)
{
	QPointF* point = nullptr;
	if (index % 2 == 0) {
		point = &m_point1;
	} else {
		point = &m_point2;
	}
	*point = p;
}

std::vector<QPointF> CrossSection::coordinates() const
{
	std::vector<QPointF> ret;

	ret.push_back(m_point1);
	ret.push_back(m_point2);

	return ret;
}

void CrossSection::setCoordinates(const std::vector<QPointF>& coords)
{
	m_point1 = coords.at(0);
	m_point2 = coords.at(1);
}

double CrossSection::waterElevation() const
{
	return m_waterElevation;
}

void CrossSection::setWaterElevation(double e)
{
	m_waterElevation = e;
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
	auto item = new QStandardItem(QIcon(":/images/iconCrossSection.png"), name());
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
