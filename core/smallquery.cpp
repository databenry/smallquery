#include <iostream>
#include <google/protobuf/util/json_util.h>

#include "core/smallquery.h"


int64_t __global_row_id_counter = 0;


void SmallQuery::CreateTable(const char* table_data_json) {
    smallquery::TableData table_data;
    google::protobuf::util::JsonParseOptions options;
    JsonStringToMessage(table_data_json, &table_data, options);

    __tables[table_data.name()] = table_data;
}


zetasql::AnalyzerOptions create_analyzer_options() {
    zetasql::AnalyzerOptions analyzer_options;
    zetasql::LanguageOptions language_options;

    language_options.SetSupportsAllStatementKinds();
    language_options.SetLanguageVersion(zetasql::VERSION_1_3);
    language_options.EnableMaximumLanguageFeatures();
    analyzer_options.set_language(language_options);

    return analyzer_options;
}


const zetasql::Type* resolve_type_name(const std::string& type) {
    // TODO: more type
    if (type == "int64") {
        return zetasql::types::Int64Type();
    } else {
        return zetasql::types::StringType();
    }

}


zetasql::Value resolve_value(const std::string& type, const std::string& val) {
    // TODO: more data types
    if (type == "int64") {
        int64_t n;
        zetasql::functions::StringToNumeric<int64_t>(val, &n, nullptr);
        return zetasql::values::Int64(n);
    } else {
        return zetasql::values::String(val);
    }
}


zetasql::Value resolve_null_value(const std::string& type) {
    if (type == "int64") {
        return zetasql::values::NullInt64();
    } else {
        return zetasql::values::NullString();
    }
}


std::unique_ptr<zetasql::SimpleColumn> create_row_id_column() {
    return std::unique_ptr<zetasql::SimpleColumn>(new zetasql::SimpleColumn("test", "__row_id", zetasql::types::Int64Type(), true));
}


std::unique_ptr<zetasql::SimpleTable> create_simple_table(smallquery::TableData& table_data) {
    std::vector<zetasql::SimpleTable::NameAndType> table_schema;
    std::map<std::string, std::string> col_types;

    for (auto col : table_data.columns()) {
        auto key = col.name();
        auto type = col.type();
        col_types[key] = type;

        table_schema.push_back({key, resolve_type_name(type)});
    }

    auto table = std::unique_ptr<zetasql::SimpleTable>(new zetasql::SimpleTable(table_data.name(), table_schema));
    std::vector<std::vector<zetasql::Value>> zetasql_records;
    for (auto record : table_data.records()) {
        std::vector<zetasql::Value> zetasql_record;
        auto m = record.map();

        for (auto col : table_schema) {
            auto key = col.first;
            auto type = col_types[key];

            if (m.count(col.first)) {
                auto val = m[col.first];

                zetasql_record.push_back(resolve_value(type, val));
            } else {
                zetasql_record.push_back(resolve_null_value(type));
            }
        }

        // set __row_id
        zetasql_record.push_back(zetasql::values::Int64(__global_row_id_counter ++));

        zetasql_records.push_back(zetasql_record);
    }

    auto col_row_id = create_row_id_column();
    table->AddColumn(col_row_id.release(), true);

    table->SetContents(zetasql_records);

    std::vector<int> pk;
    pk.push_back(table_data.columns().size());
    table->SetPrimaryKey(pk);

    return std::move(table);
}


bool is_dml(const zetasql::ResolvedStatement* stmt) {
    return  stmt->node_kind() == zetasql::RESOLVED_INSERT_STMT ||
            stmt->node_kind() == zetasql::RESOLVED_UPDATE_STMT ||
            stmt->node_kind() == zetasql::RESOLVED_DELETE_STMT ||
            stmt->node_kind() == zetasql::RESOLVED_MERGE_STMT;
}


std::vector<std::vector<zetasql::Value>> get_table_contents(const zetasql::Table* table) {
    std::vector<std::vector<zetasql::Value>> records;
    std::vector<int> column_ids;

    for (int i = 0; i < table->NumColumns(); ++ i) {
        column_ids.push_back(i);
    }

    auto iter = std::move(*(table->CreateEvaluatorTableIterator(column_ids)));
    while (iter->NextRow()) {
        std::vector<zetasql::Value> record;
        for (int i = 0; i < iter->NumColumns(); ++ i) {
            record.push_back(iter->GetValue(i));
        }
        records.push_back(record);
    }

    return records;
}


std::unique_ptr<zetasql::EvaluatorTableModifyIterator> get_modify_iter(zetasql::SimpleCatalog& catalog, zetasql::PreparedModify& q) {
    auto analyzer_options = create_analyzer_options();

    auto ret_p = q.Prepare(analyzer_options, &catalog);

    if (! ret_p.ok()) {
        std::cerr << ret_p << std::endl;
        return NULL;
    }

    auto ret_q = q.Execute();

    if (! ret_q.ok()) {
        std::cerr << ret_q.status() << std::endl;
        return NULL;
    }

    return std::move(*ret_q);
}


