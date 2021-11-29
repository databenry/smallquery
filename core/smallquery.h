#ifndef __MAPQUERY_H__
#define __MAPQUERY_H__

#include <string>

#include "zetasql/public/analyzer.h"
#include "zetasql/public/evaluator.h"
#include "zetasql/public/simple_catalog.h"
#include "zetasql/public/value.h"
#include "zetasql/public/type.h"
#include "zetasql/public/functions/convert_string.h"

#include "core/proto/smallquery.pb.h"


class SmallQuery {
public:
    void CreateTable(const char* table_json);
    const char* Execute(const char* sql);

    smallquery::TableData* FindTable(const std::string& table_name) {
        return &__tables[table_name];
    }

    void AddTable(const smallquery::TableData& table_data) {
        __tables[table_data.name()] = table_data;
    }

private:
    std::string __buf;
    std::map<std::string, smallquery::TableData> __tables;
};

extern "C" {
    SmallQuery* SmallQuery_new(){ return new SmallQuery(); }
    void SmallQuery_delete(SmallQuery* self) { delete self; }

    void SmallQuery_create_table(SmallQuery* self, const char* table_json) { self->CreateTable(table_json); }
    const char* SmallQuery_execute(SmallQuery* self, const char* sql) { self->Execute(sql); }
}

#endif