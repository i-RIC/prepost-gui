#ifndef TMSIMAGECACHEITEM_H
#define TMSIMAGECACHEITEM_H

class GDALDataset;

class QPixmap;

class TmsImageCacheItem
{
public:
	TmsImageCacheItem(int z, int x, int y, const QPixmap& image, bool lonLat);
	TmsImageCacheItem(int z, int x, int y, const QPixmap& original, const QPixmap& wgs84, QRectF wgs84Rect);
	~TmsImageCacheItem();

	QPixmap original() const;
	QPixmap wgs84();
	QRectF wgs84Rect() const;

private:
	void setupWgs84();

	int m_z;
	int m_x;
	int m_y;

	QPixmap m_original;
	QPixmap m_wgs84;
	QRectF m_wgs84Rect;
};

#endif // TMSIMAGECACHEITEM_H
