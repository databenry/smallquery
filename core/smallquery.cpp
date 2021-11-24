#include <iostream>
#include <google/protobuf/util/json_util.h>

#include "core/smallquery.h"


void SmallQuery::CreateTable(const char* table_data_json) {
    smallquery::TableData table_data;
    google::protobuf::util::JsonParseOptions options;
    JsonStringToMessage(table_data_json, &table_data, options);

    __tables[table_data.name()] = table_data;
}


zetasql::AnalyzerOptions create_analyzer_options() {
    zetasql::AnalyzerOptions analyzer_options;
    analyzer_options.mutable_language()->SetSupportsAllStatementKinds();

    zetasql::LanguageOptions language_options;
    language_options.SetLanguageVersion(zetasql::VERSION_1_3);
    language_options.EnableMaximumLanguageFeatures();
    analyzer_options.set_language(language_options);

    return analyzer_options;
}

std::unique_ptr<zetasql::SimpleTable> create_simple_table(smallquery::TableData& table_data) {
    std::vector<zetasql::SimpleTable::NameAndType> table_schema;
    std::map<std::string, std::string> col_types;
    for (auto col : table_data.columns()) {
        auto key = col.name();
        auto type = col.type();
        col_types[key] = type;

        // TODO: type
        if (type == "int64") {
            table_schema.push_back({key, zetasql::types::Int64Type()});
        } else {
            table_schema.push_back({key, zetasql::types::StringType()});
        }
        // std::cout << "create_simple_table: column: " << key << " as " << type << std::endl;
    }

    auto table = std::unique_ptr<zetasql::SimpleTable>(new zetasql::SimpleTable(table_data.name(), table_schema));
    std::vector<std::vector<zetasql::Value>> zetasql_records;
    for (auto record : table_data.records()) {
        std::vector<zetasql::Value> zetasql_record;
        for (auto col : record.map()) {
            auto key = col.first;
            auto val = col.second;
            // std::cout << "create_simple_table: record: " << key << " - " << val << std::endl;

            // TODO: support more data types
            if (col_types[key] == "int64") {
                int64_t n;
                zetasql::functions::StringToNumeric<int64_t>(val, &n, nullptr);
                zetasql_record.push_back(zetasql::values::Int64(n));
            } else {
                auto v = zetasql::values::String(val);
                zetasql_record.push_back(v);
            }
        }
        zetasql_records.push_back(zetasql_record);
    }
    table->SetContents(zetasql_records);

    return std::move(table);
}


smallquery::Rows run_query(zetasql::SimpleCatalog& catalog, const char* sql) {
    smallquery::Rows rows;
    auto analyzer_options = create_analyzer_options();

    zetasql::PreparedQuery q(sql, zetasql::EvaluatorOptions());
    auto ret_p = q.Prepare(analyzer_options, &catalog);
    if (! ret_p.ok()) {
        std::cerr << ret_p << std::endl;
        return rows;
    }

    auto ret_e = q.ExecuteAfterPrepare();
    if (! ret_e.ok()) {
        std::cerr << ret_e.status() << std::endl;
        return rows;
    }

    auto iter = std::move(*ret_e);
    auto n = iter->NumColumns();

    while (iter->NextRow()) {
        auto r = rows.add_records();
        auto m = r->mutable_map();
        for (int i = 0; i < n; ++ i) {
            (*m)[iter->GetColumnName(i)] = iter->GetValue(i).DebugString();
        }
    }

    return rows;
}


const char* SmallQuery::Execute(const char* sql) {
    zetasql::SimpleCatalog catalog{"smallquery"};

    for (auto t : __tables) {
        auto table_data = t.second;
        auto table = create_simple_table(table_data);
        catalog.AddOwnedTable(std::move(table));
    }

    catalog.AddZetaSQLFunctions();

    auto rows = run_query(catalog, sql);

    google::protobuf::util::MessageToJsonString(rows, &__buf);

    return __buf.c_str();
}
