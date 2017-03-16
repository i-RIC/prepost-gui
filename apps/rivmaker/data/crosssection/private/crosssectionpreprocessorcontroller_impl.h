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
	};

	Mode m_mode;
};

#endif // CROSSSECTIONPREPROCESSORCONTROLLER_IMPL_H
