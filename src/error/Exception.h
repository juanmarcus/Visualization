/*
 * Exception.h
 *
 *  Created on: Oct 9, 2009
 *      Author: juanmarcus
 */

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <exception>
#include <string>

class Exception: public std::exception
{
public:
	Exception(std::string msg);

	~Exception() throw ();

	virtual const char* what() const throw ()
	{
		return msg.c_str();
	}
private:
	std::string msg;

};

#endif /* EXCEPTION_H_ */
