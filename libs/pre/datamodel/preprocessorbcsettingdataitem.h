#ifndef PREPROCESSORBCSETTINGDATAITEM_H
#define PREPROCESSORBCSETTINGDATAITEM_H

#include <guicore/pre/base/preprocessordataitem.h>
#include <geodata/polygon/geodatapolygon.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>

#include <QString>
#include <QColor>
#include <QSet>
#include <QPoint>

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkActor2D.h>

class BoundaryConditionDialog;
class SolverDefinition;
class SolverDefinitionBoundaryCondition;
class PreProcessorBCDataItem;
class QAction;
class VTKGraphicsView;
class Grid;
class WaitDialog;

class PreProcessorBCSettingDataItem : public PreProcessorGeoDataDataItemInterface
{
	Q_OBJECT

public:
	PreProcessorBCSettingDataItem(PreProcessorBCDataItem* item, GraphicsWindowDataItem* parent);
	~PreProcessorBCSettingDataItem() override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void handleStandardItemDoubleClicked() override;
	void handleStandardItemChange() override;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void viewOperationEnded(VTKGraphicsView* v) override;
	void mouseDoubleClickEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void keyPressEvent(QKeyEvent*, VTKGraphicsView*) override;
	void keyReleaseEvent(QKeyEvent*, VTKGraphicsView*) override;
	void loadData();
	void addCustomMenuItems(QMenu* menu) override;
	PreProcessorBCDataItem* bcDataItem() const;
	bool isMapped() const;
	ColorMapSettingContainerI* colorMapSettingContainer() const override;

	GeoData* geoData() const override;
	void setGeoData(GeoData *) override;

	void informValueRangeChange() override;
	void informDataChange() override;
	void setDeleteSilently(bool) override;

	QAction* editAction() const;
	QAction* deleteAction();

public slots:
	void executeMapping(bool noDraw, WaitDialog* dialog);

private slots:
	void setModified(bool modified = true) override;
	void updateItem();

protected:
	void assignActorZValues(const ZDepthRange& range) override;
	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;
	void doApplyOffset(double x, double y) override;

private:
	void setupActors();
	void updateFilename();

	PreProcessorBCDataItem* m_bcDataItem;
	QAction* m_editAction;
	GeoDataPolygon* m_polygon;
	QPoint m_dragStartPoint;
	QMenu* m_rightClickingMenu;

	BoundaryConditionDialog* m_dialog;
};

#endif // PREPROCESSORBCSETTINGDATAITEM_H
