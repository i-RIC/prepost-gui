#ifndef PREPROCESSORGRIDDATAITEMFACTORY_H
#define PREPROCESSORGRIDDATAITEMFACTORY_H

class v4InputGrid;
class PreProcessorDataItem;
class PreProcessorGridDataItem;

class PreProcessorGridDataItemFactory
{
public:
	static PreProcessorGridDataItem* factory(v4InputGrid* grid, PreProcessorDataItem* parent);

private:
	PreProcessorGridDataItemFactory();
};

#endif // PREPROCESSORGRIDDATAITEMFACTORY_H
