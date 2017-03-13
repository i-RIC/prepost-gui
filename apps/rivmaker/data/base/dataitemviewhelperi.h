#ifndef DATAITEMVIEWHELPERI_H
#define DATAITEMVIEWHELPERI_H

class DataItemView;

class DataItemViewHelperI
{
public:
	DataItemViewHelperI(DataItemView* dataItemView);
	virtual ~DataItemViewHelperI();

	virtual bool prepareDraw();
	virtual void draw(QPainter* painter) const = 0;

protected:
	View* view() const;

private:
	DataItemView* m_dataItemView;
};

#endif // DATAITEMVIEWHELPERI_H
