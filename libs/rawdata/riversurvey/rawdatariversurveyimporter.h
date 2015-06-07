#ifndef RAWDATARIVERSURVEYIMPORTER_H
#define RAWDATARIVERSURVEYIMPORTER_H

#include <guicore/pre/rawdata/rawdataimporter.h>
#include "rawdatariversurveyimportersettingdialog.h"

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

class RawDataRiverSurveyImporter : public RawDataImporter
{
	Q_OBJECT

private:

public:
	/// Constructor
	RawDataRiverSurveyImporter(RawDataCreator* creator);
	bool importData(RawData* data, int index, QWidget* w) override;
	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;

private:
	PRivPath RivAlloc(double KP, char* str);
	void RivFree(PRivPath node);
	void RivFreeAll();
	bool RivSetBank(PRivPath node, PPoint2D left, PPoint2D right);
	bool RivSetPath(PRivPath node, int np, PPoint2D pt);
	bool RivSort();
	bool RivRead(const QString& name, bool* with4points);

	PRivPath m_RivRoot;
	RawDataRiverSurveyImporterSettingDialog::CenterPointSetting m_cpSetting;
};

#endif // RAWDATARIVERSURVEYIMPORTER_H
