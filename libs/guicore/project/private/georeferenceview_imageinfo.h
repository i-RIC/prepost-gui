#ifndef GEOREFERENCEVIEW_IMAGEINFO_H
#define GEOREFERENCEVIEW_IMAGEINFO_H

#include "../georeferenceview.h"
#include "../util/selectionhelper.h"

class BackgroundImageInfo;
class AddibleGcpTableModel;

// CONTROLLER
#include <QMenu>
#include <unordered_set>

// VIEW
class QPainter;
class QRectF;
class QTransform;
class GeoreferenceViewHelper;

class GeoreferenceView::ImageInfo : public QObject
{
	Q_OBJECT

// ITEM
public:
	ImageInfo(BackgroundImageInfo* info, AddibleGcpTableModel* gcpTableModel, GeoreferenceView* view);
	virtual ~ImageInfo();

	GeoreferenceView* modelView() const;
	BackgroundImageInfo* backgroundImageInfo() const;
	void showRightClickMenu(const QPoint& pos);

	AddibleGcpTableModel* gcpTableModel();
	std::vector<GcpTableRow>* gcpTable();

private:
	GeoreferenceView* m_view;

	BackgroundImageInfo* m_backgroundImageInfo;
	AddibleGcpTableModel* m_gcpTableModel;

// CONTROLLER
public:
	void setDialog(BackgroundImageInfoGeoreferenceDialog* dialog);

	void addCustomMenuItems(QMenu* menu);

	/// @name Event handlers for events on view
	//@{
	void keyPressEvent(QKeyEvent* event, GeoreferenceView* view);
	// void keyReleaseEvent(QKeyEvent* event, GeoreferenceView* view);
	void mouseDoubleClickEvent(QMouseEvent* event, GeoreferenceView* view);
	void mouseMoveEvent(QMouseEvent* event, GeoreferenceView* view);
	void mousePressEvent(QMouseEvent* event, GeoreferenceView* view);
	void mouseReleaseEvent(QMouseEvent* event, GeoreferenceView* view);
	void wheelEvent(QWheelEvent* event, GeoreferenceView* view);
	//@}

	QMenu* rightClickMenu();

public slots:
	void selectPoints(const std::unordered_set<std::vector<GcpTableRow>::size_type>& indices);
	void updateTableSelection(const std::unordered_set<std::vector<GcpTableRow>::size_type>& indices);

signals:
	void gcpDefined();
	void selectedIndexChanged(const std::unordered_set<std::vector<GcpTableRow>::size_type>&);

private slots:
	void deletePoints();
	void startGcpSelect(const QPointF& point);

private:
	BackgroundImageInfoGeoreferenceDialog* m_dialog;

	QMenu m_rightClickMenu;

	QAction* m_deletePointsAction;

	void updateSelectionHelper(::SelectionHelper* selectionHelper);

	class SelectionHelper;
	class VoidSelectionHelper;
	class InitSelectionHelper;
	class DestinationOperationWaitingSelectionHelper;
	class PointsInRectangleSelectionHelper;
	class TranslationSelectionHelper;
	class WaitingSelectionHelper;
	std::unique_ptr<SelectionHelper> m_selectionHelper;

// VIEW
public:
	void paintView(QPainter* painter, const QTransform& transform, const QRectF& rect) const;

	std::vector<GeoreferenceViewHelper*>& viewHelpers();

private:
	void doPaintView(QPainter* painter, const QTransform& transform, const QRectF& rect) const;
	void paintHelpers(QPainter* painter, const QTransform& transform, const QRectF& rect) const;

	std::vector<GeoreferenceViewHelper*> m_viewHelpers;

public:
	friend class GeoreferenceViewHelper;
};

#endif // GEOREFERENCEVIEW_IMAGEINFO_H
