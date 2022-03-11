#include "MySQLPool.h"
#include "SQLRAII.h"
#include <string>
#include <iostream>



int main()
{
    printf("Version: %s\n",mysql_get_client_info());
    MySQLPool::GetInstance()->init("localhost",3306,"webserver","root","admin",30);
    MYSQL* con;
    SQLRAII(&con,MySQLPool::GetInstance());
    if(mysql_query(con,"INSERT INTO user(user_name, user_password) VALUES('123','123');"))
        printf("Query fail\n");
        /*
    MYSQL_RES* res = mysql_store_result(con);
    int line = mysql_num_fields(res);
    MYSQL_ROW row;
    while(row = mysql_fetch_row(res))
    {
        for(int i=0; i<line; i++)
        {
            printf("%s\n",row[i]);
        }
        printf("\n");
    }*/
    //mysql_free_result(res);
    //mysql_close(con);
}