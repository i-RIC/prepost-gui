#ifndef POSTBASEITERATIVESERIESDATACONTAINER_H
#define POSTBASEITERATIVESERIESDATACONTAINER_H

#include "../guicore_global.h"
#include "postseriesdatacontainer.h"
#include "postsolutioninfo.h"
#include <QString>

class GUICOREDLL_EXPORT PostBaseIterativeSeriesDataContainer : public PostSeriesDataContainer
{
public:
	PostBaseIterativeSeriesDataContainer(PostSolutionInfo::Dimension dim, const QString& biName, ProjectDataItem* parent);
protected:
	bool loadData(const int fn);
	void doLoadFromProjectMainFile(const QDomNode& /*node*/){}
	void doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/){}
protected:
	QString m_baseIterativeName;
};

#endif // POSTBASEITERATIVESERIESDATACONTAINER_H
