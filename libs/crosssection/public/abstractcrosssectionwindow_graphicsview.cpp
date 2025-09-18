#include "abstractcrosssectionwindow_graphicsview.h"
#include "../private/abstractcrosssectionwindow_displaysettingcontainer.h"
#include "../private/abstractcrosssectionwindow_edittablecontroller.h"
#include "../private/abstractcrosssectionwindow_graphicsview_colormapcontroller.h"
#include "../private/abstractcrosssectionwindow_graphicsview_drawregioninformation.h"
#include "../private/abstractcrosssectionwindow_graphicsview_elevationchartcontroller.h"
#include "../private/abstractcrosssectionwindow_graphicsview_independentchartcontroller.h"
#include "../private/abstractcrosssectionwindow_gridattributedisplaysettingcontainer.h"
#include "../private/abstractcrosssectionwindow_impl.h"
#include "../public/abstractcrosssectionwindow_controller.h"

#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/pre/base/preprocessorgriddataitemi.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/pre/gridcond/base/gridattributeeditcommand.h>
#include <guicore/pre/gridcond/container/gridattributerealcontainer.h>
#include <misc/iricundostack.h>
#include <misc/mathsupport.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/xmlsupport.h>

#include <vtkCellData.h>
#include <vtkExtractGrid.h>
#include <vtkSmartPointer.h>
#include <vtkPointData.h>
#include <vtkPoints.h>

#include <QPainter>

#include <unordered_map>

using W2 = AbstractCrosssectionWindow;
using GV = AbstractCrosssectionWindow::GraphicsView;

namespace {

const double VERYSMALL = 1E-10;

const int FIT_MARGIN_PIXELS = 20;
const double FIT_MARGIN_RATIO = 0.15;
const int MIN_SCALE_COUNT = 3;
const int SIDE_MARKER_HOFFSET = 10;
const int SIDE_MARKER_VOFFSET = 40;
const int ASPECT_RATIO_RIGHT_MARGIN = 10;
const int ASPECT_RATIO_BOTTOM_MARGIN = 10;
const int HEIGHT_DRAG_WIDTH = 5;

void calcAutoScale(double width, double* scale, double* subScale)
{
	if (width < 0) {
		*scale = 1.0;
		*subScale = 0.2;
		return;
	}
	double w = width / MIN_SCALE_COUNT;
	int i = 0;
	while (w > 10) {
		w /= 10;
		++i;
	}
	while (w < 1) {
		w *= 10;
		--i;
	}
	double pow10 = 10;
	if (i < 0) {
		pow10 = 0.1;
		i = -i;
	}
	if (w > 5) {
		*scale = 5 * std::pow(pow10, i);
		*subScale = *scale * 0.2;
	} else if (w > 2) {
		*scale = 2 * std::pow(pow10, i);
		*subScale = *scale * 0.5;
	} else {
		*scale = std::pow(pow10, i);
		*subScale = *scale * 0.2;
	}
}

}

AbstractCrosssectionWindow::GraphicsView::GraphicsView(QWidget* w) :
	QAbstractItemView {w},
	m_viewMouseEventMode {ViewMouseEventMode::Normal},
	m_zoomPixmap {":/libs/guibase/images/cursorZoom.png"},
	m_movePixmap {":/libs/guibase/images/cursorMove.png"},
	m_zoomCursor {m_zoomPixmap},
	m_moveCursor {m_movePixmap},
	m_editEnabled {false},
	m_selectAreaStartPosition {},
	m_selectAreaEndPosition {},
	m_gridDataItem {nullptr},
	m_dragStartPosition {},
	m_elevationOffset {0},
	m_impl {nullptr}
{
	setMouseTracking(true);
}

void AbstractCrosssectionWindow::GraphicsView::setImpl(Impl* impl)
{
	m_impl = impl;
}

void AbstractCrosssectionWindow::GraphicsView::setEditEnabled(bool enabled)
{
	m_editEnabled = enabled;
}

void AbstractCrosssectionWindow::GraphicsView::clearSelection()
{
	m_selectedIndices.clear();
}

void AbstractCrosssectionWindow::GraphicsView::render()
{
	viewport()->update();
}

