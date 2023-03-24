#ifndef GEODATAPOLYDATAGROUP_H
#define GEODATAPOLYDATAGROUP_H

#include "gd_polydatagroup_global.h"

#include <guicore/pre/geodata/geodata.h>

#include <unordered_set>
#include <vector>

namespace geos {
namespace index{
namespace quadtree {
class Quadtree;
} // quadtree
} // index
} // geos

class GeoDataPolyData;
class GeoDataPolyDataGroupCreator;
class GeoDataPolyDataGroupPolyData;

class vtkActor;

class GD_POLYDATAGROUP_EXPORT GeoDataPolyDataGroup : public GeoData
{
	Q_OBJECT

public:
	enum class Mode {
		Normal,
		EditingData
	};

	GeoDataPolyDataGroup(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition);
	~GeoDataPolyDataGroup();

	void addData(GeoDataPolyDataGroupPolyData* data);
	GeoDataPolyData* editTargetData() const;
	int editTargetDataIndex() const;
	bool isSelected(GeoDataPolyDataGroupPolyData* data);
	std::vector<GeoDataPolyDataGroupPolyData*> allData() const;
	void panTo(int row);

	void updateIndex();
	void updateOrder();
	void mergeEditTargetData(bool noUpdate = false);
	GeoDataPolyDataGroupCreator* creator() const;

	bool getValueRange(double* min, double* max) override;
	void applyColorMapSetting() override;
	void updateFilename() override;
	void addCustomMenuItems(QMenu* menu) override;
	bool addToolBarButtons(QToolBar *parent) override;
	void showInitialDialog() override;

	void viewOperationEnded(PreProcessorGraphicsViewInterface* v) override;
	void keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void keyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mouseDoubleClickEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;

	void informSelection(PreProcessorGraphicsViewInterface* v) override;
	void informDeselection(PreProcessorGraphicsViewInterface* v) override;

private slots:
	void addData();
	void editName();
	void editNameAndValue();
	void deleteSelectedData();
	void editColorSetting();
	void showAttributeBrowser();
	void mergePolyDataAndPolyDataGroups();
	void handleAttributeBrowserVisibilityChange(bool visible);

	void moveSelectedDataToTop();
	void moveSelectedDataToBottom();
	void moveSelectedDataUp();
	void moveSelectedDataDown();

	void copy();

protected slots:
	void updateAttributeBrowser(bool force = false);

protected:
	const std::vector<GeoDataPolyDataGroupPolyData*>& data() const;
	std::vector<GeoDataPolyDataGroupPolyData*>& data();

	const std::unordered_set<GeoDataPolyDataGroupPolyData*>& selectedData() const;
	std::unordered_set<GeoDataPolyDataGroupPolyData*>& selectedData();

	GeoDataPolyDataGroupPolyData* editTargetDataBackup() const;
	QMenu* rightClickingMenu() const;

	Mode mode() const;
	void setMode(Mode mode);

	geos::index::quadtree::Quadtree* qTree() const;
	const ZDepthRange& depthRange() const;
	virtual void updateActorSettingForEditTargetPolyData() = 0;

	void assignActorZValues(const ZDepthRange& range) override;

	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;

	QAction* addAction() const;
	QAction* editNameAction() const;
	QAction* editNameAndValueAction() const;
	QAction* deleteAction() const;
	QAction* mergeAction() const;
	QAction* editDisplaySettingAction() const;
	QAction* attributeBrowserAction() const;
	QAction* moveToTopAction() const;
	QAction* moveToBottomAction() const;
	QAction* moveUpAction() const;
	QAction* moveDownAction() const;
	QAction* copyAction() const;

private:
	void makeConnections();
	void setupNewEditTargetData();
	void setupEditTargetDataFromSelectedData();
	void mergePolyData(GeoDataPolyData* data, int position = -1);
	void mergePolyDataGroup(GeoDataPolyDataGroup* group, int position = -1);

	void doApplyOffset(double x, double y) override;

	virtual GeoDataPolyDataGroupPolyData* createNewData() = 0;
	virtual GeoDataPolyData* createEditTargetData() = 0;
	virtual void updateActorSetting() = 0;
	virtual void updateMenu() = 0;

	virtual void updateVtkObjects() = 0;
	virtual void updateSelectedDataVtkObjects() = 0;
	virtual void selectDataWithPoint(const QPointF& point) = 0;
	virtual void selectDataWithPolygon(const QPolygonF& polygon) = 0;
	virtual std::vector<vtkActor*> actorsToShowWhenSelected() = 0;
	virtual bool isMergablePolyData(GeoData* geoData) = 0;
	virtual bool isMergablePolyDataGroup(GeoData* geoData) = 0;
	virtual QString captionForData(int number) = 0;

	virtual GeoDataPolyDataGroup* createInstanceForCopy(PreProcessorGeoDataDataItemInterface* d) = 0;

	class EditNameAndValueCommand;
	class SortCommand;
	class SortEditTargetDataCommand;

	class Impl;
	Impl* impl;

public:
	friend class GeoDataPolyDataGroupAttributeBrowser;
};

#ifdef _DEBUG
	#include "private/geodatapolydatagroup_impl.h"
#endif

#endif // GEODATAPOLYDATAGROUP_H
