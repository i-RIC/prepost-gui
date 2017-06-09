#include "crosssection.h"
#include "crosssectionpreprocessorcontroller.h"
#include "crosssectionpreprocessorview.h"
#include "../project/project.h"
#include "../../misc/geometryutil.h"

#include "private/crosssection_impl.h"

#include <misc/xmlsupport.h>

#include <QDomElement>
#include <QFile>
#include <QIcon>
#include <QStandardItem>
#include <QVector2D>
#include <QXmlStreamWriter>

CrossSection::Impl::Impl(CrossSection* cs) :
	m_isDefined {false},
	m_waterElevationIsSet {false},
	m_waterElevation {0},
	m_parent {cs}
{}

// public interfaces

CrossSection::CrossSection(DataItem* parent) :
	DataItem {parent},
	impl {new Impl {this}}
{
	setDeletable(true);
}

CrossSection::~CrossSection()
{
	delete impl;
}

bool CrossSection::isDefined() const
{
	return impl->m_isDefined;
}

int CrossSection::id() const
{
	return impl->m_id;
}

void CrossSection::setId(int id)
{
	impl->m_id = id;
}

QString CrossSection::name() const
{
	return impl->m_name;
}

void CrossSection::setName(const QString& name)
{
	impl->m_name = name;
}

QPointF CrossSection::point1() const
{
	return impl->m_point1;
}

void CrossSection::setPoint1(const QPointF& p)
{
	impl->m_isDefined = true;
	impl->m_point1 = p;
}

QPointF CrossSection::point2() const
{
	return impl->m_point2;
}

void CrossSection::setPoint2(const QPointF& p)
{
	impl->m_isDefined = true;
	impl->m_point2 = p;
}

QPointF CrossSection::point(int index) const
{
	if (index % 2 == 0) {
		return impl->m_point1;
	} else {
		return impl->m_point2;
	}
}

void CrossSection::setPoint(int index, const QPointF& p)
{
	QPointF* point = nullptr;
	if (index % 2 == 0) {
		point = &(impl->m_point1);
	} else {
		point = &(impl->m_point2);
	}
	*point = p;
	project()->setModified();
}

std::vector<QPointF> CrossSection::coordinates() const
{
	std::vector<QPointF> ret;

	ret.push_back(impl->m_point1);
	ret.push_back(impl->m_point2);

	return ret;
}

void CrossSection::setCoordinates(const std::vector<QPointF>& coords)
{
	impl->m_point1 = coords.at(0);
	impl->m_point2 = coords.at(1);
	project()->setModified();
}

bool CrossSection::waterElevationIsSet() const
{
	return impl->m_waterElevationIsSet;
}

void CrossSection::setWaterElevationIsSet(bool isSet)
{
	impl->m_waterElevationIsSet = isSet;
	project()->setModified();
}

double CrossSection::waterElevation() const
{
	return impl->m_waterElevation;
}

void CrossSection::setWaterElevation(double e)
{
	impl->m_waterElevation = e;
	project()->setModified();
}

std::vector<QVector2D> CrossSection::mappedPoints() const
{
	std::vector<QVector2D> ret;
	for (auto pair : impl->m_mappedPoints) {
		ret.push_back(QVector2D(pair.first, pair.second));
	}
	return ret;
}

void CrossSection::setMappedPoints(const std::vector<QVector2D>& points)
{
	clearMappedPoints();
	for (auto p : points) {
		addMappedPoint(p);
	}
}

void CrossSection::addMappedPoint(const QVector2D& p)
{
	impl->m_mappedPoints.insert(std::make_pair(p.x(), p.y()));
}

void CrossSection::clearMappedPoints()
{
	impl->m_mappedPoints.clear();
}

void CrossSection::getNearestPoint(double x, double y, QPointF* nearestPoint, double* distance) const
{
	*nearestPoint = GeometryUtil::nearestPoint(impl->m_point1, impl->m_point2, QPointF(x, y));
	*distance = QVector2D(x - nearestPoint->x(), y - nearestPoint->y()).length();
}

void CrossSection::getMappedPoint(double x, double y, QPointF* mappedPoint, double* position) const
{
	*mappedPoint = GeometryUtil::mappedPoint(impl->m_point1, impl->m_point2, QPointF(x, y));
	QVector2D v1(x - impl->m_point1.x(), y - impl->m_point1.y());
	QVector2D v2(impl->m_point2.x() - impl->m_point1.x(), impl->m_point2.y() - impl->m_point1.y());
	v2.normalize();
	*position = QVector2D::dotProduct(v1, v2);
}

void CrossSection::reverseDirection()
{
	auto tmpP = impl->m_point1;
	impl->m_point1 = impl->m_point2;
	impl->m_point2 = tmpP;
	project()->setModified();
}

QStandardItem* CrossSection::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem(QIcon(":/images/iconCrossSection.png"), name());
	setupStandardItem(item);
	item->setEditable(true);
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

void CrossSection::doLoadFromMainFile(const QDomElement& node)
{
	impl->m_name = node.attribute("name");
	impl->m_isDefined = iRIC::getBooleanAttribute(node, "isDefined", false);
	impl->m_waterElevationIsSet = iRIC::getBooleanAttribute(node, "isWaterElevationSet", false);
}

void CrossSection::doSaveToMainFile(QXmlStreamWriter* writer) const
{
	writer->writeAttribute("name", impl->m_name);
	iRIC::setBooleanAttribute(*writer, "isDefined", impl->m_isDefined);
	iRIC::setBooleanAttribute(*writer, "isWaterElevationSet", impl->m_waterElevationIsSet);
}

void CrossSection::loadExternalData(const QString& filename)
{
	QFile file(filename);
	bool ok = file.open(QIODevice::ReadOnly);
	if (! ok) {return;}

	QDataStream in(&file);
	in.setVersion(QDATASTREAM_VERSION);

	in >> impl->m_point1 >> impl->m_point2;
	in >> impl->m_waterElevation;

	file.close();
}

void CrossSection::saveExternalData(const QString& filename) const
{
	QFile file(filename);
	bool ok = file.open(QIODevice::WriteOnly);
	if (! ok) {return;}
	QDataStream out(&file);
	out.setVersion(QDATASTREAM_VERSION);

	out << impl->m_point1 << impl->m_point2;
	out << impl->m_waterElevation;

	file.close();
}

QString CrossSection::relativeFilename() const
{
	return QString("crosssection%1.dat").arg(impl->m_id);
}
