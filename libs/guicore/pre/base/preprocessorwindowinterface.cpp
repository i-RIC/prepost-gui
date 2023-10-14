#include "preprocessorwindowinterface.h"

PreProcessorWindowInterface::PreProcessorWindowInterface(QWidget* widget) :
	QMainWindowWithSnapshot {widget}
{}

PreProcessorWindowInterface::~PreProcessorWindowInterface()
{}
