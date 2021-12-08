//
// Copyright 2021 Hiroyuki Sano
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

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

    smallquery::Rows run_query(zetasql::SimpleCatalog& catalog, const char* sql);
    smallquery::Rows run_query_create_table(zetasql::SimpleCatalog& catalog, const zetasql::ResolvedStatement* resolved_stmt);
    smallquery::Rows run_query_select(zetasql::SimpleCatalog& catalog, const char* sql);
    smallquery::Rows run_query_dml(zetasql::SimpleCatalog& catalog, const char* sql);
};

extern "C" {
    SmallQuery* SmallQuery_new(){ return new SmallQuery(); }
    void SmallQuery_delete(SmallQuery* self) { delete self; }

    void SmallQuery_create_table(SmallQuery* self, const char* table_json) { self->CreateTable(table_json); }
    const char* SmallQuery_execute(SmallQuery* self, const char* sql) { self->Execute(sql); }
}

#endif