void AbstractCrosssectionWindow::GraphicsView::cameraFit()
{
	if (m_impl->m_displaySetting.fixRegion) {return;}

	auto positions = setupNodePositions();
	if (positions.size() == 0) {return;}

	auto xMin = *positions.begin();
	auto xMax = *positions.rbegin();

	double yMin = 0, yMax = 0;

	std::unordered_map<v4Structured2dGrid*, vtkPointSet*> outputs;

	if (m_impl->m_window->impl->m_mode == Mode::UnstructuredEdge) {
		auto g = m_impl->m_window->targetGrid();
		g->vtkData()->data()->Register(nullptr);
		outputs.insert({g, g->vtkData()->data()});

		auto ag = m_impl->m_window->targetAdditionalGrid();
		if (ag != nullptr) {
			ag->vtkData()->data()->Register(nullptr);
			outputs.insert({ag, ag->vtkData()->data()});
		}
	} else if (m_impl->m_window->impl->m_mode == Mode::StructuredIJ) {
		auto extractGrid = vtkSmartPointer<vtkExtractGrid>::New();
		auto additinalExtractGrid = vtkSmartPointer<vtkExtractGrid>::New();
		auto g = m_impl->m_window->targetGrid();
		auto index = m_impl->m_controller->targetIndex();
		if (m_impl->m_controller->targetDirection() == Direction::I) {
			extractGrid->SetVOI(index, index, 0, g->dimensionJ(), 0, 0);
			additinalExtractGrid->SetVOI(index, index, 0, g->dimensionJ(), 0, 0);
		} else if (m_impl->m_controller->targetDirection() == Direction::J) {
			extractGrid->SetVOI(0, g->dimensionI(), index, index, 0, 0);
			additinalExtractGrid->SetVOI(0, g->dimensionI(), index, index, 0, 0);
		}

		extractGrid->SetInputData(g->vtkConcreteData()->concreteData());
		extractGrid->Update();
		auto output = extractGrid->GetOutput();
		output->Register(nullptr);
		outputs.insert({g, output});

		auto ag = m_impl->m_window->targetAdditionalGrid();
		if (ag != nullptr) {
			additinalExtractGrid->SetInputData(ag->vtkConcreteData()->concreteData());
			additinalExtractGrid->Update();
			auto output = additinalExtractGrid->GetOutput();
			output->Register(nullptr);
			outputs.insert({ag, output});
		}
	}

	bool first = true;
	for (auto& s : m_impl->m_displaySettings) {
		if (! s.visible) {continue;}
		if (s.mode != GridAttributeDisplaySettingContainer::Mode::AsElevation) {continue;}
		if (s.position() != GridAttributeDisplaySettingContainer::Position::Node) {continue;}

		auto it = outputs.find(s.grid);
		if (it == outputs.end()) {
			continue;
		}

		auto data = it->second->GetPointData()->GetArray(s.attributeName().c_str());
		double range[2];
		data->GetRange(range);
		if (first || range[0] < yMin) {yMin = range[0];}
		if (first || range[1] > yMax) {yMax = range[1];}

		first = false;
	}

	for (auto pair : outputs) {
		pair.second->Delete();
	}

	// xMin, xMax, yMin, yMax are setup.

	m_center.setX((xMin + xMax) * 0.5);
	m_center.setY((yMin + yMax) * 0.5);

	double xDiff = xMax - xMin;
	if (xDiff < VERYSMALL) {xDiff = 1;}
	double yDiff = yMax - yMin;
	if (yDiff < VERYSMALL) {yDiff = 1;}

	double previousAspectRatio = aspectRatio();

	m_scaleX = (viewport()->width() - 2 * FIT_MARGIN_PIXELS) / (xDiff * (1 + FIT_MARGIN_RATIO * 2));
	if (m_scaleX < 0) {m_scaleX = 1;}

	auto height = viewport()->height();
	for (auto& s : m_impl->m_displaySettings) {
		if (! s.visible) {continue;}
		if (s.mode == GridAttributeDisplaySettingContainer::Mode::Chart) {
			height -= s.independentChartHeight;
		} else if (s.mode == GridAttributeDisplaySettingContainer::Mode::ColorMap) {
			height -= s.colorBarHeight;
		}
	}

	m_scaleY = (height - 2 * FIT_MARGIN_PIXELS) / (yDiff * (1 + FIT_MARGIN_RATIO * 2));
	if (m_scaleY < 0) {m_scaleY = 1;}

	if (m_impl->m_displaySetting.fixAspectRatio) {
		m_scaleY = m_scaleX * previousAspectRatio;
	}

	m_impl->updateAspectRatioEdit();
	render();
}

void AbstractCrosssectionWindow::GraphicsView::paintEvent(QPaintEvent* /*event*/)
{
	QPainter painter(viewport());
	painter.setRenderHint(QPainter::Antialiasing);

	std::unordered_map<GridAttributeDisplaySettingContainer*, DrawRegionInformation> colorMapRegions;
	std::unordered_map<GridAttributeDisplaySettingContainer*, DrawRegionInformation> independentChartRegions;
	DrawRegionInformation elevationChartRegion;

	setupRegions(m_impl->m_displaySettings, &colorMapRegions, &independentChartRegions, &elevationChartRegion);

	auto m = matrix(elevationChartRegion.yMax);
	drawScales(&painter, m, elevationChartRegion.yMax);

	auto nodePositions = setupNodePositions();

	std::vector<GridAttributeDisplaySettingContainer*> activeSettings;
	for (auto& s : m_impl->m_displaySettings) {
		if (! s.visible) {continue;}

		activeSettings.push_back(&s);
	}

	const auto& model = m_impl->m_editTableController->model();
	if (model.rowCount() == 0) {return;}
	if (model.rowCount() != nodePositions.size()) {return;}

	for (auto it = activeSettings.rbegin(); it != activeSettings.rend(); ++it) {
		auto setting = *it;

		int column = static_cast<int> (activeSettings.size()) - 1 - (it - activeSettings.rbegin());

		if (setting->mode == GridAttributeDisplaySettingContainer::Mode::AsElevation) {
			ElevationChartController controller(this);
			std::vector<QVariant> values;
			for (int i = 0; i < model.rowCount(); ++i) {
				values.push_back(model.data(model.index(i, column), Qt::EditRole));
			}

			std::vector<unsigned int> selected;
			auto it = m_selectedIndices.find(column);
			if (it != m_selectedIndices.end()) {
				selected = it->second;
			}
			controller.paint(nodePositions, values, selected, setting, m, m_elevationOffset, &painter);
		} else if (setting->mode == GridAttributeDisplaySettingContainer::Mode::Chart) {
			IndependentChartController controller(this);
			std::vector<QVariant> values;
			int startIndex = 0;
			if (setting->position() == GridAttributeDisplaySettingContainer::Position::Cell) {
				startIndex = 1;
			}
			for (int i = startIndex; i < model.rowCount(); ++i) {
				values.push_back(model.data(model.index(i, column), Qt::EditRole));
			}
			if (setting->position() == GridAttributeDisplaySettingContainer::Position::Cell) {
				controller.paintCell(nodePositions, values, setting, independentChartRegions.at(setting), m, &painter);
			} else {
				controller.paintNode(nodePositions, values, setting, independentChartRegions.at(setting), m, &painter);
			}
		} else if (setting->mode == GridAttributeDisplaySettingContainer::Mode::ColorMap) {
			ColorMapController controller(this);
			std::vector<QVariant> values;
			int startIndex = 0;
			if (setting->position() == GridAttributeDisplaySettingContainer::Position::Cell) {
				startIndex = 1;
			}
			for (int i = startIndex; i < model.rowCount(); ++i) {
				values.push_back(model.data(model.index(i, column), Qt::EditRole));
			}
			if (setting->position() == GridAttributeDisplaySettingContainer::Position::Cell) {
				controller.paintCell(nodePositions, values, setting, colorMapRegions.at(setting), m, &painter);
			} else {
				controller.paintNode(nodePositions, values, setting, colorMapRegions.at(setting), m, &painter);
			}
		}
	}
	drawAspectRatio(&painter, elevationChartRegion.yMax);
	drawSelectArea(&painter);
}

