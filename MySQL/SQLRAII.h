#pragma once

#include "MySQLPool.h"

class SQLRAII{
    public:
        SQLRAII(MYSQL** con, MySQLPool* pool);
        ~SQLRAII();

    private:
        MYSQL* m_con;
        MySQLPool* m_pool;
};