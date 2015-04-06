#ifndef DOUBLEMAPPINGSETTING_H
#define DOUBLEMAPPINGSETTING_H

#include <QList>

class DoubleMappingSetting
{
public:
	unsigned int target;

	QList<unsigned int> indices;
	QList<double> weights;

};

#endif // DOUBLEMAPPINGSETTING_H

