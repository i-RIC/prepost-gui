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
	ShapeMode shapeMode() const;
	void setShapeLineStyle(const LineStyleInformation& ls);
	const LineStyleInformation& shapeLineStyle() const;
	void setShapePointStyle(const PointStyleInformation& ps);
	const PointStyleInformation& shapePointStyle() const;
	bool showNodeValue() const;
	const QString& shownNodeValueName() const;
	void setShownNodeValueName(const QString& n);
	bool showCellValue() const;
	const QString& shownCellValueName() const;
	void setShownCellValueName(const QString& n);

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
