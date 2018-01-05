#ifndef GRIDATTRIBUTEGENERATORLAUNCHER_H
#define GRIDATTRIBUTEGENERATORLAUNCHER_H

#include <QObject>

#include <string>

class PreProcessorGridDataItem;
class QWidget;

class GridAttributeGeneratorLauncher : public QObject
{
	Q_OBJECT

public:
	static bool launchGenerator(PreProcessorGridDataItem* gItem, const std::string& attName, const QString& workDir, QWidget* parent);

private:
	GridAttributeGeneratorLauncher();
};

#endif // GRIDATTRIBUTEGENERATORLAUNCHER_H
