#ifndef PREPROCESSORMODEL_H
#define PREPROCESSORMODEL_H

#include <QObject>

class Project;
class PreProcessorView;

class QStandardItemModel;

class PreProcessorModel : public QObject
{
	Q_OBJECT

public:
	PreProcessorModel(QObject* parent);
	~PreProcessorModel();

	void setProject(Project* project);
	void setView(PreProcessorView* view);

	QStandardItemModel* standardItemmodel() const;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/preprocessormodel_impl.h"
#endif // _DEBUG

#endif // PREPROCESSORMODEL_H