void AbstractCrosssectionWindow::GraphicsView::loadFromProjectMainFile(const QDomNode& node)
{
	double cx = iRIC::getDoubleAttribute(node, "centerX");
	double cy = iRIC::getDoubleAttribute(node, "centerY");
	m_center = QPointF(cx, cy);

	m_scaleX = iRIC::getDoubleAttribute(node, "scaleX");
	m_scaleY = iRIC::getDoubleAttribute(node, "scaleY");
	m_impl->updateAspectRatioEdit();
}

void AbstractCrosssectionWindow::GraphicsView::saveToProjectMainFile(QXmlStreamWriter& writer)
{
	iRIC::setDoubleAttribute(writer, "centerX", m_center.x());
	iRIC::setDoubleAttribute(writer, "centerY", m_center.y());
	iRIC::setDoubleAttribute(writer, "scaleX", m_scaleX);
	iRIC::setDoubleAttribute(writer, "scaleY", m_scaleY);
}

QRect AbstractCrosssectionWindow::GraphicsView::visualRect(const QModelIndex&) const
{
	return QRect();
}

void AbstractCrosssectionWindow::GraphicsView::scrollTo(const QModelIndex& /*index*/, ScrollHint /*hint*/)
{}

QModelIndex AbstractCrosssectionWindow::GraphicsView::indexAt(const QPoint&) const
{
	viewport()->update();
	return QModelIndex();
}

QModelIndex AbstractCrosssectionWindow::GraphicsView::moveCursor(QAbstractItemView::CursorAction, Qt::KeyboardModifiers)
{
	viewport()->update();
	return QModelIndex();
}

int AbstractCrosssectionWindow::GraphicsView::horizontalOffset() const
{
	return 0;
}

int AbstractCrosssectionWindow::GraphicsView::verticalOffset() const
{
	return 0;
}

bool AbstractCrosssectionWindow::GraphicsView::isIndexHidden(const QModelIndex&) const
{
	return false;
}

void AbstractCrosssectionWindow::GraphicsView::setSelection(const QRect& /*rect*/, QItemSelectionModel::SelectionFlags /*command*/)
{
	viewport()->update();
}

void AbstractCrosssectionWindow::GraphicsView::selectionChanged(const QItemSelection& /*selected*/, const QItemSelection& /*deselected*/)
{
	viewport()->update();
}

QRegion AbstractCrosssectionWindow::GraphicsView::visualRegionForSelection(const QItemSelection& /*selection*/) const
{
	return QRegion();
}

