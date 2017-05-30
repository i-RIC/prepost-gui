#ifndef CROSSSECTIONPREPROCESSORCONTROLLER_H
#define CROSSSECTIONPREPROCESSORCONTROLLER_H

#include "crosssection.h"
#include "../base/dataitemcontroller.h"

class CrossSectionPreProcessorController : public DataItemController
{
	Q_OBJECT

public:
	CrossSectionPreProcessorController(Model* model, CrossSection* item);
	~CrossSectionPreProcessorController();

	void mouseMoveEvent(QMouseEvent* event, View* v) override;
	void mousePressEvent(QMouseEvent* event, View* v) override;
	void mouseReleaseEvent(QMouseEvent*, View *v) override;

	void setupViewRightClickMenu(QMenu* menu) override;

	void handleStandardItemChange(QStandardItem* item) override;

public slots:
	void editCoordinates();

private:
	void finishDefining();
	void updateMouseCursor(View* view);

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/crosssectionpreprocessorcontroller_impl.h"
#endif // _DEBUG

#endif // CROSSSECTIONPREPROCESSORCONTROLLER_H
