#ifndef GRIDDISPLAYSETTING_H
#define GRIDDISPLAYSETTING_H

#include <guibase/pointstyleinformation.h>
#include <guibase/linestyleinformation.h>

class GridDisplaySetting
{
public:
	enum ShapeMode {
		ShapeOutline,
		ShapeNodes,
		ShapeLines,
	};
	/// Constructor
	GridDisplaySetting();
	ShapeMode shapeMode() const {return m_shapeMode;}
	void setShapeLineStyle(const LineStyleInformation& ls) {m_shapeLineStyle = ls;}
	const LineStyleInformation& shapeLineStyle() const {return m_shapeLineStyle;}
	void setShapePointStyle(const PointStyleInformation& ps) {m_shapePointStyle = ps;}
	const PointStyleInformation& shapePointStyle() const {return m_shapePointStyle;}
	bool showNodeValue() const {return m_showNodeValue;}
	const QString& shownNodeValueName() const {return m_shownNodeValueName;}
	void setShownNodeValueName(const QString& n) {m_shownNodeValueName = n;}
	bool showCellValue() const {return m_showCellValue;}
	const QString& shownCellValueName() const {return m_shownCellValueName;}
	void setShownCellValueName(const QString& n) {m_shownCellValueName = n;}

private:
	ShapeMode m_shapeMode;
	LineStyleInformation m_shapeLineStyle;
	PointStyleInformation m_shapePointStyle;
	bool m_showNodeValue;
	QString m_shownNodeValueName;
	bool m_showCellValue;
	QString m_shownCellValueName;
};

#endif // GRIDDISPLAYSETTING_H
