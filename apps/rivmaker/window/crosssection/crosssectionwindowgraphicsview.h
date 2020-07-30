#ifndef CROSSSECTIONWINDOWGRAPHICSVIEW_H
#define CROSSSECTIONWINDOWGRAPHICSVIEW_H

#include "crosssectionwindowdisplaysetting.h"
#include "../../widgets/chartgraphicsview.h"

class CrossSectionWindow;

class CrossSectionWindowGraphicsView : public ChartGraphicsView
{
public:
	CrossSectionWindowGraphicsView(QWidget* parent);

	const CrossSectionWindowDisplaySetting& crossSectionWindowDisplaySetting() const;
	void setCrossSectionWindowDisplaySetting(const CrossSectionWindowDisplaySetting& setting);

	void paintEvent(QPaintEvent* event) override;
	void draw(QPainter* painter);

private:
	void drawHWMs(QPainter* painter, const QMatrix& matrix);
	void drawLines(QPainter* painter, const QMatrix& matrix);
	void drawCircles(QPainter* painter, const QMatrix& matrix);

	CrossSectionWindow* chartWindow() const;
	QRectF getRegion() override;

	CrossSectionWindowDisplaySetting m_crossSectionWindowDisplaySetting;
};

#endif // CROSSSECTIONWINDOWGRAPHICSVIEW_H
