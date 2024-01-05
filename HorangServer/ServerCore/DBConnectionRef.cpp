#include "pch.h"
#include "DBConnectionRef.h"
#include "DBConnectionPool.h"

DBConnectionRef::DBConnectionRef()
	: _dbConnection(GDBConnectionPool->Pop())
{

}

DBConnectionRef::~DBConnectionRef()
{
	if (this->_dbConnection != nullptr)
		GDBConnectionPool->Push(this->_dbConnection);
}

DBConnection DBConnectionRef::operator*() const
{
	return *_dbConnection;
}
