#ifndef GEODATAPOINTMAPTEMPLATENODEMAPPER_H
#define GEODATAPOINTMAPTEMPLATENODEMAPPER_H

#include <guicore/pre/geodata/geodatanodemappert.h>
#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <misc/doublemappingsetting.h>

#include <vector>

class GeoDataPointmapTemplateNodeMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataPointmapTemplateNodeMapperSetting() : GeoDataMapperSettingI() {}
	~GeoDataPointmapTemplateNodeMapperSetting() {}

	std::vector<DoubleMappingSetting> settings;
};

class GeoDataPointmapTemplateNodeMapper : public GeoDataNodeMapperT<double, vtkDoubleArray>
{
public:
	GeoDataPointmapTemplateNodeMapper(GeoDataCreator* parent);

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

#endif // GEODATAPOINTMAPTEMPLATENODEMAPPER_H
