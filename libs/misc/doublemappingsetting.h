#ifndef DOUBLEMAPPINGSETTING_H
#define DOUBLEMAPPINGSETTING_H

#include <vector>

/// Mapping setting container for double-precision attributes
class DoubleMappingSetting
{
public:
	/// Target node (or cell) index
	unsigned int target;

	std::vector<unsigned int> indices;
	std::vector<double> weights;
};

#endif // DOUBLEMAPPINGSETTING_H
