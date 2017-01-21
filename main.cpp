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
    int ret = stmt->update("005", "zhaosi", 86);
    if( ret != SQLITE_OK && ret != SQLITE_DONE)
    {
        std::cout<<"fail to execute "<<sql<<". reason: "<<stmt->errorMsg()<<std::endl;
        return ;
    }

    ret = stmt->reset();
    if( ret != SQLITE_OK )
    {
        std::cout<<"fial to reset "<<sql<<". reason: "<<stmt->errorMsg()<<std::endl;
        return ;
    }

    ret = stmt->update("004", "test", 73);
    if( ret != SQLITE_OK && ret != SQLITE_DONE)
    {
        std::cout<<"fail to execute "<<sql<<". reason: "<<stmt->errorMsg()<<std::endl;
        return ;
    }


    sql = "insert into student(id, name, score) values('006', ?, ?)";
    st = db.createPreparedStatement(sql);
    stmt = st.lock();
    ret = stmt->update("wangliu", 87);
    if( ret != SQLITE_OK && ret != SQLITE_DONE )
        std::cout<<"fail to execute "<<sql<<".. reason: "<<stmt->errorMsg()<<std::endl;

    ret = stmt->close();//支持手动关闭PreparedStatement，可以检查其返回值。
    assert(ret == SQLITE_OK);


    sql = "insert into student(id, name, score) values(?, 'Tom', 79)";
    st = db.createPreparedStatement(sql);
    stmt = st.lock();
    ret = stmt->update("007");
    if( ret != SQLITE_OK && ret != SQLITE_DONE )
        std::cout<<"fail to execute "<<sql<<".. reason: "<<stmt->errorMsg()<<std::endl;


    sql = "insert into student(id, name, score) values('008', 'Jack', 99)";
    st = db.createPreparedStatement(sql);
    stmt = st.lock();
    ret = stmt->update();
    if( ret != SQLITE_OK && ret != SQLITE_DONE )
        std::cout<<"fail to execute "<<sql<<".. reason: "<<stmt->errorMsg()<<std::endl;

}


void testQueryPreparedStatement()
{
    SqliteDB db("test.db");

    std::string sql("select id, name, score from student");

    std::weak_ptr<PreparedStatement> st = db.createPreparedStatement(sql);

    std::shared_ptr<PreparedStatement> stmt = st.lock();

    int ret;
    QueryResultRowSet begin, end;
    std::tie(ret, begin, end) = stmt->query();

    if( ret != SQLITE_OK )
    {
        std::cout<<"fail to query "<<stmt->errorMsg()<<std::endl;
        return ;
    }


    for(; begin != end; ++begin)
    {
        auto col = *begin;

        std::string id = col.getColumn<std::string>(0);
        std::string name = col.getColumn<std::string>(1);
        int score = col.getColumn<int>(2);

        std::cout<<id<<"\t"<<name<<"\t"<<score<<std::endl;
    }


    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    std::cout<<"\n\n second query "<<std::endl;
    sql = "select * from student where score > ?";
    st = db.createPreparedStatement(sql);
    stmt = st.lock();
    std::tie(ret, begin, end) = stmt->query(80);

    if( ret != SQLITE_OK )
    {
        std::cout<<"select * fail. Reason: "<<stmt->errorMsg()<<std::endl;
    }


    for(; begin != end; ++begin)
    {
        auto col = *begin;

        std::string id = col.getColumn<std::string>(0);
        std::string name = col.getColumn<std::string>(1);
        int score = col.getColumn<int>(2);

        std::cout<<id<<"\t"<<name<<"\t"<<score<<std::endl;
    }
}


int testInsertMany()
{
    SqliteDB db("test_many.db");

    std::string sql = "create table if not exists student(id varchar(32) primary key, "
                      "name varchar(128), score int)";
    int ret = db.execute(sql);
    if( ret != SQLITE_OK )
    {
        std::cout<<"fail to create table, "<<db.errorMsg()<<std::endl;
        return -1;
    }

    std::vector<std::tuple<std::string, std::string, int>> vec;
    vec.emplace_back(std::make_tuple(std::string("001"), std::string("aa"), 81));
    vec.emplace_back(std::make_tuple(std::string("002"), std::string("bb"), 82));
    vec.emplace_back(std::make_tuple(std::string("003"), std::string("cc"), 83));
    vec.emplace_back(std::make_tuple(std::string("004"), std::string("dd"), 84));

    sql = "insert into student(id, name, score) ";

    //对应sql语句
    //insert into student(id, name, score) values('001', 'aa', 81),('002', 'bb', 82),
    //('003', 'cc', 83),('004', 'dd', 84);
    ret = db.executemany(sql, vec);
    if( ret != SQLITE_OK )
    {
        std::cout<<"fail to insertmany "<<db.errorMsg()<<std::endl;
        return -1;
    }


    vec.clear();
    vec.emplace_back(std::make_tuple(std::string("005"), std::string("ee"), 85));
    vec.emplace_back(std::make_tuple(std::string("006"), std::string("ff"), 86));
    vec.emplace_back(std::make_tuple(std::string("007"), std::string("gg"), 87));

    sql = "insert into student(id, name, score) ";
    //对应sql语句 insert into student(id, name, score)  select '005', 'ee', 85 union all
    // select '006', 'ff', 86 union all  select '007', 'gg', 87
    ret = db.executemany2(sql, vec);
    if( ret != SQLITE_OK )
    {
        std::cout<<"fail to insertmany2 "<<db.errorMsg()<<std::endl;
        return -1;
    }

    return 0;
}


int main()
{
    //testBaseExecute();
    //testPreparedStatement();

    //testQueryPreparedStatement();

    testInsertMany();

    cout << "Hello World!" << endl;
    return 0;
}

