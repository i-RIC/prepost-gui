#ifndef MODEL_H
#define MODEL_H

#include <QObject>

class DataItem;
class DataItemController;
class DataItemView;
class View;

class Model : public QObject
{
public:
	Model(QObject* parent);
	virtual ~Model();

	View* view() const;
	void setView(View* view);

	DataItem* selectedItem() const;
	DataItemController* selectedItemController() const;

	bool shouldDraw(DataItem* item) const;

	DataItemView* dataItemView(DataItem* item) const;
	DataItemController* dataItemController(DataItem* item) const;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/model_impl.h"
#endif // _DEBUG

#endif // MODEL_H
