/*
 * Exception.cpp
 *
 *  Created on: Oct 9, 2009
 *      Author: juanmarcus
 */

#include "Exception.h"

using namespace std;

Exception::Exception(string msg):
	exception()
{
	this->msg = msg;
}

Exception::~Exception() throw ()
{

}
