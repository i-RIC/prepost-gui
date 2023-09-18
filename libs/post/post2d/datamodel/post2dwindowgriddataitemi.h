#ifndef POST2DWINDOWGRIDDATAITEMI_H
#define POST2DWINDOWGRIDDATAITEMI_H

#include "../post2dwindowdataitem.h"

class v4Grid2d;

class Post2dWindowGridDataItemI : public Post2dWindowDataItem
{
public:
	Post2dWindowGridDataItemI(const QString& caption, Post2dWindowDataItem* parent);

	virtual v4Grid2d* grid() const = 0;
};

#endif // POST2DWINDOWGRIDDATAITEMI_H
