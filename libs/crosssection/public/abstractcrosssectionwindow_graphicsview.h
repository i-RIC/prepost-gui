#ifndef ABSTRACTCROSSSECTIONWINDOW_GRAPHICSVIEW_H
#define ABSTRACTCROSSSECTIONWINDOW_GRAPHICSVIEW_H

#include "../crosssection_global.h"
#include "../abstractcrosssectionwindow.h"

#include <QAbstractItemView>
#include <QPointF>

#include <unordered_map>
#include <vector>

class PreProcessorGridDataItemI;

class CROSSSECTIONDLL_EXPORT AbstractCrosssectionWindow::GraphicsView : public QAbstractItemView
{
	Q_OBJECT

public:
	GraphicsView(QWidget* w);

	void setImpl(Impl* impl);
	void setEditEnabled(bool enabled);
	void clearSelection();

	void render();
	void cameraFit();
	double aspectRatio() const;
	void setAspectRatio(double ratio);

	void paintEvent(QPaintEvent* event) override;

	void loadFromProjectMainFile(const QDomNode& node);
	void saveToProjectMainFile(QXmlStreamWriter& writer);
	std::vector<double> setupNodePositions() const;
	void setGridDataItem(PreProcessorGridDataItemI* item);

	QRect visualRect(const QModelIndex&) const override;
	void scrollTo(const QModelIndex& index, ScrollHint hint) override;
	QModelIndex indexAt(const QPoint&) const override;
	QModelIndex moveCursor(QAbstractItemView::CursorAction, Qt::KeyboardModifiers) override;
	int horizontalOffset() const override;
	int verticalOffset() const override;
	bool isIndexHidden(const QModelIndex&) const override;
	void setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command) override;
	void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) override;
	QRegion visualRegionForSelection(const QItemSelection& selection) const override;

private:
	enum class ViewMouseEventMode {
		Normal,
		Zooming,
		Translating,
		ChangeHeightPossible,
		ChangingHeight,
		SelectingArea,
		ChangingElevationPossible,
		ChangingElevation,
	};

	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

	class DrawRegionInformation;

	void setupRegions(
		std::vector<GridAttributeDisplaySettingContainer>& settings,
		std::unordered_map<GridAttributeDisplaySettingContainer*, DrawRegionInformation>* colorMapRegions,
		std::unordered_map<GridAttributeDisplaySettingContainer*, DrawRegionInformation>* independentChartRegions,
		DrawRegionInformation* elevationChartRegion) const;

	void drawScales(QPainter* painter, const QMatrix& matrix, int ymax);
	void drawAspectRatio(QPainter* painter, int ymax);
	void drawSelectArea(QPainter* painter);

	void translate(int x, int y);
	void zoom(double scaleX, double scaleY);
	void updateMouseCursor();
	void updateSelectedIndices();
	void pushElevationEditCommand();

	QMatrix matrix(int chartHeight) const;

	class ElevationChartController;
	class IndependentChartController;
	class ColorMapController;

	ViewMouseEventMode m_viewMouseEventMode;
	std::string m_heightChangeTarget;

	QPointF m_center;
	double m_scaleX;
	double m_scaleY;

	QPixmap m_zoomPixmap;
	QPixmap m_movePixmap;
	QCursor m_zoomCursor;
	QCursor m_moveCursor;
	QPoint m_oldPosition;

	bool m_editEnabled;
	QPoint m_selectAreaStartPosition;
	QPoint m_selectAreaEndPosition;
	PreProcessorGridDataItemI* m_gridDataItem;

	std::unordered_map<unsigned int, std::vector<unsigned int> > m_selectedIndices;
	QPoint m_dragStartPosition;
	int m_elevationOffset;

	Impl* m_impl;
};

#endif // ABSTRACTCROSSSECTIONWINDOW_GRAPHICSVIEW_H
