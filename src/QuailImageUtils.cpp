/**
 * @file QGaimImageUtils.h Image utility functions
 *
 * @Copyright (C) 2003-2004 Christian Hammond.
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

#include <libpurple/debug.h>

#include <glib.h>

QImage &
QGaimImageUtils::greyImage(QImage &image)
{
	int pixels = (image.depth() > 8
				  ? image.width() * image.height()
				  : image.numColors());
	unsigned int *data = (image.depth() > 8
						  ? (unsigned int *)image.bits()
						  : (unsigned int *)image.colorTable());
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
QGaimImageUtils::desaturate(QImage &image, float value)
{
	if (image.width() == 0 || image.height() == 0)
		return image;

	if (value < 0) value = 0.0;
	if (value > 1) value = 1.0;

	int pixels = (image.depth() > 8
				  ? image.width() * image.height()
				  : image.numColors());
	unsigned int *data = (image.depth() > 8
						  ? (unsigned int *)image.bits()
						  : (unsigned int *)image.colorTable());

	QColor color;
	int h, s, v, i;

	for (i = 0; i < pixels; i++)
	{
		color.setRgb(data[i]);
		color.hsv(&h, &s, &v);
		color.setHsv(h, (int)(s * (1.0 - value)), v);

		data[i] = qRgba(color.red(), color.green(), color.blue(),
						qAlpha(data[i]));
	}

	return image;
}

QImage &
QGaimImageUtils::saturate(QImage &image, float value)
{
	if (image.width() == 0 || image.height() == 0)
		return image;

	if (value < 0) value = 0.0;
	if (value > 1) value = 1.0;

	int pixels = (image.depth() > 8
				  ? image.width() * image.height()
				  : image.numColors());
	unsigned int *data = (image.depth() > 8
						  ? (unsigned int *)image.bits()
						  : (unsigned int *)image.colorTable());

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

QPixmap &
QGaimImageUtils::greyPixmap(QPixmap &pixmap)
{
	QImage image;

	image = pixmap.convertToImage();

	greyImage(image);

	pixmap.convertFromImage(image);

	return pixmap;
}

QPixmap &
QGaimImageUtils::saturate(QPixmap &pixmap, float value)
{
	QImage image;

	image = pixmap.convertToImage();

	saturate(image, value);

	pixmap.convertFromImage(image);

	return pixmap;
}

void
QGaimImageUtils::blendOnLower(int xOffset, int yOffset, const QImage &upper,
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
