#ifndef CROSSSECTIONPREPROCESSORCONTROLLER_IMPL_H
#define CROSSSECTIONPREPROCESSORCONTROLLER_IMPL_H

#include "../crosssectionpreprocessorcontroller.h"

class CrossSectionPreProcessorController::Impl
{
public:
	enum class Mode {
		Normal,
		BeforeDefining,
		Defining,
		MovePoint,
		MovePointPrepare,
	};

	Impl();
	~Impl();

	Mode m_mode;
	int m_movingPointIndex;

	QAction* m_editCoordinatesAction;
};

#endif // CROSSSECTIONPREPROCESSORCONTROLLER_IMPL_H
