/*
 * QPixmapFactory.h
 *
 *  Created on: Oct 9, 2009
 *      Author: juanmarcus
 */

#ifndef QPIXMAPFACTORY_H_
#define QPIXMAPFACTORY_H_

#include <teem/nrrd.h>
#include <QtGui/QPixmap>

class QPixmapFactory
{
public:
	QPixmapFactory();
	~QPixmapFactory();

	QPixmap* createPixmap(Nrrd* nin);
};

#endif /* QPIXMAPFACTORY_H_ */
