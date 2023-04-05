#ifndef GEODATAPOINTMAP_TINMANAGER_TINBUILDER_H
#define GEODATAPOINTMAP_TINMANAGER_TINBUILDER_H

#include "geodatapointmap_tinmanager.h"

#include <QObject>

class GeoDataPointmap::TINManager::TINBuilder : public QObject
{
	Q_OBJECT

public:
	TINBuilder(TINManager* manager);

	bool build(bool allowCancel);

private slots:
	void cancel();

private:
	bool m_canceled;

	TINManager* m_manager;
};

#endif // GEODATAPOINTMAP_TINMANAGER_TINBUILDER_H
