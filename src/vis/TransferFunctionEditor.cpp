#include "TransferFunctionEditor.h"

TransferFunctionEditor::TransferFunctionEditor(QWidget *parent) :
	ibiQGLViewer(parent)
{

}

TransferFunctionEditor::~TransferFunctionEditor()
{

}

void TransferFunctionEditor::init()
{
	setDesiredAspectRatio(2.0);
}

void TransferFunctionEditor::draw()
{
	start2DMode();

	drawFullScreenQuad();

	stop2DMode();
}