void AbstractCrosssectionWindow::GraphicsView::mouseMoveEvent(QMouseEvent* event)
{
	int diffx = event->x() - m_oldPosition.x();
	int diffy = event->y() - m_oldPosition.y();

	if (m_viewMouseEventMode == ViewMouseEventMode::Translating) {
		translate(diffx, diffy);
	} else if (m_viewMouseEventMode == ViewMouseEventMode::Zooming) {
		double scaleX = 1 + diffx * 0.02;
		double scaleY = 1 - diffy * 0.02;
		if (scaleX < 0.5) {scaleX = 0.5;}
		if (scaleY < 0.5) {scaleY = 0.5;}
		if (scaleX > 2) {scaleX = 2;}
		if (scaleY > 2) {scaleY = 2;}

		if (m_impl->m_displaySetting.fixAspectRatio) {
			zoom(scaleY, scaleY);
		} else {
			zoom(scaleX, scaleY);
		}
	} else if (m_viewMouseEventMode == ViewMouseEventMode::ChangingHeight) {
		auto it = m_impl->m_displaySettingMap.find(m_heightChangeTarget);
		if (it == m_impl->m_displaySettingMap.end()) {
			double d = 10 + 10;
		} else {
			auto setting = it->second;
			if (setting->mode == GridAttributeDisplaySettingContainer::Mode::Chart) {
				setting->independentChartHeight -= diffy;
				if (setting->independentChartHeight < GridAttributeDisplaySettingContainer::MIN_HEIGHT) {
					setting->independentChartHeight = GridAttributeDisplaySettingContainer::MIN_HEIGHT;
				}
			}
			else if (setting->mode == GridAttributeDisplaySettingContainer::Mode::ColorMap) {
				setting->colorBarHeight -= diffy;
				if (setting->colorBarHeight < GridAttributeDisplaySettingContainer::MIN_HEIGHT) {
					setting->colorBarHeight = GridAttributeDisplaySettingContainer::MIN_HEIGHT;
				}
			}
			viewport()->update();
		}
	} else if (m_viewMouseEventMode == ViewMouseEventMode::ChangingElevation) {
		m_elevationOffset = event->y() - m_dragStartPosition.y();
		viewport()->update();
	} else if (m_viewMouseEventMode == ViewMouseEventMode::Normal || m_viewMouseEventMode == ViewMouseEventMode::ChangeHeightPossible || m_viewMouseEventMode == ViewMouseEventMode::ChangingElevationPossible){
		std::unordered_map<GridAttributeDisplaySettingContainer*, DrawRegionInformation> colorMapRegions;
		std::unordered_map<GridAttributeDisplaySettingContainer*, DrawRegionInformation> independentChartRegions;
		DrawRegionInformation elevationChartRegion;

		setupRegions(m_impl->m_displaySettings, &colorMapRegions, &independentChartRegions, &elevationChartRegion);
		m_viewMouseEventMode = ViewMouseEventMode::Normal;
		for (auto pair : independentChartRegions) {
			if (std::abs(event->y() - pair.second.yMin) < HEIGHT_DRAG_WIDTH) {
				m_viewMouseEventMode = ViewMouseEventMode::ChangeHeightPossible;
				m_heightChangeTarget = pair.first->attributeName();
				break;
			}
		}
		for (auto pair : colorMapRegions) {
			if (std::abs(event->y() - pair.second.yMin) < HEIGHT_DRAG_WIDTH) {
				m_viewMouseEventMode = ViewMouseEventMode::ChangeHeightPossible;
				m_heightChangeTarget = pair.first->attributeName();
				break;
			}
		}

		auto m = matrix(elevationChartRegion.yMax);
		auto nodePositions = setupNodePositions();
		const auto& model = m_impl->m_editTableController->model();
		for (const auto& pair : m_selectedIndices) {
			auto column = pair.first;
			for (const auto& row : pair.second) {
				auto value = model.data(model.index(row, column), Qt::EditRole).toDouble();
				auto p = m.map(QPointF(nodePositions.at(row), value));
				if (std::abs(p.x() - event->x()) < iRIC::nearRadius() && std::abs(p.y() - event->y()) < iRIC::nearRadius()) {
					m_viewMouseEventMode = ViewMouseEventMode::ChangingElevationPossible;
					break;
				}
			}
		}

		updateMouseCursor();
	} else if (m_viewMouseEventMode == ViewMouseEventMode::SelectingArea) {
		m_selectAreaEndPosition = event->pos();
		viewport()->update();
	}

	m_oldPosition = event->pos();
}

void AbstractCrosssectionWindow::GraphicsView::mousePressEvent(QMouseEvent* event)
{

	if (event->modifiers() == Qt::ControlModifier) {
		if (m_impl->m_displaySetting.fixRegion) {
			return;
		}

		switch (event->button()) {
		case Qt::LeftButton:
			m_viewMouseEventMode = ViewMouseEventMode::Translating;
			break;
		case Qt::MidButton:
			m_viewMouseEventMode = ViewMouseEventMode::Zooming;
			break;
		default:
			break;
		}
		m_oldPosition = event->pos();
		updateMouseCursor();
	} else {
		if (event->button() == Qt::LeftButton) {
			if (m_viewMouseEventMode == ViewMouseEventMode::ChangeHeightPossible) {
				m_viewMouseEventMode = ViewMouseEventMode::ChangingHeight;
				updateMouseCursor();
			} else if (m_editEnabled) {
				if (m_viewMouseEventMode == ViewMouseEventMode::ChangingElevationPossible) {
					m_viewMouseEventMode = ViewMouseEventMode::ChangingElevation;
					m_dragStartPosition = event->pos();
					updateMouseCursor();
				} else {
					m_selectAreaStartPosition = event->pos();
					m_selectAreaEndPosition = event->pos();
					m_viewMouseEventMode = ViewMouseEventMode::SelectingArea;
				}
			}
		}
	}
}

void AbstractCrosssectionWindow::GraphicsView::mouseReleaseEvent(QMouseEvent* /*event*/)
{
	bool needRender = false;
	if (m_viewMouseEventMode == ViewMouseEventMode::SelectingArea) {
		updateSelectedIndices();
		needRender = true;
	} else if (m_viewMouseEventMode == ViewMouseEventMode::ChangingElevation) {
		pushElevationEditCommand();
		m_elevationOffset = 0;
	}

	m_viewMouseEventMode = ViewMouseEventMode::Normal;
	updateMouseCursor();

	if (needRender) {
		viewport()->update();
	}
}

void AbstractCrosssectionWindow::GraphicsView::setupRegions(
		std::vector<GridAttributeDisplaySettingContainer>& settings,
		std::unordered_map<GridAttributeDisplaySettingContainer*, DrawRegionInformation>* colorMapRegions,
		std::unordered_map<GridAttributeDisplaySettingContainer*, DrawRegionInformation>* independentChartRegions,
		DrawRegionInformation* elevationChartRegion) const
{
	auto height = viewport()->height();
	double heightSum = 0;
	for (auto it = settings.rbegin(); it != settings.rend(); ++it) {
		auto& setting = *it;
		if (! setting.visible) {continue;}

		if (setting.mode == GridAttributeDisplaySettingContainer::Mode::Chart) {
			DrawRegionInformation info;
			info.yMax = height - heightSum;
			info.yMin = info.yMax - setting.independentChartHeight;
			independentChartRegions->insert({&setting, info});
			heightSum += setting.independentChartHeight;
		} else if (setting.mode == GridAttributeDisplaySettingContainer::Mode::ColorMap) {
			DrawRegionInformation info;
			info.yMax = height - heightSum;
			info.yMin = info.yMax - setting.colorBarHeight;
			colorMapRegions->insert({&setting, info});
			heightSum += setting.colorBarHeight;
		}
	}
	elevationChartRegion->yMax = height - heightSum;
	elevationChartRegion->yMin = 0;
}

