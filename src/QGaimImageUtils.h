/**
 * @file QGaimImageUtils.h Image utility functions
 *
 * @Copyright (C) 2003 Christian Hammond.
 *
 * Parts of this code Copyright (C) 2003 The KDE project.
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
#ifndef _QGAIM_IMAGE_UTILS_H_
#define _QGAIM_IMAGE_UTILS_H_

#include <qpixmap.h>
#include <qimage.h>

class QGaimImageUtils
{
	public:
		static QImage &greyImage(QImage &image);
		static QImage &desaturate(QImage &image, float value);
		static QImage &saturate(QImage &image, float value);
		static QPixmap &saturate(QPixmap &pixmap, float value);

		static void blendOnLower(int x, int y, const QImage &upper,
								 QImage &lower);
};

#endif /* _QGAIM_IMAGE_UTILS_H_ */
