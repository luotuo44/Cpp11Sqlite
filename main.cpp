//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license

#include<assert.h>

#include <iostream>
#include<string>


#include<sqlite3.h>

#include"SqliteDB.h"
#include"PreparedStatement.h"


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


void testPreparedStatement()
{
    SqliteDB db("test.db");

    std::string sql("insert into student(id, name, score) values(?, ?, ?)");

    std::weak_ptr<PreparedStatement> st = db.createPreparedStatement(sql);

    std::shared_ptr<PreparedStatement> stmt = st.lock();
    int ret = stmt->execute("005", "zhaosi", 86);
    if( ret != SQLITE_OK && ret != SQLITE_DONE)
        std::cout<<"fail to execute "<<sql<<". reason: "<<stmt->errorMsg()<<std::endl;


    sql = "insert into student(id, name, score) values('006', ?, ?)";
    st = db.createPreparedStatement(sql);
    stmt = st.lock();
    ret = stmt->execute("wangliu", 87);
    if( ret != SQLITE_OK && ret != SQLITE_DONE )
        std::cout<<"fail to execute "<<sql<<".. reason: "<<stmt->errorMsg()<<std::endl;

    ret = stmt->close();//支持手动关闭PreparedStatement，可以检查其返回值。
    assert(ret == SQLITE_OK);


    sql = "insert into student(id, name, score) values(?, 'Tom', 79)";
    st = db.createPreparedStatement(sql);
    stmt = st.lock();
    ret = stmt->execute("007");
    if( ret != SQLITE_OK && ret != SQLITE_DONE )
        std::cout<<"fail to execute "<<sql<<".. reason: "<<stmt->errorMsg()<<std::endl;


    sql = "insert into student(id, name, score) values('008', 'Jack', 99)";
    st = db.createPreparedStatement(sql);
    stmt = st.lock();
    ret = stmt->execute();
    if( ret != SQLITE_OK && ret != SQLITE_DONE )
        std::cout<<"fail to execute "<<sql<<".. reason: "<<stmt->errorMsg()<<std::endl;

}



int main()
{
    //testBaseExecute();
    testPreparedStatement();

    cout << "Hello World!" << endl;
    return 0;
}