std::vector<double> AbstractCrosssectionWindow::GraphicsView::setupNodePositions() const
{
	if (m_impl->m_window->impl->m_mode == AbstractCrosssectionWindow::Mode::UnstructuredEdge) {
		auto grid = m_impl->m_window->targetGrid();
		std::vector<double> positions;

		auto points = grid->vtkData()->data()->GetPoints();
		QPointF point, previousPoint;

		double p[3];
		points->GetPoint(0, p);
		previousPoint = QPointF(p[0], p[1]);

		double lastPosition = 0;
		positions.push_back(lastPosition);

		for (vtkIdType i = 1; i < points->GetNumberOfPoints(); ++i) {
			points->GetPoint(i, p);
			point = QPointF(p[0], p[1]);
			double dist = iRIC::distance(point, previousPoint);
			double position = lastPosition + dist;
			positions.push_back(position);

			lastPosition = position;
			previousPoint = point;
		}

		return positions;
	} else if (m_impl->m_window->impl->m_mode == AbstractCrosssectionWindow::Mode::StructuredIJ) {
		auto grid = m_impl->m_window->targetGrid();
		std::vector<double> positions;

		if (grid == nullptr) {return positions;}
		auto extractGrid = vtkSmartPointer<vtkExtractGrid>::New();
		extractGrid->SetInputData(grid->vtkConcreteData()->concreteData());
		auto index = m_impl->m_controller->targetIndex();
		if (index == -1) {return positions;}

		if (m_impl->m_controller->targetDirection() == Direction::I) {
			extractGrid->SetVOI(index, index, 0, grid->dimensionJ(), 0, 0);
		} else if (m_impl->m_controller->targetDirection() == Direction::J) {
			extractGrid->SetVOI(0, grid->dimensionI(), index, index, 0, 0);
		}
		extractGrid->Update();
		auto output = extractGrid->GetOutput();
		auto points = output->GetPoints();
		if (points == nullptr) {return positions;}

		QPointF point, previousPoint;

		double p[3];
		points->GetPoint(0, p);
		previousPoint = QPointF(p[0], p[1]);

		double lastPosition = 0;
		positions.push_back(lastPosition);

		for (vtkIdType i = 1; i < points->GetNumberOfPoints(); ++i) {
			points->GetPoint(i, p);
			point = QPointF(p[0], p[1]);
			double dist = iRIC::distance(point, previousPoint);
			double position = lastPosition + dist;
			positions.push_back(position);

			lastPosition = position;
			previousPoint = point;
		}

		return positions;
	}
}

void AbstractCrosssectionWindow::GraphicsView::setGridDataItem(PreProcessorGridDataItemI* item)
{
	m_gridDataItem = item;
}

