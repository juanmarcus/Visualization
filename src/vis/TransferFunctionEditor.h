#ifndef TRANSFERFUNCTIONEDITOR_H
#define TRANSFERFUNCTIONEDITOR_H

#include "ibi_qt/ibiQGLViewer.h"

using namespace ibi;

class TransferFunctionEditor: public ibiQGLViewer
{
Q_OBJECT

public:
	TransferFunctionEditor(QWidget *parent = 0);
	~TransferFunctionEditor();

	void init();
	void draw();

};

#endif // TRANSFERFUNCTIONEDITOR_H
