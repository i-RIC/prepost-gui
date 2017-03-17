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

	Mode m_mode;
	int m_movingPointIndex = 0;
};

#endif // CROSSSECTIONPREPROCESSORCONTROLLER_IMPL_H
