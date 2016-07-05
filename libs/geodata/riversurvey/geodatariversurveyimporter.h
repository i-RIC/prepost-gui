#ifndef GEODATARIVERSURVEYIMPORTER_H
#define GEODATARIVERSURVEYIMPORTER_H

#include <guicore/pre/geodata/geodataimporter.h>
#include "geodatariversurveyimportersettingdialog.h"

typedef	struct	_Point2D Point2D, *PPoint2D;
typedef struct	_RivPath RivPath, *PRivPath;
struct _Point2D {
	double x, y;
};
struct _RivPath {
	std::string strKP;  // distance from sea (string)
	double KP;          // distance from sea (real number)
	Point2D*		bank;    // pointer to bank coordinates
	int np;             // number of latitude data
	Point2D*		pt;      // x=distance from left bank,y=depth
	PRivPath next;      // next river path point
	int divIndices[4];
};

class GeoDataRiverSurveyImporter : public GeoDataImporter
{
	Q_OBJECT

private:

public:
	GeoDataRiverSurveyImporter(GeoDataCreator* creator);

	bool importData(GeoData* data, int index, QWidget* w) override;
	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;

private:
	PRivPath RivAlloc(double KP, const char* str);
	void RivFree(PRivPath node);
	void RivFreeAll();
	bool RivSetBank(PRivPath node, PPoint2D left, PPoint2D right);
	bool RivSetPath(PRivPath node, int np, PPoint2D pt);
	bool RivSort();
	bool RivRead(const QString& name, bool* with4points);

	PRivPath m_RivRoot;
	GeoDataRiverSurveyImporterSettingDialog::CenterPointSetting m_cpSetting;
};

#endif // GEODATARIVERSURVEYIMPORTER_H
