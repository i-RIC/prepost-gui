#ifndef TOPVIEW_H
#define TOPVIEW_H

#include "view.h"

class TopView : public View
{
public:
	TopView(QWidget* parent);

private:
	void paramsFit() override;
	void paramsZoomIn() override;
	void paramsZoomOut() override;

	void paramsResetRotation() override;
	void paramsRotate90() override;

	void viewMouseMoveEvent(QMouseEvent* event) override;
	void viewMousePressEvent(QMouseEvent* event) override;
	void viewMouseReleaseEvent(QMouseEvent* event) override;
	void viewWheelEvent(QWheelEvent* event) override;

	bool inViewOperation() override;

	void updateTransform();

	QPointF m_centerPoint {0, 0};
	double m_scale {1};
	double m_angleDegree {0};

	bool m_zooming {false};
	bool m_translating {false};
	bool m_rotating {false};
};

#endif // TOPVIEW_H
