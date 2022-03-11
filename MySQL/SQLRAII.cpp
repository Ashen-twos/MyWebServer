#include "SQLRAII.h"

SQLRAII::SQLRAII(MYSQL** con, MySQLPool* pool)
{
    *con = pool->GetConnection();
    m_con = *con;
    m_pool = pool;
}

SQLRAII::~SQLRAII()
{
    if(m_con)
        m_pool->ReturnConnection(m_con);
}