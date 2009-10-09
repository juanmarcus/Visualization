/*
 * QPixmapFactory.cpp
 *
 *  Created on: Oct 9, 2009
 *      Author: juanmarcus
 */

#include "QPixmapFactory.h"
#include "error/Exception.h"

QPixmapFactory::QPixmapFactory()
{
	// TODO Auto-generated constructor stub

}

QPixmapFactory::~QPixmapFactory()
{
	// TODO Auto-generated destructor stub
}

QPixmap* QPixmapFactory::createPixmap(Nrrd* nin)
{
	if (nin->dim != 2)
	{
		throw Exception("Nrrd dimension must be exactly 2");
	}
	unsigned int size = nin->axis[0].size * nin->axis[1].size;
	QPixmap* qpixmap = new QPixmap();
	if (!qpixmap->loadFromData((uchar*) nin->data, size, "RAW"))
	{
		throw Exception("Could not create QPixmap from Nrrd data.");
	}
	return qpixmap;
}
