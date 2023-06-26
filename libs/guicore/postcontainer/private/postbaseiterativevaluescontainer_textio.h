#ifndef POSTBASEITERATIVEVALUESCONTAINER_TEXTIO_H
#define POSTBASEITERATIVEVALUESCONTAINER_TEXTIO_H

#include "postbaseiterativevaluescontainer_io.h"

class PostBaseIterativeValuesContainer::TextIO : public PostBaseIterativeValuesContainer::IO
{
public:
	TextIO(PostBaseIterativeValuesContainer* container);

	void load() override;
	bool save() override;
};

#endif // POSTBASEITERATIVEVALUESCONTAINER_TEXTIO_H
