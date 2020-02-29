#ifndef SCALARBARI_H
#define SCALARBARI_H

#include <guibase/scalarbarsetting.h>

class ScalarbarI
{
public:
    virtual ScalarBarSetting::Quadrant quadrant() const = 0;
};

#endif // SCALARBARI_H
