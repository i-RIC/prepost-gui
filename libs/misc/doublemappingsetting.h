#ifndef DOUBLEMAPPINGSETTING_H
#define DOUBLEMAPPINGSETTING_H

#include <QList>

/// Mapping setting container for double-precision attributes
class DoubleMappingSetting
{

public:
	/// Target node (or cell) index
	unsigned int target;

	/// Mapping source indices
	QList<unsigned int> indices;
	/// Weights
	QList<double> weights;

};

#endif // DOUBLEMAPPINGSETTING_H

