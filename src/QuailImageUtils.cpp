/**
 * @file QQuailImageUtils.h Image utility functions
 *
 * @Copyright (C) 2003-2004 Christian Hammond.
 * @Copyright (C) 2013      Phil Hannent.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA  02111-1307  USA
 */
#include "QuailImageUtils.h"

#include <QDebug>
#include <QPixmapCache>

//TODO: Re-implement using Nokia's method https://www.developer.nokia.com/Community/Wiki/Image_editing_techniques_and_algorithms_using_Qt

QImage &
quail_image_utils::greyImage(QImage &image)
{
	int pixels = (image.depth() > 8
				  ? image.width() * image.height()
                  : image.colorCount());
	unsigned int *data = (image.depth() > 8
						  ? (unsigned int *)image.bits()
                          : (unsigned int *)image.colorTable().data());
	int val, alpha, i;

	for (i = 0; i < pixels; i++)
	{
		val = qGray(data[i]);
		alpha = qAlpha(data[i]);
		data[i] = qRgba(val, val, val, alpha);
	}

	return image;
}

QImage &
quail_image_utils::desaturate(QImage &image, float value)
{
	if (image.width() == 0 || image.height() == 0)
		return image;

	if (value < 0) value = 0.0;
	if (value > 1) value = 1.0;

	int pixels = (image.depth() > 8
				  ? image.width() * image.height()
                  : image.colorCount());
	unsigned int *data = (image.depth() > 8
						  ? (unsigned int *)image.bits()
                          : (unsigned int *)image.colorTable().data());

	QColor color;
	int h, s, v, i;

	for (i = 0; i < pixels; i++)
	{
		color.setRgb(data[i]);
        color.getHsv(&h, &s, &v);
		color.setHsv(h, (int)(s * (1.0 - value)), v);

		data[i] = qRgba(color.red(), color.green(), color.blue(),
						qAlpha(data[i]));
	}

	return image;
}

QImage &
quail_image_utils::saturate(QImage &image, float value)
{
	if (image.width() == 0 || image.height() == 0)
		return image;

	if (value < 0) value = 0.0;
	if (value > 1) value = 1.0;

	int pixels = (image.depth() > 8
				  ? image.width() * image.height()
                  : image.colorCount());
	unsigned int *data = (image.depth() > 8
						  ? (unsigned int *)image.bits()
                          : (unsigned int *)image.colorTable().data());

	for (int i = 0; i < pixels; i++)
	{
		int intensity = (int)(qRed(data[i])   * 0.30 +
							  qGreen(data[i]) * 0.59 +
							  qBlue(data[i])  * 0.11);

		int sRed   = (int)((1.0 - value) * intensity + value * qRed(data[i]));
		int sGreen = (int)((1.0 - value) * intensity + value * qGreen(data[i]));
		int sBlue  = (int)((1.0 - value) * intensity + value * qBlue(data[i]));

		data[i] = qRgba(sRed, sGreen, sBlue, qAlpha(data[i]));
	}

	return image;
}

QPixmap
quail_image_utils::greyPixmap(QPixmap pixmap, QString name)
{
    qDebug() << "QQuailImageUtils::greyPixmap:" << name;
    QPixmap newPixmap = pixmap.copy();
    if (!QPixmapCache::find(name + "-greyed", newPixmap) || name.isEmpty()) {
        QImage image = newPixmap.toImage();

        greyImage(image);

        newPixmap.convertFromImage(image);
        QPixmapCache::insert(name + "-greyed", newPixmap);
    }
    return newPixmap;
}

QPixmap
quail_image_utils::saturate(QPixmap pixmap, float value, QString name)
{
    qDebug() << "QQuailImageUtils::saturate:" << name;
    QPixmap newPixmap = pixmap;
    if (!QPixmapCache::find(name + QString("-%1").arg(value), pixmap) || name.isEmpty()) {
        QImage image = newPixmap.toImage();

        saturate(image, value);

        pixmap.convertFromImage(image);
        QPixmapCache::insert(name + QString("-%1").arg(value), pixmap);
    }
    return pixmap;
}

void
quail_image_utils::blendOnLower(int xOffset, int yOffset,
                               QImage &upper,
							  QImage &lower)
{
	int upperWidth  = upper.width();
	int upperHeight = upper.height();
	int lowerWidth  = lower.width();
	int lowerHeight = lower.height();
	int width  = upperWidth;
	int height = upperHeight;
	int upperXOffset = 0;
	int upperYOffset = 0;
	int alpha;

	if (xOffset + width  > lowerWidth  ||
		yOffset + height > lowerHeight ||
		xOffset < 0 || yOffset < 0)
	{
		if (xOffset > lowerWidth || yOffset > lowerHeight)
			return;

		if (xOffset < 0)
		{
			upperXOffset = -xOffset;
			width += xOffset;
			xOffset = 0;
		}

		if (xOffset + width > lowerWidth)
			width = lowerWidth - xOffset;

		if (yOffset < 0)
		{
			upperYOffset = -yOffset;
			height += yOffset;
			yOffset = 0;
		}

		if (yOffset + height > lowerHeight)
			height = lowerHeight - yOffset;

		if (upperXOffset > upperWidth || upperYOffset >= upperHeight)
			return;

		if (width <= 0 || height <= 0)
			return;
	}

	for (int y = 0; y < height; y++)
	{
        uchar *upperLine = upper.scanLine(y + upperYOffset) +
            ((width + upperXOffset) << 2) - 1;
		uchar *lowerLine = lower.scanLine(y + yOffset) +
			((width + xOffset) << 2) - 1;

		int x = width - 1;

		do
		{
#ifdef WORDS_BIGENDIAN
			while (!(alpha = *(upperLine - 3)) && x > 0)
#else
			while (!(alpha = *upperLine) && x > 0)
#endif
			{
				upperLine -= 4;
				lowerLine -= 4;
				x--;
			}

#ifndef WORDS_BIGENDIAN
			*lowerLine = 255 - (((255 - *upperLine) *
								 (255 - *lowerLine) + 255) >> 8);
			upperLine--;
			lowerLine--;
#endif

			*lowerLine += (((*upperLine - *lowerLine) * alpha) >> 8);
			upperLine--;
			lowerLine--;

			*lowerLine += (((*upperLine - *lowerLine) * alpha) >> 8);
			upperLine--;
			lowerLine--;

			*lowerLine += (((*upperLine - *lowerLine) * alpha) >> 8);
			upperLine--;
			lowerLine--;

#ifdef WORDS_BIGENDIAN
			*lowerLine = 255 - (((255 - *upperLine) *
								 (255 - *lowerLine) + 255) >> 8);
			upperLine--;
			lowerLine--;
#endif

			x--;
		}
		while (x >= 0);
	}
}
