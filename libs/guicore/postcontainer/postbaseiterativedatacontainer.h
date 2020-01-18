#ifndef POSTBASEITERATIVEDATACONTAINER_H
#define POSTBASEITERATIVEDATACONTAINER_H

#include "postdatacontainer.h"

#include <string>

class PostBaseIterativeDataContainer : public PostDataContainer
{
public:
	PostBaseIterativeDataContainer(int baseId, const std::string& name, PostSolutionInfo* parent);

	int baseId() const;
	const std::string& name() const;

protected:
	bool getNumArrays(const int fn, int* nArrays);

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	int m_baseId;
	std::string m_name;
};

#endif // POSTBASEITERATIVEDATACONTAINER_H
