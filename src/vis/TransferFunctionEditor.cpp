#include "TransferFunctionEditor.h"

#include "QtGui/QKeyEvent"

TransferFunctionEditor::TransferFunctionEditor(QWidget *parent) :
	ibiQGLViewer(parent)
{
	createActions();
	createContextMenu();
}

TransferFunctionEditor::~TransferFunctionEditor()
{

}

void TransferFunctionEditor::createContextMenu()
{
	contextMenu = new QMenu(this);
	contextMenu->addAction(addPointAct);
}

void TransferFunctionEditor::createActions()
{
	addPointAct = new QAction(tr("Add point"), this);
	connect(addPointAct, SIGNAL(triggered()), this, SLOT(addPointSlot()));
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

void TransferFunctionEditor::keyPressEvent(QKeyEvent *e)
{
	// Get event modifiers key
	const Qt::KeyboardModifiers modifiers = e->modifiers();

	// A simple switch on e->key() is not sufficient if we want to take state key into account.
	// With a switch, it would have been impossible to separate 'F' from 'CTRL+F'.
	// That's why we use imbricated if...else and a "handled" boolean.
	bool handled = false;
	//	if ((e->key() == Qt::Key_W) && (modifiers == Qt::NoButton))
	//	{
	//		handled = true;
	//		updateGL();
	//	}

	if (!handled)
		QGLViewer::keyPressEvent(e);
}

void TransferFunctionEditor::mousePressEvent(QMouseEvent* e)
{
	if ((e->button() == Qt::RightButton) && (e->modifiers() == Qt::NoButton))
	{
		lastMouseClick = e->globalPos();
		QAction* action = contextMenu->exec(e->globalPos());

		//		if (action)
		//		{
		//			action->trigger();
		//		}
	}
	else
		QGLViewer::mousePressEvent(e);
}

void TransferFunctionEditor::addPointSlot()
{
	std::cout << lastMouseClick.x() << std::endl;
}
