#ifndef BASELINEPREPROCESSORCONTROLLER_H
#define BASELINEPREPROCESSORCONTROLLER_H

#include "baseline.h"
#include "../base/dataitemcontroller.h"

class BaseLinePreProcessorController : public DataItemController
{
public:
	BaseLinePreProcessorController(Model* model, BaseLine* item);
	~BaseLinePreProcessorController();

	void keyPressEvent(QKeyEvent*, View*) override;
	void mouseDoubleClickEvent(QMouseEvent*, View*) override;
	void mouseMoveEvent(QMouseEvent*, View*) override;
	void mousePressEvent(QMouseEvent*, View*) override;
	void mouseReleaseEvent(QMouseEvent*, View*) override;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/baselinepreprocessorcontroller_impl.h"
#endif // _DEBUG

#endif // BASELINEPREPROCESSORCONTROLLER_H
