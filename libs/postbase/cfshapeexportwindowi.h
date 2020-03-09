#ifndef CFSHAPEEXPORTWINDOWI_H
#define CFSHAPEEXPORTWINDOWI_H

#include <QList>
#include <QString>

class QString;

/// Window that can export Contour Figure to ESRI Shapefiles
class CfShapeExportWindowI
{
public:
	virtual ~CfShapeExportWindowI() {}
	/**
	 * @brief Export contour figure to ESRI Shapefiles
	 * @param filePrefix Prefix of the exported files. For example "C:/Result_". The exported files will be "C:/Result_1.shp", "C:/Result_1.shx", "C:/Result_1.dbf", "C:/Result_1.prj". etc.
	 * @param index The index of the result. i. e. 1, 2, 3, ... for each time steps
	 * @param time The value of time.
	 * @param zonename The name of zone on which contour figure is drawn.
	 * @return
	 */
	virtual bool exportContourFigureToShape(const QString& filePrefix, int index, double time, const QString& zonename) = 0;
	/// Returns the list of zones on which contour figure is drawn on the window.
	virtual QList<QString> contourFigureDrawingZones() = 0;
};

#endif // CFSHAPEEXPORTWINDOWI_H
