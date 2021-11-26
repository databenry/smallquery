#include <google/protobuf/util/json_util.h>

#include "gtest/gtest.h"
#include "core/smallquery.h"


class SmallQueryTest : public ::testing::Test {
protected:
    std::unique_ptr<SmallQuery> mq;

    void SetUp() {
        mq = std::unique_ptr<SmallQuery>(new SmallQuery);
    }
};


TEST(SmallQuery, PythonAPI) {
    auto mq = SmallQuery_new();
    SmallQuery_delete(mq);
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
    mq->CreateTable(table_json.c_str());

    auto ret_json = mq->Execute("select answer + 123 as new_answer from hello");

    smallquery::Rows ret;
    google::protobuf::util::JsonParseOptions options;
    google::protobuf::util::JsonStringToMessage(ret_json, &ret, options);

    ASSERT_EQ(ret.records()[0].map().at("new_answer"), "165");
}