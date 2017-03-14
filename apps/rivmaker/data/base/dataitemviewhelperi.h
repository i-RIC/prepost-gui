#ifndef DATAITEMVIEWHELPERI_H
#define DATAITEMVIEWHELPERI_H

class DataItem;
class DataItemView;
class Model;
class View;

class QPainter;

class DataItemViewHelperI
{
public:
	DataItemViewHelperI(DataItemView* dataItemView);
	virtual ~DataItemViewHelperI();

	virtual bool prepareDraw();
	virtual void draw(QPainter* painter) const = 0;

protected:
	DataItem* dataItem() const;
	Model* model() const;
	View* view() const;

private:
	DataItemView* m_dataItemView;
};

#endif // DATAITEMVIEWHELPERI_H
