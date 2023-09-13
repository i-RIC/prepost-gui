#ifndef WINDOWSIZESTANDARDSETTING_H
#define WINDOWSIZESTANDARDSETTING_H

#include "misc_global.h"

#include <QString>

class MISCDLL_EXPORT WindowSizeStandardSetting
{
public:
	WindowSizeStandardSetting();
	WindowSizeStandardSetting(int w, int h, const QString& c = "");

	QString toString() const;
	void fromString(const QString setting);

	QString labelString() const;
	QSize size() const;

	bool operator<(const WindowSizeStandardSetting& s) const;
	bool operator==(const WindowSizeStandardSetting& s) const;

	static std::vector<WindowSizeStandardSetting> buildDefaultSettings();

	int width;
	int height;
	QString comment;
};

#endif // WINDOWSIZESTANDARDSETTING_H
