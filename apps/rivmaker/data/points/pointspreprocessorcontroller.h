#ifndef POINTSPREPROCESSORCONTROLLER_H
#define POINTSPREPROCESSORCONTROLLER_H

#include "points.h"
#include "../base/dataitemcontroller.h"

class PointsPreProcessorController : public DataItemController
{
	Q_OBJECT

public:
	PointsPreProcessorController(Model* model, Points* item);
	~PointsPreProcessorController();

	void mouseMoveEvent(QMouseEvent* event, View* view) override;

	void setupObjectBrowserRightClickMenu(QMenu* menu) override;

public slots:
	void importData();
	void clearData();

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/pointspreprocessorcontroller_impl.h"
#endif // _DEBUG

#endif // POINTSPREPROCESSORCONTROLLER_H