void AbstractCrosssectionWindow::GraphicsView::drawScales(QPainter* painter, const QMatrix& matrix, int ymax)
{
	QWidget* w = viewport();
	QMatrix invMatrix = matrix.inverted();
	QPointF mins, maxs;
	mins = invMatrix.map(QPointF(w->width(), w->height()));
	maxs = invMatrix.map(QPointF(0, 0));

	double xoffset = 5;
	double yoffset = 5;
	double fontoffset = 4;
	double mainruler = 5;
	double subruler = 3;

	double xScale = m_impl->m_displaySetting.bgHScaleInterval;
	double xSubScale = m_impl->m_displaySetting.bgHSubScaleInterval;
	double yScale = m_impl->m_displaySetting.bgVScaleInterval;
	double ySubScale = m_impl->m_displaySetting.bgVSubScaleInterval;

	if (m_impl->m_displaySetting.bgHScaleAuto) {
		calcAutoScale(maxs.x() - mins.x(), &xScale, &xSubScale);
	}
	if (m_impl->m_displaySetting.bgVScaleAuto) {
		calcAutoScale(maxs.y() - mins.y(), &yScale, &ySubScale);
	}

	if (m_impl->m_displaySetting.gridVisible) {
		painter->save();

		if (m_impl->m_displaySetting.bgGridType == DisplaySettingContainer::BackgroundGridType::Lines) {
			// lines
			// scales
			QPen pen(m_impl->m_displaySetting.bgGridColor);
			pen.setStyle(Qt::SolidLine);
			painter->setPen(pen);

			double x = std::floor(mins.x() / xScale) * xScale;
			while (x < maxs.x()) {
				auto from = matrix.map(QPointF(x, maxs.y()));
				auto to = from;
				from.setY(0);
				to.setY(w->height());
				painter->drawLine(from, to);
				x += xScale;
			}
			double y = std::floor(mins.y() / yScale) * yScale;
			while (y < maxs.y()) {
				auto from = matrix.map(QPointF(mins.x(), y));
				auto to = from;
				from.setX(0);
				to.setX(w->width());
				if (from.y() < ymax) {
					painter->drawLine(from, to);
				}
				y += yScale;
			}
			// subscales
			pen.setStyle(Qt::DashLine);
			painter->setPen(pen);
			x = std::floor(mins.x() / xSubScale) * xSubScale;
			while (x < maxs.x()) {
				auto from = matrix.map(QPointF(x, maxs.y()));
				auto to = from;
				from.setY(0);
				to.setY(w->height());
				painter->drawLine(from, to);
				x += xSubScale;
			}
			y = std::floor(mins.y() / ySubScale) * ySubScale;
			while (y < maxs.y()) {
				auto from = matrix.map(QPointF(mins.x(), y));
				auto to = from;
				from.setX(0);
				to.setX(w->width());
				if (from.y() < ymax) {
					painter->drawLine(from, to);
				}
				y += ySubScale;
			}
		} else {
			// dots
			QPen pen(m_impl->m_displaySetting.bgGridColor);
			pen.setStyle(Qt::SolidLine);
			painter->setPen(pen);
			QBrush brush(m_impl->m_displaySetting.bgGridColor);
			painter->setBrush(brush);

			double x = std::floor(mins.x() / xScale) * xScale;
			while (x < maxs.x()) {
				double y = std::floor(mins.y() / yScale) * yScale;
				while (y < maxs.y()) {
					auto from = matrix.map(QPointF(x, y));
					from.setX(from.x() - 1);
					from.setY(from.y() - 1);
					QPointF to(from.x() + 2, from.y() + 2);
					if (from.y() < ymax) {
						painter->drawRect(QRectF(from, to));
					}
					y += yScale;
				}
				x += xScale;
			}
			x = std::floor(mins.x() / xSubScale) * xSubScale;
			while (x < maxs.x()) {
				double y = std::floor(mins.y() / ySubScale) * ySubScale;
				while (y < maxs.y()) {
					auto p = matrix.map(QPointF(x, y));
					if (p.y() < ymax) {
						painter->drawPoint(p);
					}
					y += ySubScale;
				}
				x += xSubScale;
			}
		}
		painter->restore();
	}

	if (m_impl->m_displaySetting.scaleVisible) {
		painter->save();

		painter->setPen(m_impl->m_displaySetting.distanceMarkersColor);
		painter->setFont(m_impl->m_displaySetting.distanceMarkersFont);
		QFontMetrics metrics(m_impl->m_displaySetting.distanceMarkersFont);

		// Horizontal line
		QPointF from, to;
		from = QPointF(0, yoffset);
		to = QPointF(w->width(), yoffset);
		painter->drawLine(from, to);

		// draw X scales
		double x = std::floor(mins.x() / xScale) * xScale;
		while (x < maxs.x()) {
			from = matrix.map(QPointF(x, maxs.y()));
			from.setY(yoffset);
			to = from;
			to.setY(yoffset + mainruler);
			painter->drawLine(from, to);

			QPointF fontPos = to;
			fontPos.setY(yoffset + mainruler + fontoffset);
			double displayX = x;
			if (std::abs(displayX) < VERYSMALL) {
				displayX = 0;
			}
			QString str = QString("%1").arg(displayX);
			auto rect = metrics.boundingRect(str);
			QRectF fontRect(fontPos.x() - rect.width() / 2, fontPos.y(), rect.width() + 5, rect.height() + 5);
			painter->drawText(fontRect, Qt::AlignHCenter | Qt::AlignTop, str);
			x += xScale;
		}
		// draw X sub scales.
		x = std::floor(mins.x() / xSubScale) * xSubScale;
		while (x < maxs.x()) {
			from = matrix.map(QPointF(x, maxs.y()));
			from.setY(from.y() + yoffset);
			to = from;
			to.setY(to.y() + yoffset + subruler);
			painter->drawLine(from, to);
			x += xSubScale;
		}

		// Vertical line
		from = QPointF(xoffset, 0);
		to = QPointF(xoffset, ymax);
		painter->drawLine(from, to);

		// draw Y scales
		double y = std::floor(mins.y() / yScale) * yScale;
		while (y < maxs.y()) {
			from = matrix.map(QPointF(mins.x(), y));
			from.setX(xoffset);
			to.setX(xoffset + mainruler);
			to.setY(from.y());
			if (from.y() < ymax) {
				painter->drawLine(from, to);
			}

			QPointF fontPos = to;
			fontPos.setX(xoffset + mainruler + fontoffset);
			double displayY = y;
			if (std::abs(displayY) < VERYSMALL) {
				displayY = 0;
			}
			QString str = QString("%1").arg(displayY);
			auto rect = metrics.boundingRect(str);
			QRectF fontRect(fontPos.x(), fontPos.y() - rect.height() / 2, rect.width() + 5, rect.height() + 5);
			if (fontRect.bottom() < ymax) {
				painter->drawText(fontRect, Qt::AlignLeft | Qt::AlignVCenter, str);
			}
			y += yScale;
		}
		// draw Y sub scales.
		y = std::floor(mins.y() / ySubScale) * ySubScale;
		while (y < maxs.y()) {
			from = matrix.map(QPointF(mins.x(), y));
			from.setX(xoffset);
			to.setX(xoffset + subruler);
			to.setY(from.y());
			if (from.y() < ymax) {
				painter->drawLine(from, to);
			}
			y += ySubScale;
		}
		painter->restore();
	}

	if (m_impl->m_displaySetting.markersVisible) {
		painter->save();

		// line at x = 0;
		auto from = matrix.map(QPointF(0, 0));
		from.setY(0);
		auto to = from;
		to.setY(w->height());

		QPen pen(m_impl->m_displaySetting.lbBankMarkersColor);
		pen.setStyle(Qt::DotLine);
		painter->setPen(pen);
		painter->drawLine(from, to);

		painter->setFont(m_impl->m_displaySetting.lbBankMarkersFont);
		QFontMetricsF metrics(m_impl->m_displaySetting.lbBankMarkersFont);

		QString label;

		if (m_impl->m_controller->targetDirection() == Direction::I) {
			// left bank
			label = tr("Left Bank Side");
			auto rect = metrics.boundingRect(label);
			QRectF fontRect = QRectF(SIDE_MARKER_HOFFSET, SIDE_MARKER_VOFFSET, rect.width() + 5, rect.height() + 5);
			painter->drawText(fontRect, Qt::AlignRight | Qt::AlignTop, label);

			// right bank side
			label = tr("Right Bank Side");
			rect = metrics.boundingRect(label);
			fontRect = QRectF(w->width() - rect.width()	- SIDE_MARKER_HOFFSET, SIDE_MARKER_VOFFSET, rect.width() + 5, rect.height() + 5);
			painter->drawText(fontRect, Qt::AlignLeft | Qt::AlignTop, label);
		} else {
			// Downstream side
			label = tr("Downstream");
			auto rect = metrics.boundingRect(label);
			QRectF fontRect = QRectF(SIDE_MARKER_HOFFSET, SIDE_MARKER_VOFFSET, rect.width() + 5, rect.height() + 5);
			painter->drawText(fontRect, Qt::AlignRight | Qt::AlignTop, label);

			// Upstream
			label = tr("Upstream");
			rect = metrics.boundingRect(label);
			fontRect = QRectF(w->width() - rect.width()	- SIDE_MARKER_HOFFSET, SIDE_MARKER_VOFFSET, rect.width() + 5, rect.height() + 5);
			painter->drawText(fontRect, Qt::AlignLeft | Qt::AlignTop, label);
		}

		painter->restore();
	}
}

