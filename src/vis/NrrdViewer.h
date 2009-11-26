#ifndef NRRDVIEWER_H
#define NRRDVIEWER_H

#include "RaycastingViewer.h"

class NrrdViewer : public RaycastingViewer
{
    Q_OBJECT

public:
    NrrdViewer(QWidget *parent = 0);
    ~NrrdViewer();

};

#endif // NRRDVIEWER_H
