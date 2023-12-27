#ifndef IMAGESETTINGCONTAINER_IMAGEBUILDER_H
#define IMAGESETTINGCONTAINER_IMAGEBUILDER_H

#include "../imagesettingcontainer.h"

class GUICOREDLL_EXPORT ImageSettingContainer::ImageBuilder
{
public:
	ImageBuilder();
	virtual ~ImageBuilder();

	virtual bool build(QImage* image);
	virtual QSize autoSize() const = 0;
};

#endif // IMAGESETTINGCONTAINER_IMAGEBUILDER_H
