#ifndef ANIMATIONCONTROLLERI_H
#define ANIMATIONCONTROLLERI_H

#include "../guicore_global.h"

#include <QObject>

class GUICOREDLL_EXPORT AnimationControllerI : public QObject
{
	Q_OBJECT

public slots:
	virtual void setCurrentStepIndex(unsigned int i) = 0;
};

#endif // ANIMATIONCONTROLLERI_H
