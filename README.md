# Cpp11Sqlite
用C++ 11 封装sqlite的C接口。封装的接口简洁、易用，并支持prepared statement以及批量插入。

# 使用例子

## 没有返回数据的语句

### 不影响数据集的语句

#### 表操作语句

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


#### 简单插入语句

```cpp
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
```

#### 批量插入语法1

> 对应sql语法: insert into student(id, name, score) values('001', 'aa', 81),('002', 'bb', 82)('003', 'cc', 83),('004', 'dd', 84);

```cpp
    SqliteDB db("test_many.db");
    std::vector<std::tuple<std::string, std::string, int>> vec;
    vec.emplace_back(std::make_tuple(std::string("001"), std::string("aa"), 81));
    vec.emplace_back(std::make_tuple(std::string("002"), std::string("bb"), 82));
    vec.emplace_back(std::make_tuple(std::string("003"), std::string("cc"), 83));
    vec.emplace_back(std::make_tuple(std::string("004"), std::string("dd"), 84));

    string sql = "insert into student(id, name, score) ";
    ret = db.executemany(sql, vec);
    if( ret != SQLITE_OK )
    {
        std::cout<<"fail to insert many "<<db.errorMsg()<<std::endl;
        return -1;
    }
```

#### 批量插入语法2

> 对应sql语法: insert into student(id, name, score)  select '005', 'ee', 85 union all select '006', 'ff', 86 union all  select '007', 'gg', 87

```cpp
    SqliteDB db("test_many.db");
    std::vector<std::tuple<std::string, std::string, int>> vec;
    vec.emplace_back(std::make_tuple(std::string("005"), std::string("ee"), 85));
    vec.emplace_back(std::make_tuple(std::string("006"), std::string("ff"), 86));
    vec.emplace_back(std::make_tuple(std::string("007"), std::string("gg"), 87));

    string sql = "insert into student(id, name, score) ";

    ret = db.executemany2(sql, vec);
    if( ret != SQLITE_OK )
    {
        std::cout<<"fail to insertmany2 "<<db.errorMsg()<<std::endl;
        return -1;
    }
```



#### prepared statement插入，所以参数都未知

```cpp
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
```

#### prepared statement插入，部分参数未知

```cpp
    SqliteDB db("test.db");
    std::string sql = "insert into student(id, name, score) values('006', ?, ?)";
    std::weak_ptr<PreparedStatement> st = db.createPreparedStatement(sql);
    std::shared_ptr<PreparedStatement> stmt = st.lock();
    int ret = stmt->update("wangliu", 87);
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
```

## 带返回数据的语句

#### 使用列序号访问一行数据


```cpp
    SqliteDB db("test.db");
    std::string sql("select * from student score > ?");
    std::weak_ptr<PreparedStatement> st = db.createPreparedStatement(sql);
    std::shared_ptr<PreparedStatement> stmt = st.lock();

    int ret;
    QueryResultRowSet begin, end;
    std::tie(ret, begin, end) = stmt->query(80);

    if( ret != SQLITE_OK )
    {
        std::cout<<"fail to query "<<stmt->errorMsg()<<std::endl;
        return ;
    }


    std::cout<<"id\tname\tscore"<<std::endl;
    for(; begin != end; ++begin)
    {
        auto col = *begin;

        std::string id = col.getColumn<std::string>(0);
        std::string name = col.getColumn<std::string>(1);
        int score = col.getColumn<int>(2);

        std::cout<<id<<"\t"<<name<<"\t"<<score<<std::endl;
    }
```

输出结果

|id   |   name       |  score|
|-----|--------------|-------|
|001  |   xiaoming   |   99|
|005  |   zhaosi     |   86|
|006  |   wangliu    |   87|
|008  |   Jack       |   99|



#### 使用列名访问一行数据

```cpp
    SqliteDB db("test.db");
    std::string sql = "select name, score as sc from student";
    std::weak_ptr<PreparedStatement> st = db.createPreparedStatement(sql);
    std::shared_ptr<PreparedStatement> stmt = st.lock();

    int ret;
    QueryResultRowSet begin, end;
    std::tie(ret, begin, end) = stmt->query();
    std::cout<<"name\tscore"<<std::endl;
    while(begin != end)
    {
        auto col = *begin++;

        std::string name = col.getColumn<std::string>("name");
        int score = col.getColumn<int>("sc");

        std::cout<<name<<"\t"<<score<<std::endl;
    }
    stmt->close();//主动关闭
```

输出结果:

|name       | score|
|-----------|------|
|xiaoming   |  99|
|zhangsan   |  80|
|zhaosi     |  86|
|test       |  73|
|wangliu    |  87|
|Tom        |  79|
|Jack       |  99|
