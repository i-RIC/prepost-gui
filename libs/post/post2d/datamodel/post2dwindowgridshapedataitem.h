#ifndef POST2DWINDOWGRIDSHAPEDATAITEM_H
#define POST2DWINDOWGRIDSHAPEDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <crosssection/abstractcrosssectionwindow.h>
#include <guibase/point/pointcontroller.h>
#include <guibase/polyline/polylinecontroller.h>
#include <guibase/gridshape/gridshapesettingcontainer.h>

#include <unordered_map>
#include <unordered_set>

class Post2dWindowGridDataItemI;
class PostCrosssectionWindow;

class Post2dWindowGridShapeDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	enum class Mode {
		Other,
		Structured2d,
		Unstructured2dEdge,
	};

	Post2dWindowGridShapeDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowGridShapeDataItem();

	void update();
	void handleStandardItemDoubleClicked() override;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void keyPressEvent(QKeyEvent* event, VTKGraphicsView* v) override;
	void mouseDoubleClickEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void updateZDepthRangeItemCount() override;

	void showPropertyDialog() override;
	QDialog* propertyDialog(QWidget* parent) override;

private slots:
	void openCrossSectionWindow();
	void openVerticalCrossSectionWindow();
	void startEdgeSelect();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void assignActorZValues(const ZDepthRange& range) override;

	void setupActors();
	void doUpdateActorSetting() override;
	PostCrosssectionWindow* createCrosssectionWindow(AbstractCrosssectionWindow::Mode mode) const;
	void openEdgeCrossSectionWindow();
	Post2dWindowGridDataItemI* gridDataItem() const;

	void enterEdgeSelectMode();
	void leaveEdgeSelectMode();
	void setupEdgeCounterParts();

	GridShapeSettingContainer m_setting;

	Mode m_mode;
	vtkIdType m_selectedpointId;
	PointController m_pointController;
	std::vector<vtkIdType> m_linePointIds;
	std::vector<QPointF> m_linePoints;
	PolyLineController m_lineController;
	QPoint m_dragStartPoint;
	vtkActor* m_pointsActor;
	vtkDataSetMapper* m_pointsMapper;

	QMenu* m_rightClickingMenu;
	QAction* m_openXsectionWindowAction;
	QAction* m_openVXsectionWindowAction;
	QAction* m_openEdgeXsectionWindowAction;

	std::unordered_multimap<vtkIdType, vtkIdType> m_edgeCounterParts;
	std::unordered_set<vtkIdType> m_nextNodes;

	class SettingEditWidget;
};

#endif // POST2DWINDOWGRIDSHAPEDATAITEM_H
