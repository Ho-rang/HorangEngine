#include "pch.h"
#include "DBConnectionPool.h"

using namespace Horang;

/*
	DBConnectionPool
*/

DBConnectionPool::DBConnectionPool()
{

}

DBConnectionPool::~DBConnectionPool()
{
	this->Clear();
}

bool DBConnectionPool::Connect(int32 connectionCount, const WCHAR* connectionString)
{
	WRITE_LOCK;

	if (::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &_environment) != SQL_SUCCESS)
		return false;

	if (::SQLSetEnvAttr(_environment, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), 0) != SQL_SUCCESS)
		return false;

	for (int32 i = 0; i < connectionCount; i++)
	{
		auto connection = xnew<DBConnection>();

		if (connection->Connect(_environment, connectionString) == false)
			return false;

		_connections.push_back(connection);
	}

	return true;
}

void DBConnectionPool::Clear()
{
	WRITE_LOCK;

	if (_environment != SQL_NULL_HANDLE)
	{
		::SQLFreeHandle(SQL_HANDLE_ENV, _environment);
		_environment = SQL_NULL_HANDLE;
	}

	for (auto connection : _connections)
		xdelete(connection);

	_connections.clear();
}

DBConnection* DBConnectionPool::Pop()
{
	WRITE_LOCK;

	if (_connections.empty() == true)
		return nullptr;


	auto connection = _connections.back();
	_connections.pop_back();

	return connection;
}

void DBConnectionPool::Push(DBConnection* connection)
{
	WRITE_LOCK;

	_connections.push_back(connection);
}
