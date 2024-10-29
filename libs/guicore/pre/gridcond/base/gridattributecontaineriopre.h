#ifndef GRIDATTRIBUTECONTAINERIOPRE_H
#define GRIDATTRIBUTECONTAINERIOPRE_H

#include "../../../guicore_global.h"
#include "gridattributecontainerioi.h"

class GUICOREDLL_EXPORT GridAttributeContainerIoPre : public GridAttributeContainerIoI
{
public:
	GridAttributeContainerIoPre(const QString& tempDir);

	int loadFromCgnsFile(const iRICLib::H5CgnsGridAttributes& atts, GridAttributeContainer* cont) override;
	int saveToCgnsFile(iRICLib::H5CgnsGridAttributes* atts, GridAttributeContainer* cont) override;
	void clearTemporaryData(GridAttributeContainer* cont) override;
	void handleDimensionCurrentIndexChange(int oldIndex, int newIndex, GridAttributeContainer* cont) override;

private:
	QString tempExternalFilename(GridAttributeContainer* cont, int index) const;

	QString m_tempDir;
};

#endif // GRIDATTRIBUTECONTAINERIOPRE_H
