#ifndef ANIMATIONCONTROLLERINTERFACE_H
#define ANIMATIONCONTROLLERINTERFACE_H

#include <QObject>

#include "../guicore_global.h"

class GUICOREDLL_EXPORT AnimationControllerInterface : public QObject
{
	Q_OBJECT

public slots:
	virtual void setCurrentStepIndex(unsigned int i) = 0;
};

#endif // ANIMATIONCONTROLLERINTERFACE_H
