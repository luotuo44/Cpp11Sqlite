# Cpp11Sqlite
用C++ 11 封装sqlite的C接口。封装的接口简洁、易用，并支持prepared statement以及批量插入。

#使用例子
##没有返回数据的语句
###不影响数据集的语句
####表操作语句
```cpp
    SqliteDB db("test.db");

    int ret = db.execute("drop table if exists student");
    if( ret != SQLITE_OK )
    {
        std::cout<<"fail to drop table "<<db.errorMsg()<<std::endl;
        return -1;
    }

    std::string sql = "create table if not exists student(id varchar(32) primary key, "
                      "name varchar(128), score int)";
    ret = db.execute(sql);
    if( ret != SQLITE_OK )
    {
        std::cout<<"fail to create table, "<<db.errorMsg()<<std::endl;
        return -1;
    }
```
