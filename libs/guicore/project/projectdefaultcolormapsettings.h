#ifndef PROJECTDEFAULTCOLORMAPSETTINGS_H
#define PROJECTDEFAULTCOLORMAPSETTINGS_H

#include "../guicore_global.h"

#include <unordered_map>

class ColorMapSettingContainerI;

class QDomNode;
class QXmlStreamWriter;

class GUICOREDLL_EXPORT ProjectDefaultColorMapSettings
{
public:
	ProjectDefaultColorMapSettings();
	~ProjectDefaultColorMapSettings();

	const std::unordered_map<std::string, ColorMapSettingContainerI*>& colorMaps() const;
	ColorMapSettingContainerI* colorMap(const std::string& name) const;

	void add(const std::string& name, ColorMapSettingContainerI* colorMap);

	void load(const QDomNode& node);
	void save(QXmlStreamWriter& writer);

private:
	std::unordered_map<std::string, ColorMapSettingContainerI*> m_colorMaps;
};

#endif // PROJECTDEFAULTCOLORMAPSETTINGS_H
