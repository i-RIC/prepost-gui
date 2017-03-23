#ifndef PREPROCESSORMODEL_H
#define PREPROCESSORMODEL_H

#include "../../data/base/model.h"

#include <vector>

class CrossSection;
class Project;
class PreProcessorView;

class QStandardItemModel;

class PreProcessorModel : public Model
{
	Q_OBJECT

public:
	PreProcessorModel(QObject* parent);
	~PreProcessorModel();

	void setProject(Project* project);
	void setCrossSections(const std::vector<CrossSection*>& crossSections);

	void importElevation();
	void importWaterSurfaceElevation();
	void importBaseLine();
	void exportBaseLine();

	void importCrossSections();
	void exportCrossSections();

	void addCrossSection();
	void deleteCrossSection();
	void editCrossSectionCoordinates();

	void addBaseLineVertex();
	void removeBaseLineVertex();
	void editBaseLineCoordinates();
	void reverseBaseLineDirection();
	void deleteBaseLine();

	void setupStandardItemAndViewAndController(PreProcessorDataItemI *newItem, DataItem* parent);

	void updateCrossSections();

private slots:
	void deleteSelectedItem() override;

private:
	void setupStandatdItemModel();
	RootDataItem* rootDataItem() const override;

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/preprocessormodel_impl.h"
#endif // _DEBUG

#endif // PREPROCESSORMODEL_H