void AbstractCrosssectionWindow::GraphicsView::drawAspectRatio(QPainter* painter, int yMax)
{
	if (! m_impl->m_displaySetting.aspectRatioVisible) {return;}

	painter->save();

	painter->setPen(m_impl->m_displaySetting.aspectRatioColor);
	painter->setFont(m_impl->m_displaySetting.aspectRatioFont);

	QSize windowSize = size();

	auto aspectRatioStr = tr("Aspect ratio (V/H): 1 / %1").arg(aspectRatio());
	QFontMetricsF metrics(painter->font());
	auto rect = metrics.boundingRect(aspectRatioStr);

	auto left = windowSize.width() - ASPECT_RATIO_RIGHT_MARGIN - rect.width();
	auto top = yMax - ASPECT_RATIO_BOTTOM_MARGIN - rect.height();
	QRectF fontRect(left, top, rect.width() + 1, rect.height() + 1);
	painter->drawText(fontRect, Qt::AlignLeft | Qt::AlignTop, aspectRatioStr);

	painter->restore();
}

void AbstractCrosssectionWindow::GraphicsView::drawSelectArea(QPainter* painter)
{
	if (m_viewMouseEventMode != ViewMouseEventMode::SelectingArea) {return;}

	double left = std::min(m_selectAreaStartPosition.x(), m_selectAreaEndPosition.x());
	double right = std::max(m_selectAreaStartPosition.x(), m_selectAreaEndPosition.x());
	double top = std::min(m_selectAreaStartPosition.y(), m_selectAreaEndPosition.y());
	double bottom = std::max(m_selectAreaStartPosition.y(), m_selectAreaEndPosition.y());

	QRectF rect(QPointF(left, top), QPointF(right, bottom));

	painter->save();
	QBrush brush(QColor(0, 0, 0, 128));
	painter->fillRect(rect, brush);

	QPen pen(Qt::black, 2);
	painter->setPen(pen);
	painter->drawRect(rect);

	painter->restore();
}

double AbstractCrosssectionWindow::GraphicsView::aspectRatio() const
{
	return m_scaleY / m_scaleX;
}

void AbstractCrosssectionWindow::GraphicsView::setAspectRatio(double ratio)
{
	if (ratio == 0) {ratio = 1;}

	double currentRatio = aspectRatio();
	double rate = currentRatio / ratio;
	if (rate > 0.999 && rate < 1.001) {return;}

	zoom(1.0, ratio / currentRatio);
}

void AbstractCrosssectionWindow::GraphicsView::translate(int x, int y)
{
	m_center.setX(m_center.x() + x / m_scaleX);
	m_center.setY(m_center.y() + y / m_scaleY);
	viewport()->update();
}

void AbstractCrosssectionWindow::GraphicsView::zoom(double scaleX, double scaleY)
{
	m_scaleX *= scaleX;
	m_scaleY *= scaleY;

	m_impl->updateAspectRatioEdit();
	viewport()->update();
}

void AbstractCrosssectionWindow::GraphicsView::updateMouseCursor()
{
	if (m_viewMouseEventMode == ViewMouseEventMode::Zooming) {
		setCursor(m_zoomCursor);
	} else if (m_viewMouseEventMode == ViewMouseEventMode::Translating) {
		setCursor(m_moveCursor);
	} else if (m_viewMouseEventMode == ViewMouseEventMode::ChangeHeightPossible || m_viewMouseEventMode == ViewMouseEventMode::ChangingHeight) {
		setCursor(Qt::SizeVerCursor);
	} else if (m_viewMouseEventMode == ViewMouseEventMode::ChangingElevationPossible) {
		setCursor(Qt::OpenHandCursor);
	} else if (m_viewMouseEventMode == ViewMouseEventMode::ChangingElevation) {
		setCursor(Qt::ClosedHandCursor);
	} else {
		setCursor(Qt::ArrowCursor);
	}
}

