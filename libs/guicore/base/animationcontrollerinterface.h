#ifndef ANIMATIONCONTROLLERINTERFACE_H
#define ANIMATIONCONTROLLERINTERFACE_H

#include "../guicore_global.h"

#include <QObject>

class GUICOREDLL_EXPORT AnimationControllerInterface : public QObject
{
	Q_OBJECT

public slots:
	virtual void setCurrentStepIndex(unsigned int i) = 0;
};

#endif // ANIMATIONCONTROLLERINTERFACE_H
