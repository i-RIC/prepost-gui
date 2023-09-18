#ifndef V4STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTION_H
#define V4STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTION_H

#include "../guicore_global.h"

#include <vector>

class v4Structured15dGridWithCrossSection;

class GUICOREDLL_EXPORT v4Structured15dGridWithCrossSectionCrossSection
{
public:
	struct Altitude {
		double position;
		double height;
	};

	v4Structured15dGridWithCrossSectionCrossSection(const QString& name, v4Structured15dGridWithCrossSection* grid);
	v4Structured15dGridWithCrossSection* grid() const;

	const QString& name();

	const std::vector<Altitude>& altitudeInfo() const;
	std::vector<Altitude>& altitudeInfo();
	void setAltitudeInfo(const std::vector<Altitude> alt);

private:
	QString m_name;
	std::vector<Altitude> m_altitudeInfo;
	v4Structured15dGridWithCrossSection* m_grid;
};

#endif // V4STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTION_H