void AbstractCrosssectionWindow::GraphicsView::updateSelectedIndices()
{
	m_selectedIndices.clear();

	double left = std::min(m_selectAreaStartPosition.x(), m_selectAreaEndPosition.x());
	double right = std::max(m_selectAreaStartPosition.x(), m_selectAreaEndPosition.x());
	double top = std::min(m_selectAreaStartPosition.y(), m_selectAreaEndPosition.y());
	double bottom = std::max(m_selectAreaStartPosition.y(), m_selectAreaEndPosition.y());

	std::unordered_map<GridAttributeDisplaySettingContainer*, DrawRegionInformation> colorMapRegions;
	std::unordered_map<GridAttributeDisplaySettingContainer*, DrawRegionInformation> independentChartRegions;
	DrawRegionInformation elevationChartRegion;

	setupRegions(m_impl->m_displaySettings, &colorMapRegions, &independentChartRegions, &elevationChartRegion);

	auto m = matrix(elevationChartRegion.yMax);

	auto nodePositions = setupNodePositions();

	std::vector<GridAttributeDisplaySettingContainer*> activeSettings;
	for (auto& s : m_impl->m_displaySettings) {
		if (! s.visible) {continue;}

		activeSettings.push_back(&s);
	}

	const auto& model = m_impl->m_editTableController->model();
	if (model.rowCount() == 0) {return;}
	if (model.rowCount() != nodePositions.size()) {return;}

	for (auto it = activeSettings.rbegin(); it != activeSettings.rend(); ++it) {
		auto setting = *it;
		unsigned int column = static_cast<int> (activeSettings.size()) - 1 - (it - activeSettings.rbegin());

		if (setting->mode == GridAttributeDisplaySettingContainer::Mode::AsElevation) {
			std::vector<QVariant> values;
			for (int i = 0; i < model.rowCount(); ++i) {
				values.push_back(model.data(model.index(i, column), Qt::EditRole));
			}

			std::vector<unsigned int> selected;
			for (int i = 0; i < nodePositions.size(); ++i) {
				auto p = m.map(QPointF(nodePositions.at(i), values.at(i).toDouble()));
				if (p.x() >= left && p.x() <= right && p.y() >= top && p.y() <= bottom) {
					selected.push_back(i);
				}
			}
			if (selected.size() > 0) {
				m_selectedIndices.insert({column, selected});
			}
		}
	}
}
void AbstractCrosssectionWindow::GraphicsView::pushElevationEditCommand()
{
	std::unordered_map<GridAttributeDisplaySettingContainer*, DrawRegionInformation> colorMapRegions;
	std::unordered_map<GridAttributeDisplaySettingContainer*, DrawRegionInformation> independentChartRegions;
	DrawRegionInformation elevationChartRegion;

	setupRegions(m_impl->m_displaySettings, &colorMapRegions, &independentChartRegions, &elevationChartRegion);
	auto m = matrix(elevationChartRegion.yMax);
	double scale = 1 / m.m22();
	double offset = scale * m_elevationOffset;

	std::vector<GridAttributeDisplaySettingContainer*> activeSettings;
	for (auto& s : m_impl->m_displaySettings) {
		if (! s.visible) {continue;}

		activeSettings.push_back(&s);
	}

	v4InputGrid* grid = m_gridDataItem->grid();
	auto sgrid = dynamic_cast<v4Structured2dGrid*> (grid->grid());

	auto command = new MergeSupportedListCommand(0, false);
	for (const auto& pair : m_selectedIndices) {
		auto setting = activeSettings.at(pair.first);
		auto att = grid->attribute(setting->attributeName());
		auto realAtt = dynamic_cast<GridAttributeRealContainer*> (att);
		if (realAtt == nullptr) {continue;}

		auto def = att->gridAttribute();
		auto pos = def->position();
		auto c = m_impl->m_controller;

		vtkDataSetAttributes* atts = nullptr;

		auto oldData = realAtt->dataArrayCopy();
		auto newData = realAtt->dataArrayCopy();

		vtkIdType idx = 0;
		int i, j;

		if (pos == SolverDefinitionGridAttribute::Position::Node) {
			atts = grid->grid()->vtkData()->data()->GetPointData();
			for (unsigned int index : pair.second) {
				if (c->targetDirection() == Direction::I) {
					i = c->targetIndex();
					j = index;
				} else {
					i = index;
					j = c->targetIndex();
				}
				idx = i + sgrid->dimensionI() * j;
				auto v = newData->GetValue(idx);
				v += offset;
				newData->SetValue(idx, v);
			}
		} else if (pos == SolverDefinitionGridAttribute::Position::CellCenter) {
			atts = grid->grid()->vtkData()->data()->GetCellData();
			for (unsigned int index : pair.second) {
				if (c->targetDirection() == Direction::I) {
					if (c->cellSide() == Controller::CellSide::Previous) {
						i = c->targetIndex() - 1;
					} else {
						i = c->targetIndex();
					}
					j = index - 1;
				} else {
					i = index - 1;
					if (c->cellSide() == Controller::CellSide::Previous) {
						j = c->targetIndex() - 1;
					} else {
						j = c->targetIndex();
					}
				}
				idx = i + (sgrid->dimensionI() - 1) * j;
				auto v = newData->GetValue(idx);
				v += offset;
				newData->SetValue(idx, v);
			}
		}

		auto com = new GridAttributeEditCommand(setting->attributeName(), newData, oldData, atts, m_gridDataItem);
		command->addCommand(com);
	}
	iRICUndoStack::instance().push(command);
}

QMatrix AbstractCrosssectionWindow::GraphicsView::matrix(int chartHeight) const
{
	int xReverse = 1;

	if (m_impl->m_displaySetting.reverseXDirection) {
		xReverse = -1;
	}

	QMatrix translate1(1, 0, 0, 1, - m_center.x(), - m_center.y());
	QMatrix scale(- m_scaleX * xReverse, 0, 0, - m_scaleY, 0, 0);
	QMatrix translate2(1, 0, 0, 1, viewport()->width() * 0.5, chartHeight * 0.5);

	return translate1 * scale * translate2;
}

