#ifndef CHARTWINDOW_H
#define CHARTWINDOW_H

#include "../window/viewwindowi.h"

#include <QMainWindow>

class ChartGraphicsView;

class QCheckBox;
class QComboBox;
class QLineEdit;
class QToolBar;

class ChartWindow : public QMainWindow, public ViewWindowI
{
	Q_OBJECT

public:
	ChartWindow(QWidget* parent);

	void fit() override;
	void resetRotation() override;
	void rotate90() override;

	void zoomIn() override;
	void zoomInX() override;
	void zoomInY() override;
	void zoomOut() override;
	void zoomOutX() override;
	void zoomOutY() override;

	bool showGrid() const;
	bool showScales() const;
	bool showAspectRatio() const;

	bool fixAspectRatio() const;
	bool fixRegion() const;

	void setAspectRatio(double ratio);

signals:
	void positionChangedForStatusBar(const QPointF& point);

	public slots:
	void updateChart();

private slots:
	void handleAspectRatioChange(const QString& newValue);
	void updateAspectRatioStatus();

protected:
	void addToolBars();
	QToolBar* displayToolBar() const;
	QToolBar* viewToolBar() const;

	QCheckBox* m_fixAspectRatioCheckBox;

private:
	void addDisplayToolBar();
	void addViewToolBar();

	ChartGraphicsView* graphicsView();

	QToolBar* m_displayToolBar;
	QToolBar* m_viewToolBar;

	QCheckBox* m_showGridCheckBox;
	QCheckBox* m_showScalesCheckBox;
	QCheckBox* m_showAspectRatioCheckBox;
	QLineEdit* m_aspectRatioEdit;
	QCheckBox* m_fixRegionCheckBox;
};

#endif // CHARTWINDOW_H
