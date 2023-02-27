#ifndef POST2DWINDOWPOINTSCALARGROUPTOPDATAITEMI_H
#define POST2DWINDOWPOINTSCALARGROUPTOPDATAITEMI_H

class Post2dWindowZoneDataItem;

class Post2dWindowPointScalarGroupTopDataItemI
{
public:
	virtual ~Post2dWindowPointScalarGroupTopDataItemI() {}

	virtual Post2dWindowZoneDataItem* zoneDataItem() const = 0;
};

#endif // POST2DWINDOWPOINTSCALARGROUPTOPDATAITEMI_H
