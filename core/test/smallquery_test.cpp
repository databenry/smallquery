#include <google/protobuf/util/json_util.h>

#include "gtest/gtest.h"
#include "core/smallquery.h"


class SmallQueryTest : public ::testing::Test {
protected:
    std::unique_ptr<SmallQuery> sq;

    void SetUp() {
        sq = std::unique_ptr<SmallQuery>(new SmallQuery);
    }
};


TEST(SmallQuery, ExternalAPI) {
    auto sq = SmallQuery_new();
    SmallQuery_delete(sq);
}


TEST_F(SmallQueryTest, SelectQuery) {
    std::string table_json;
    smallquery::TableData table;
    table.set_name("hello");

    auto col1 = table.add_columns();
    col1->set_name("answer");
    col1->set_type("int64");

    auto record = table.add_records();
    auto m = record->mutable_map();
    (*m)["answer"] = "42";

    google::protobuf::util::MessageToJsonString(table, &table_json);
    sq->CreateTable(table_json.c_str());

    auto ret_json = sq->Execute("select answer + 123 as new_answer from hello");

    smallquery::Rows ret;
    google::protobuf::util::JsonParseOptions options;
    google::protobuf::util::JsonStringToMessage(ret_json, &ret, options);

    ASSERT_EQ(ret.records()[0].map().at("new_answer"), "165");
}


TEST_F(SmallQueryTest, InsertQuery) {
    std::string table_json;
    smallquery::TableData table;
    table.set_name("hello");

    auto col1 = table.add_columns();
    col1->set_name("id");
    col1->set_type("int64");

    auto col2 = table.add_columns();
    col2->set_name("answer");
    col2->set_type("int64");

    google::protobuf::util::MessageToJsonString(table, &table_json);
    sq->CreateTable(table_json.c_str());

    sq->Execute("insert into hello values (1, 42)");

    auto ret_json = sq->Execute("select id, answer + 123 as new_answer from hello");

    smallquery::Rows ret;
    google::protobuf::util::JsonParseOptions options;
    google::protobuf::util::JsonStringToMessage(ret_json, &ret, options);

    ASSERT_EQ(ret.records()[0].map().at("new_answer"), "165");
}


TEST_F(SmallQueryTest, DeleteQuery) {
    std::string table_json;
    smallquery::TableData table;
    table.set_name("hello");

    auto col1 = table.add_columns();
    col1->set_name("col1");
    col1->set_type("int64");

    auto col2 = table.add_columns();
    col2->set_name("answer");
    col2->set_type("int64");

    google::protobuf::util::MessageToJsonString(table, &table_json);
    sq->CreateTable(table_json.c_str());

    sq->Execute("insert into hello values (1, 42)");
    sq->Execute("insert into hello values (1, 42)");
    sq->Execute("insert into hello values (2, 42 + 123)");
    sq->Execute("delete from hello where col1 = 1");

    auto ret_json = sq->Execute("select * from hello");

    smallquery::Rows ret;
    google::protobuf::util::JsonParseOptions options;
    google::protobuf::util::JsonStringToMessage(ret_json, &ret, options);

    ASSERT_EQ(ret.records().size(), 1);
    ASSERT_EQ(ret.records()[0].map().at("col1"), "2");
}


TEST_F(SmallQueryTest, CreateTable) {
    sq->Execute("create table hello (col1 int64, answer int64)");
    sq->Execute("insert into hello values (1, 42)");
    sq->Execute("insert into hello values (1, 42)");
    sq->Execute("insert into hello values (2, 42 + 123)");
    sq->Execute("delete from hello where col1 = 1");

    auto ret_json = sq->Execute("select * from hello");

    smallquery::Rows ret;
    google::protobuf::util::JsonParseOptions options;
    google::protobuf::util::JsonStringToMessage(ret_json, &ret, options);

    ASSERT_EQ(ret.records().size(), 1);
    ASSERT_EQ(ret.records()[0].map().at("col1"), "2");
    ASSERT_EQ(ret.records()[0].map().at("answer"), "165");
}


TEST_F(SmallQueryTest, InsertNull) {
    sq->Execute("create table hello (col1 int64, answer int64)");
    sq->Execute("insert into hello values (1, NULL)");

    auto ret_json = sq->Execute("select count(distinct col1) as cnt from hello where answer is null");

    smallquery::Rows ret;
    google::protobuf::util::JsonParseOptions options;
    google::protobuf::util::JsonStringToMessage(ret_json, &ret, options);

    ASSERT_EQ(ret.records().size(), 1);
    ASSERT_EQ(ret.records()[0].map().at("cnt"), "1");
}