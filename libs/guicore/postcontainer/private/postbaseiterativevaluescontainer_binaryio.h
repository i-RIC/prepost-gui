#ifndef POSTBASEITERATIVEVALUESCONTAINER_BINARYIO_H
#define POSTBASEITERATIVEVALUESCONTAINER_BINARYIO_H

#include "postbaseiterativevaluescontainer_io.h"

class PostBaseIterativeValuesContainer::BinaryIO : public PostBaseIterativeValuesContainer::IO
{
public:
	BinaryIO(PostBaseIterativeValuesContainer* container);

	void load() override;
	bool save() override;
};

#endif // POSTBASEITERATIVEVALUESCONTAINER_BINARYIO_H
