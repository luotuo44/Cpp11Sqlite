//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license

#include <iostream>
#include<string>

#include"SqliteDB.h"

#include<sqlite3.h>

using namespace std;

int testBaseExecute()
{
    SqliteDB db("test.db");


    std::string sql = "create table if not exists student(id varchar(32) primary key, "
                      "name varchar(128), score int)";
    int ret = db.execute(sql);
    if( ret != SQLITE_OK )
    {
        std::cout<<"fail to create table, "<<db.errorMsg()<<std::endl;
        return -1;
    }

    sql = "insert into student(id, name, score) values(\"001\", \"xiaoming\", 99)";
    ret = db.execute(sql);
    if( ret != SQLITE_OK )
    {
        std::cout<<"fail to insert, "<<db.errorMsg()<<std::endl;
        return -1;
    }


    sql = "insert into student values('002', 'zhangsan', 80)";
    ret = db.execute(sql);
    if( ret != SQLITE_OK )
    {
        std::cout<<"fail to insert, "<<db.errorMsg()<<std::endl;
        return -1;
    }


    sql = "insert into student values('003', '李四', 91)";
    ret = db.execute(sql);
    if( ret != SQLITE_OK )
    {
        std::cout<<"fail to insert, "<<db.errorMsg()<<std::endl;
        return -1;
    }

    return 0;
}



int main()
{
    testBaseExecute();

    cout << "Hello World!" << endl;
    return 0;
}

