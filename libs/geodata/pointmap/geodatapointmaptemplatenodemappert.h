#ifndef GEODATAPOINTMAPTEMPLATENODEMAPPERT_H
#define GEODATAPOINTMAPTEMPLATENODEMAPPERT_H

#include <guicore/pre/geodata/geodatanodemappert.h>
#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <misc/doublemappingsetting.h>

#define VERYSMALL 1.0E-12

class GeoDataPointmapTemplateNodeMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataPointmapTemplateNodeMapperSetting() : GeoDataMapperSettingI() {}
	~GeoDataPointmapTemplateNodeMapperSetting() {}

	QList<DoubleMappingSetting> settings;
};

template <class V, class DA>
class GeoDataPointmapTemplateNodeMapperT : public GeoDataNodeMapperT<V, DA>
{
public:
	GeoDataPointmapTemplateNodeMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;

	void setAutoMode(bool a);
	void setStreamWiseLength(double l);
	void setCrossStreamLength(double l);
	void setNumExpansions(int num);
	void setWeightPowVal(double val);

private:
	void init();

	bool m_autoMode;
	double m_streamWiseLength;
	double m_crossStreamLegnth;
	int m_numExpansions;
	double m_weightPowVal;
};

#include "private/geodatapointmaptemplatenodemappert_detail.h"

#endif // GEODATAPOINTMAPTEMPLATENODEMAPPERT_H
