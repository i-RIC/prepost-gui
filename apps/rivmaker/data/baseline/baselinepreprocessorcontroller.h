#ifndef BASELINEPREPROCESSORCONTROLLER_H
#define BASELINEPREPROCESSORCONTROLLER_H

#include "baseline.h"
#include "../base/dataitemcontroller.h"

class BaseLinePreProcessorController : public DataItemController
{
	Q_OBJECT

public:
	BaseLinePreProcessorController(Model* model, BaseLine* item);
	~BaseLinePreProcessorController();

	void keyPressEvent(QKeyEvent*, View* v) override;
	void mouseDoubleClickEvent(QMouseEvent*, View* v) override;
	void mouseMoveEvent(QMouseEvent*, View* v) override;
	void mousePressEvent(QMouseEvent*, View* v) override;
	void mouseReleaseEvent(QMouseEvent*, View* v) override;

	void updateMode();

	virtual void setupObjectBrowserRightClickMenu(QMenu* menu) override;
	virtual void setupViewRightClickMenu(QMenu* menu) override;

public slots:
	void importData();
	void exportData();

	void addVertex();
	void removeVertex();
	void editCoordinates();
	void reverseDirection();

	void clear();

private:
	void finishDefining();
	void updateMouseCursor(View* v);

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/baselinepreprocessorcontroller_impl.h"
#endif // _DEBUG

#endif // BASELINEPREPROCESSORCONTROLLER_H
