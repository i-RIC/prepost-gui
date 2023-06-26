#ifndef POSTBASEITERATIVEVALUESCONTAINER_IO_H
#define POSTBASEITERATIVEVALUESCONTAINER_IO_H

#include "../postbaseiterativevaluescontainer.h"

#include <QFile>

class PostBaseIterativeValuesContainer::IO
{
public:
	IO(PostBaseIterativeValuesContainer* container);
	virtual ~IO();

	QString filename() const;
	static QString filename(const QString& workdir);

	virtual void load() = 0;
	virtual bool save() = 0;

protected:
	PostBaseIterativeValuesContainer* m_container;
	QFile m_file;
};

#endif // POSTBASEITERATIVEVALUESCONTAINER_IO_H
