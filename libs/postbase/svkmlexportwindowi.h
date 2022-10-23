#ifndef SVKMLEXPORTWINDOWI_H
#define SVKMLEXPORTWINDOWI_H

#include <string>
#include <vector>

class QXmlStreamWriter;

/// Window that support KML for street view.
class SVKmlExportWindowI
{
public:
	virtual ~SVKmlExportWindowI() {}

	virtual std::vector<std::string> discreteColorDrawingZones() = 0;
	virtual bool checkKmlExportCondition(const std::string &zonename) const = 0;
	virtual bool exportKMLHeader(QXmlStreamWriter& writer, const std::string& zonename) = 0;
	virtual bool exportKMLFooter(QXmlStreamWriter& writer, const std::string& zonename) = 0;
	virtual bool exportKMLForTimestep(QXmlStreamWriter& writer, int index, double time, const std::string& zonename, bool oneShot) = 0;
};

#endif // SVKMLEXPORTWINDOWI_H
