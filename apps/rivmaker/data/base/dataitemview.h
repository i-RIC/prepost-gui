#ifndef DATAITEMVIEW_H
#define DATAITEMVIEW_H

#include <vector>

class DataItem;
class Model;
class DataItemViewHelperI;
class View;

class QPainter;
class QRectF;
class QTransform;

class DataItemView
{
public:
	DataItemView(Model* model, DataItem* item);
	virtual ~DataItemView();

	void discardDrawCache();
	bool prepareDraw();
	virtual void draw(QPainter* painter) const;

	void addChildItem(DataItemView* v);
	void removeChildItem(DataItemView* v);

protected:
	virtual void doDraw(QPainter *painter) const;

	DataItem* item() const;
	View* view() const;
	Model* model() const;

	const std::vector<DataItemViewHelperI*>& viewHelpers() const;
	std::vector<DataItemViewHelperI*>& viewHelpers();

private:
	virtual void doDiscardDrawCache();
	virtual bool doPrepareDraw();

	class Impl;
	Impl* impl;

public:
	friend class DataItemViewHelperI;
};

#ifdef _DEBUG
	#include "private/dataitemview_impl.h"
#endif // _DEBUG

#endif // DATAITEMVIEW_H
