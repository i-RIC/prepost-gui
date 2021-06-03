#ifndef POSTBASEITERATIVESERIESDATACONTAINER_H
#define POSTBASEITERATIVESERIESDATACONTAINER_H

#include "../guicore_global.h"
#include "postseriesdatacontainer.h"
#include "postsolutioninfo.h"

#include <string>

class GUICOREDLL_EXPORT PostBaseIterativeSeriesDataContainer : public PostSeriesDataContainer
{
public:
	PostBaseIterativeSeriesDataContainer(const std::string& name, PostSolutionInfo* parent);

private:
	int loadData() override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	std::string m_baseIterativeName;
};

#endif // POSTBASEITERATIVESERIESDATACONTAINER_H
