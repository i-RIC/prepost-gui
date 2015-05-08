#ifndef POSTEXPORTSETTING_H
#define POSTEXPORTSETTING_H

#include "../guicore_global.h"
#include <QString>

class GUICOREDLL_EXPORT PostExportSetting
{
public:
	/// Constructor
	PostExportSetting();
	/// Destructor
	~PostExportSetting();

	bool allSteps;
	int startStep;
	int endStep;
	int skipRate;

	bool fullRange;
	int iMin;
	int iMax;
	int jMin;
	int jMax;
	int kMin;
	int kMax;

	QString folder;
	QString filename;
	QString prefix;
};

#endif // POSTEXPORTSETTING_H
