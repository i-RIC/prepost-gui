#include "griddisplaysetting.h"

GridDisplaySetting::GridDisplaySetting()
{
	m_shapeMode = ShapeOutline;
	m_showNodeValue = false;
	m_showCellValue = false;
}

GridDisplaySetting::ShapeMode GridDisplaySetting::shapeMode() const
{
	return m_shapeMode;
}

void GridDisplaySetting::setShapeLineStyle(const LineStyleInformation& ls)
{
	m_shapeLineStyle = ls;
}

const LineStyleInformation& GridDisplaySetting::shapeLineStyle() const
{
	return m_shapeLineStyle;
}

void GridDisplaySetting::setShapePointStyle(const PointStyleInformation& ps)
{
	m_shapePointStyle = ps;
}

const PointStyleInformation& GridDisplaySetting::shapePointStyle() const
{
	return m_shapePointStyle;
}

bool GridDisplaySetting::showNodeValue() const
{
	return m_showNodeValue;
}

const QString& GridDisplaySetting::shownNodeValueName() const
{
	return m_shownNodeValueName;
}

void GridDisplaySetting::setShownNodeValueName(const QString& n)
{
	m_shownNodeValueName = n;
}

bool GridDisplaySetting::showCellValue() const
{
	return m_showCellValue;
}

const QString& GridDisplaySetting::shownCellValueName() const
{
	return m_shownCellValueName;
}

void GridDisplaySetting::setShownCellValueName(const QString& n)
{
	m_shownCellValueName = n;
}
