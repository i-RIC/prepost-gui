#ifndef PREPROCESSORGRIDDATAITEMFACTORY_H
#define PREPROCESSORGRIDDATAITEMFACTORY_H

class Grid;
class PreProcessorDataItem;
class PreProcessorGridDataItem;

class PreProcessorGridDataItemFactory
{
public:
	static PreProcessorGridDataItem* factory(Grid* grid, PreProcessorDataItem* parent);

private:
	PreProcessorGridDataItemFactory();
};

#endif // PREPROCESSORGRIDDATAITEMFACTORY_H
