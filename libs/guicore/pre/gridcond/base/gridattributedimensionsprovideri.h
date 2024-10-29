#ifndef GRIDATTRIBUTEDIMENSIONSPROVIDERI_H
#define GRIDATTRIBUTEDIMENSIONSPROVIDERI_H

#include <string>

class GridAttributeDimensionsContainer;

class GridAttributeDimensionsProviderI
{
public:
	virtual ~GridAttributeDimensionsProviderI() {};

	virtual GridAttributeDimensionsContainer* gridAttributeDimensions(const std::string& attName) const = 0;
};

#endif // GRIDATTRIBUTEDIMENSIONSPROVIDERI_H
