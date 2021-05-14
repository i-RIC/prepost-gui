#ifndef POSTBASEITERATIVEDATACONTAINER_H
#define POSTBASEITERATIVEDATACONTAINER_H

#include "postdatacontainer.h"

#include <string>

namespace iRICLib {
	class H5CgnsFile;
} // namespace iRICLib

class PostBaseIterativeDataContainer : public PostDataContainer
{
public:
	PostBaseIterativeDataContainer(const std::string& name, PostSolutionInfo* parent);

	const std::string& name() const;

	virtual bool handleCurrentStepUpdate(iRICLib::H5CgnsFile* file, const int timeStep) = 0;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	std::string m_name;
};

#endif // POSTBASEITERATIVEDATACONTAINER_H
