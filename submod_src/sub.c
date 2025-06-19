#include <stdio.h>
#include <stdlib.h>

// 抽象接口：数据库操作
typedef struct Database Database;

typedef struct {
    void (*fetch_data)(Database*);
} DatabaseOperations;

struct Database {
    DatabaseOperations* ops;  // 函数指针表
};

// 低层模块：SQL数据库实现
typedef struct {
    Database base;          // 继承自Database接口
    char host[50];
    char username[50];
    char password[50];
} SQLDatabase;

void sql_database_fetch_data(Database* base) {
    SQLDatabase* db = (SQLDatabase*)base;  // 转换为具体类型
    printf("从SQL数据库(%s)获取数据...\n", db->host);
    // 实际实现会执行SQL查询
}

SQLDatabase* create_sql_database(const char* host, const char* username, const char* password) {
    SQLDatabase* db = malloc(sizeof(SQLDatabase));
    if (!db) return NULL;
    
    strncpy(db->host, host, sizeof(db->host));
    strncpy(db->username, username, sizeof(db->username));
    strncpy(db->password, password, sizeof(db->password));
    
    // 初始化接口实现
    static DatabaseOperations sql_ops = {
        .fetch_data = sql_database_fetch_data
    };
    
    db->base.ops = &sql_ops;
    return db;
}

// 低层模块：MongoDB数据库实现
typedef struct {
    Database base;          // 继承自Database接口
    char connection_string[100];
} MongoDatabase;

void mongo_database_fetch_data(Database* base) {
    MongoDatabase* db = (MongoDatabase*)base;  // 转换为具体类型
    printf("从MongoDB数据库(%s)获取数据...\n", db->connection_string);
    // 实际实现会执行MongoDB查询
}

MongoDatabase* create_mongo_database(const char* connection_string) {
    MongoDatabase* db = malloc(sizeof(MongoDatabase));
    if (!db) return NULL;
    
    strncpy(db->connection_string, connection_string, sizeof(db->connection_string));
    
    // 初始化接口实现
    static DatabaseOperations mongo_ops = {
        .fetch_data = mongo_database_fetch_data
    };
    
    db->base.ops = &mongo_ops;
    return db;
}

// 高层模块：报告生成器（依赖抽象接口Database）
typedef struct {
    Database* db;  // 依赖抽象接口，而非具体实现
} ReportGenerator;

void report_generator_init(ReportGenerator* generator, Database* db) {
    generator->db = db;
}

void report_generator_generate_report(ReportGenerator* generator) {
    printf("开始生成报告...\n");
    generator->db->ops->fetch_data(generator->db);  // 通过接口调用
    printf("报告生成完成\n");
}

int main() {
    // 使用SQL数据库
    SQLDatabase* sql_db = create_sql_database("localhost", "user", "pass");
    ReportGenerator generator;
    report_generator_init(&generator, (Database*)sql_db);
    report_generator_generate_report(&generator);
    
    // 清理
    free(sql_db);
    
    // 使用MongoDB数据库（无需修改ReportGenerator）
    MongoDatabase* mongo_db = create_mongo_database("mongodb://localhost:27017");
    report_generator_init(&generator, (Database*)mongo_db);
    report_generator_generate_report(&generator);
    
    // 清理
    free(mongo_db);
    
    return 0;
}