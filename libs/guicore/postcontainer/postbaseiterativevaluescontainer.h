#ifndef POSTBASEITERATIVEVALUESCONTAINER_H
#define POSTBASEITERATIVEVALUESCONTAINER_H

#include <vector>

class ProjectData;

class QString;

class PostBaseIterativeValuesContainer
{
public:
	PostBaseIterativeValuesContainer(ProjectData* projectData);
	~PostBaseIterativeValuesContainer();

	QString filename() const;

	void load();
	bool save();
	void updateIfNeeded(int *progress, int *invalidId);

	class BaseContainer;

	const std::vector<BaseContainer*>& baseContainers() const;
	std::vector<BaseContainer*>& baseContainers();
	BaseContainer* baseContainer(int baseId) const;
	static QString filename(const QString workdir);

	template<class V> class ValueContainerT;
	class IntegerValueContainer;
	class RealValueContainer;
	class ValueContainer;

private:
	void rebuild(int* progress, int *invalidId);

	void setupContainers();
	bool addValuesFor(int solId);
	void clear();

	int resultCgnsCount() const;
	int valueCount() const;

	class IO;
	class TextIO;
	class BinaryIO;

	class Impl;
	std::unique_ptr<Impl> impl;
};

#ifdef _DEBUG
	#include "private/postbaseiterativevaluescontainer_impl.h"
#endif // _DEBUG

#endif // POSTBASEITERATIVEVALUESCONTAINER_H
