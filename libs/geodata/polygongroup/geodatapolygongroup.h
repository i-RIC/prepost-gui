#ifndef GEODATAPOLYGONGROUP_H
#define GEODATAPOLYGONGROUP_H

#include "gd_polygongroup_global.h"
#include "geodatapolygongroupcolorsettingdialog.h"

#include <guicore/pre/geodata/geodata.h>

#include <vector>

class GeoDataPolygon;
class GeoDataPolygonGroupPolygon;

class GD_POLYGONGROUP_EXPORT GeoDataPolygonGroup : public GeoData
{
	Q_OBJECT

public:
	GeoDataPolygonGroup(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition);
	~GeoDataPolygonGroup();

	void addPolygon(GeoDataPolygonGroupPolygon* polygon);
	std::vector<GeoDataPolygonGroupPolygon*> polygonsInBoundingBox(double xmin, double xmax, double ymin, double ymax) const;
	std::vector<GeoDataPolygonGroupPolygon*> allPolygons() const;
	GeoDataPolygon* editTargetPolygon() const;
	int editTargetPolygonIndex() const;
	bool isSelected(GeoDataPolygonGroupPolygon* polygon);
	void updateOrder();

	void panTo(int row);

	void updateVtkObjects();
	void updateIndex();
	void mergeEditTargetPolygon(bool noUpdate = false);

	void setupMenu() override;
	bool addToolBarButtons(QToolBar* parent) override;
	void showInitialDialog() override;

	void informSelection(PreProcessorGraphicsViewInterface* v) override;
	void informDeselection(PreProcessorGraphicsViewInterface* v) override;

	void addCustomMenuItems(QMenu* menu) override;
	void viewOperationEnded(PreProcessorGraphicsViewInterface* v) override;

	void keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void keyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mouseDoubleClickEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void updateZDepthRangeItemCount(ZDepthRange& range) override;
	void assignActorZValues(const ZDepthRange& range) override;

	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* d) override;

	bool getValueRange(double* min, double* max) override;
	void updateFilename() override;

private slots:
	void addPolygon();
	void editName();
	void editNameAndValue();
	void mergePolygonsAndPolygonGroups();
	void moveSelectedPolygonsToTop();
	void moveSelectedPolygonsToBottom();
	void moveSelectedPolygonsUp();
	void moveSelectedPolygonsDown();
	void deleteSelectedPolygons();
	void editColorSetting();
	void showAttributeBrowser();
	void updateAttributeBrowser();
	void handleAttributeBrowserVisibilityChange(bool visible);

private:
	GeoDataPolygonGroupColorSettingDialog::Setting colorSetting() const;
	void setColorSetting(const GeoDataPolygonGroupColorSettingDialog::Setting& setting);
	void setSelectedPolygon(int index);
	void updateMenu();

	void makeConnections();

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;
	void doApplyOffset(double x, double y) override;

	class EditPropertyCommand;
	class SortCommand;
	class SortEditTargetPolygonCommand;

	class Impl;
	Impl* impl;

public:
	friend class GeoDataPolygonGroupAttributeBrowser;
	friend class GeoDataPolygonGroupShpImporter;
};

#endif // GEODATAPOLYGONGROUP_H