smallquery::Rows SmallQuery::run_query_dml(zetasql::SimpleCatalog& catalog, const char* sql) {
    smallquery::Rows rows;

    zetasql::PreparedModify q(sql, zetasql::EvaluatorOptions());

    auto iter = get_modify_iter(catalog, q);

    bool is_delete = iter->GetOperation() == zetasql::EvaluatorTableModifyIterator::Operation::kDelete;

    auto table = iter->table()->GetAs<zetasql::SimpleTable>();
    auto table_rows = get_table_contents(table);
    auto table_data = this->FindTable(table->Name());
    auto n = table->NumColumns();


    auto pk_idx = table->PrimaryKey().value();
    absl::flat_hash_map<std::vector<zetasql::Value>, int> row_idxs;
    for (int i = 0; i < table_rows.size(); ++ i) {
        auto r = table_rows[i];
        std::vector<zetasql::Value> key;
        for (int idx : pk_idx) {
            key.push_back(r[idx]);
        }
        row_idxs[key] = i;
    }

    absl::flat_hash_set<int> deleted_rows;

    while (iter->NextRow()) {
        if (q.resolved_statement()->node_kind() == zetasql::RESOLVED_INSERT_STMT) {
            auto r = table_data->add_records();
            auto m = r->mutable_map();
            for (int i = 0; i < n; ++ i) {
                // std::cout << "dml: " << table->GetColumn(i)->Name() << " : " << iter->GetColumnValue(i).DebugString() << std::endl;
                if (! iter->GetColumnValue(i).is_null()) {
                    (*m)[table->GetColumn(i)->Name()] = iter->GetColumnValue(i).DebugString();
                }
            }
        } else if (q.resolved_statement()->node_kind() == zetasql::RESOLVED_DELETE_STMT) {
            std::vector<zetasql::Value> key;

            for (int i = 0; i < table->PrimaryKey().value().size(); ++ i) {
                key.push_back(iter->GetOriginalKeyValue(i));
            }

            auto row_it = row_idxs.find(key);
            if (row_it != row_idxs.end()) {
                deleted_rows.insert(row_it->second);
            }
        } else {
            std::cerr << "run_query_dml: not supported" << std::endl;
        }
    }

    if (is_delete) {
        auto src_records = table_data->records();
        table_data->clear_records();
        for (int i = 0; i < src_records.size(); ++ i) {
            if (deleted_rows.contains(i)) continue;

            auto r = table_data->add_records();
            *(r->mutable_map()) = src_records[i].map();
        }
    }

    return rows;
}


smallquery::Rows SmallQuery::run_query_select(zetasql::SimpleCatalog& catalog, const char* sql) {
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


bool is_create_table(const zetasql::ResolvedStatement* stmt) {
    return stmt->node_kind() == zetasql::RESOLVED_CREATE_TABLE_STMT;
}


smallquery::Rows SmallQuery::run_query_create_table(zetasql::SimpleCatalog& catalog, const zetasql::ResolvedStatement* resolved_stmt) {
    smallquery::Rows rows;

    auto create_stmt = resolved_stmt->GetAs<zetasql::ResolvedCreateTableStmt>();

    std::vector<std::string> table_path = create_stmt->name_path();
    auto table_name = absl::AsciiStrToLower(table_path[0]); // TODO: add support for dataset.table

    smallquery::TableData table_data;
    table_data.set_name(table_name);

    for (const auto& col_def : create_stmt->column_definition_list()) {
        auto col = table_data.add_columns();
        col->set_name(col_def->name());
        if (col_def->type() == zetasql::types::Int64Type()) {
            col->set_type("int64");
        } else if (col_def->type() == zetasql::types::StringType()) {
            col->set_type("string");
        } else {
            std::cerr << "create_table: not supported: " << col_def->type()->DebugString() << std::endl;
        }
    }

    this->AddTable(table_data);

    return rows;
}


smallquery::Rows SmallQuery::run_query(zetasql::SimpleCatalog& catalog, const char* sql) {
    auto analyzer_options = create_analyzer_options();
    std::unique_ptr<const zetasql::AnalyzerOutput> analyzer_output;
    zetasql::TypeFactory type_factory;

    auto ret = zetasql::AnalyzeStatement(sql, analyzer_options, &catalog, &type_factory, &analyzer_output);
    if (! ret.ok()) {
        std::cerr << ret << std::endl;
    }

    auto resolved_stmt = analyzer_output->resolved_statement();

    if (is_dml(resolved_stmt)) {
        return this->run_query_dml(catalog, sql);
    } else if (is_create_table(resolved_stmt)) {
        return this->run_query_create_table(catalog, resolved_stmt);
    } else {
        return this->run_query_select(catalog, sql);
    }
}


const char* SmallQuery::Execute(const char* sql) {
    zetasql::SimpleCatalog catalog{"smallquery"};

    for (auto t : __tables) {
        auto table_data = t.second;
        auto table = create_simple_table(table_data);
        catalog.AddOwnedTable(std::move(table));
    }

    catalog.AddZetaSQLFunctions();

    auto rows = this->run_query(catalog, sql);

    __buf.clear();
    google::protobuf::util::MessageToJsonString(rows, &__buf);

    return __buf.c_str();
}