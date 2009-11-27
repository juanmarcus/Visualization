#ifndef NRRDVIEWER_H
#define NRRDVIEWER_H

#include "RaycastingViewer.h"
#include "ibi_geometry/Ray.h"
#include "ibi_geometry/AxisAlignedBox.h"
#include "ibi_gl/GeometryDrawer.h"
#include "Sampler.h"

using namespace ibi;

class NrrdViewer: public RaycastingViewer
{
Q_OBJECT

public:
	NrrdViewer(QWidget *parent = 0);
	~NrrdViewer();

	void init();
	void draw();

public slots:
	void toggleRayQuerySlot();
	void toggleShowRayImageSlot();
	void toggleShowRayHistogramSlot();

private:
	void createActions();
private:
	// Configuration
	bool doRayQuery;
	int viewMode;

	// Querying
	Ray ray;
	AxisAlignedBox box;
	Sampler sampler;
	Texture* queryResult;

	// Drawing
	GeometryDrawer drawer;

	// Actions
	QAction* toggleRayQueryAct;
	QAction* toggleShowRayImageAct;
	QAction* toggleShowRayHistogramAct;
};

#endif // NRRDVIEWER_H
