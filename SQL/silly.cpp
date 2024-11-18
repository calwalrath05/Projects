//C0F4DFE8B340D81183C208F70F9D2D797908754D

#include "silly.h"
#include "TableEntry.h"
#include <getopt.h>
#include <algorithm>
#include <string>
#include <iostream>
#include <map>
#include <vector>

using namespace std;


class SQL {

    public:

    SQL()
     : quiet_mode(false) {}

    void get_options(int argc, char *argv[]) {
        opterr = false;
        int choice;
        int option_index = 0;
    
        struct option long_options[] = {
            { "help",     no_argument,       nullptr, 'h' },
            { "quiet",    no_argument,       nullptr, 'q' },
            { nullptr,    0,                 nullptr, '\0'}
        };

        while ((choice = getopt_long(argc, argv, "hq", long_options, &option_index)) != -1) {
            switch (choice) {
                case 'q': {
                    quiet_mode = true;
                    break;
                }
                case 'h': {
                    print_help();
                    exit(0);
                }
                default: {
                    cout << "Error: Unknown command line option\n";
                    exit(0);
                }
            }
        }
    }

    void read_commands() {
        string cmd;
        do {
            if (cin.fail()) {
                cerr << "Error: Reading from cin has failed" << endl;
                exit(0);
            }

            cout << "% ";
            cin >> cmd;
            // process command (if error, getline() to clear cin)
            switch(cmd[0]) {
                case 'C': {
                    create();
                    break;
                }
                case '#': {
                    comment();
                    break;
                }
                case 'R': {
                    remove();
                    break;
                }
                case 'I': {
                    t_insert();
                    break;
                }
                case 'P': {
                    t_print();
                    break;
                }
                case 'D': {
                    t_delete();
                    break;
                }
                case 'J': {
                    t_join();
                    break;
                }
                case 'G': {
                    t_generate();
                    break;
                }
                case 'Q': {
                    break;
                }
                default: {
                    print_error();
                    break;
                }
            }
        }   while (cmd[0] != 'Q'); 
        cout << "Thanks for being silly!\n";
    }


    private:

    void print_help() {

    }

    void create() {
        string name;
        cin >> name;
        if (database.find(name) != database.end()) {
            print_error(1, "CREATE", name);
        }
        else {
            size_t n;
            cin >> n;
            database.insert({name, Table(name, n)});
            for (size_t i = 0; i < n; ++i) { // Fills vector of types
                string next;
                cin >> next;
                if (next == "string") { database[name].types.push_back(EntryType::String); }
                else if (next == "bool") { database[name].types.push_back(EntryType::Bool); }
                else if (next == "int") { database[name].types.push_back(EntryType::Int); }
                else if (next == "double") { database[name].types.push_back(EntryType::Double); }
            }
            cout << "New table " << name << " with column(s) ";
            for (size_t i = 0; i < n; ++i) { // Inserts names into unordered_map<>
                string next;
                cin >> next;
                cout << next << " ";
                database[name].columns.insert({next, i});
            }
            cout << "created\n";
        }
    }

    void comment() {
        string junk;
        getline(cin, junk);
    }
    
    void remove() {
        string name;
        cin >> name;
        if (database.find(name) != database.end()) {
            database.erase(name);
            cout << "Table " << name << " removed\n";
        }
        else {
            print_error(2, "REMOVE", name);
        }
    }

    void t_insert() {
        string name;
        cin >> name >> name;
        if (database.find(name) != database.end()) {
            Table& table = database[name];
            size_t original_rows = table.data.size();
            size_t rows;
            cin >> rows;
            string next;
            cin >> next;

            table.data.resize(table.data.size() + rows);

            for (size_t i = original_rows; i < rows + original_rows; ++i) {
                // Reserve the columns for each row
                table.data[i].reserve(table.num_cols);
                for (size_t j = 0; j < table.types.size(); ++j) { // Wasn't inserting correct types, only inserting strings
                    switch(table.types[j]) {
                        case EntryType::String: {
                            string new_value;
                            cin >> new_value; // NOT READING IN NEXT ITEM
                            table.data[i].emplace_back(new_value);
                            if (j == table.indexed_column) {
                                if (table.hash_on) {
                                    table.user_hash[table.data[i][j]].push_back(i);
                                }
                                else if (table.bst_on) {
                                    table.user_bst[table.data[i][j]].push_back(i);
                                }
                            }
                            break;
                        }
                        case EntryType::Int: {
                            int new_value;
                            cin >> new_value;
                            table.data[i].emplace_back(new_value);
                            if (j == table.indexed_column) {
                                if (database[name].hash_on) {
                                    table.user_hash[table.data[i][j]].push_back(i);
                                }
                                else if (table.bst_on) {
                                    table.user_bst[table.data[i][j]].push_back(i);
                                }
                            }
                            break;
                        }
                        case EntryType::Double: {
                            double new_value;
                            cin >> new_value;
                            table.data[i].emplace_back(new_value);
                            if (j == table.indexed_column) {
                                if (table.hash_on) {
                                    table.user_hash[table.data[i][j]].push_back(i);
                                }
                                else if (table.bst_on) {
                                    table.user_bst[table.data[i][j]].push_back(i);
                                }
                            }
                            break;
                        }
                        case EntryType::Bool: {
                            bool new_value;
                            cin >> new_value;
                            table.data[i].emplace_back(new_value);
                            if (j == table.indexed_column) {
                                if (table.hash_on) {
                                    table.user_hash[table.data[i][j]].push_back(i);
                                }
                                else if (database[name].bst_on) {
                                    table.user_bst[table.data[i][j]].push_back(i);
                                }
                            }
                            break;
                        }
                    }
                }
            }
            cout << "Added " << rows << " rows to " << name << " from position " 
            << table.data.size() - rows << " to " << table.data.size() - 1 << "\n";
        }
        else {
            cout << "Error during INSERT: " << name << " does not name a table in the database\n";
            string junk;
            getline(cin, junk);
        }
    }

    void t_join() { // Check col_num 1/2 are correct, checking wrong columns?
        string table1_name;
        string table2_name;
        string col1;
        string col2;
        size_t n;
        string next_col;
        size_t next_table;
        cin >> table1_name >> table2_name >> table2_name >> col1 >> col1 >> col2 >> col2 >> next_col >> next_col >> n;
        vector<string> print_cols;
        vector<size_t> col_tables;
        if (database.find(table1_name) == database.end()) {
            print_error(2, "JOIN", table1_name);
            return;
        }
        if (database.find(table2_name) == database.end()) {
            print_error(2, "JOIN", table2_name);
            return;
        }
        if (database[table1_name].columns.find(col1) == database[table1_name].columns.end()) {
            print_error("JOIN", table1_name, col1);
            return;
        }
        if (database[table2_name].columns.find(col2) == database[table2_name].columns.end()) {
            print_error("JOIN", table2_name, col2);
            return;
        }
        Table& t1 = database[table1_name];
        Table& t2 = database[table2_name];
        for (size_t i = 0; i < n; ++i) { // Reads in columns to print
            cin >> next_col >> next_table;
            if (next_table == 1) {
                if (t1.columns.find(next_col) == t1.columns.end()) {
                    print_error("JOIN", table1_name, next_col);
                    return;
                }
            }
            else {
                if (t2.columns.find(next_col) == t2.columns.end()) {
                    print_error("JOIN", table2_name, next_col);
                    return;
                }
            }
            print_cols.push_back(next_col);
            col_tables.push_back(next_table);
        }
        // Make a temp hash
        unordered_map<TableEntry, vector<size_t>> temp_hash;
        size_t col_num1 = t1.columns[col1];
        size_t col_num2 = t2.columns[col2];
        size_t rows_printed = 0;
        if (!quiet_mode) {
            for (size_t i = 0; i < print_cols.size(); ++i) {
                cout << print_cols[i] << " ";
            }
            cout << "\n";
            if (t2.hash_on && t2.indexed_column == col_num2 ) {
                for (size_t i = 0; i < t1.data.size(); ++i) {
                    TableEntry entry = t1.data[i][col_num1];
                    rows_printed += t2.user_hash[entry].size();
                    for (size_t k = 0; k < t2.user_hash[entry].size(); ++k) { // For each match between row i of table1 and another item in table2
                        for (size_t j = 0; j < print_cols.size(); ++j) {
                            if (col_tables[j] == 1) {
                                cout << t1.data[i][t1.columns[print_cols[j]]] << " ";
                            }
                            else {
                                size_t row = t2.user_hash[entry][k];
                                cout << t2.data[row][t2.columns[print_cols[j]]] << " ";
                            }
                        }
                        cout << "\n";
                    }
                }
            }
            else {
                for (size_t i = 0; i < t2.data.size(); ++i) {
                    temp_hash[t2.data[i][col_num2]].push_back(i);
                }

                for (size_t i = 0; i < t1.data.size(); ++i) { // Not printing properly
                    TableEntry entry = t1.data[i][col_num1];
                    rows_printed += temp_hash[entry].size();
                    for (size_t k = 0; k < temp_hash[entry].size(); ++k) { // For each match between row i of table1 and another item in table2
                        for (size_t j = 0; j < print_cols.size(); ++j) {
                            if (col_tables[j] == 1) {
                                cout << t1.data[i][t1.columns[print_cols[j]]] << " ";
                            }
                            else {
                                size_t row = temp_hash[entry][k]; // "row" is the row of the next TableEntry with the key "entry"
                                cout << t2.data[row][t2.columns[print_cols[j]]] << " ";
                            }
                        }
                        cout << "\n";
                    }
                }
            }
        }
        else {
            if (t2.hash_on && t2.indexed_column == col_num2 ) {
                for (size_t i = 0; i < t1.data.size(); ++i) {
                    TableEntry entry = t1.data[i][col_num1];
                    rows_printed += t2.user_hash[entry].size();
                }
            }
            else {
                for (size_t i = 0; i < t2.data.size(); ++i) {
                    temp_hash[t2.data[i][col_num2]].push_back(i);
                }

                for (size_t i = 0; i < t1.data.size(); ++i) {
                    TableEntry entry = t1.data[i][col_num1];
                    rows_printed += temp_hash[entry].size();
                }
            }
        }
        cout << "Printed " << rows_printed << " rows from joining " << table1_name << " to " << table2_name << "\n";
    }

    void t_print() { // Use an existing hash if possible
        vector<string> requested_cols;
        string name;
        cin >> name >> name;
        string next;
        size_t num_cols_requested;
        cin >> num_cols_requested;
        if (database.find(name) == database.end()) {
            print_error(2, "PRINT", name);
            return;
        }
        Table& table = database[name];
        for (size_t i = 0; i < num_cols_requested; ++i) {    
            cin >> next;
            if (table.columns.find(next) == table.columns.end()) {
                print_error("PRINT", name, next);
                return;
            }
            requested_cols.push_back(next);
        }
        cin >> next;
        if (next == "WHERE") {
            t_print_where(requested_cols, name);
        }
        else if (next == "ALL") {
            if (!quiet_mode) {
                for (size_t i = 0; i < requested_cols.size(); ++i) {
                    cout << requested_cols[i] << " ";
                }
                cout << "\n";
                t_print_all(requested_cols, name);
            }
            cout << "Printed " << table.data.size() << " matching rows from " << name << "\n";
        }
    }

    void t_print_all(const vector<string>& requested_cols, const string& name) { 
        Table& table = database[name];
        for (size_t i = 0; i < table.data.size(); ++i) {
            for (size_t j = 0; j < requested_cols.size(); ++j) {
                cout << table.data[i][table.columns[requested_cols[j]]] << " ";
            }
            cout << "\n";
        }
    }

    void t_print_where(const vector<string>& requested_cols, const string& name) { // If hash table in conditional column AND using op=, make opimized route with .find()
        string colname;
        string op;
        cin >> colname >> op;
        if (database[name].columns.find(colname) == database[name].columns.end()) {
            print_error("PRINT", name, colname);
            return;
        }
        size_t col_idx = database[name].columns[colname];
        if (!quiet_mode) {
            for (size_t i = 0; i < requested_cols.size(); ++i) {
                cout << requested_cols[i] << " ";
            }
            cout << "\n";
        }
        switch(database[name].types[col_idx]) {
            case EntryType::Bool: {
                bool value;
                cin >> value;
                TableEntry entry(value);
                t_print_where_helper(entry, name, op, requested_cols, col_idx);
                break;
            }
            case EntryType::Double: {
                double value;
                cin >> value;
                TableEntry entry(value);
                t_print_where_helper(entry, name, op, requested_cols, col_idx);
                break;
            }
            case EntryType::Int: {
                int value;
                cin >> value;
                TableEntry entry(value);
                t_print_where_helper(entry, name, op, requested_cols, col_idx);
                break;
            }
            case EntryType::String: {
                string value;
                cin >> value;
                TableEntry entry(value);
                t_print_where_helper(entry, name, op, requested_cols, col_idx);
                break;
            }
        }
    }

    void t_print_where_helper(const TableEntry& entry, const string& name, const string& op, const vector<string>& requested_cols, size_t col_idx) {
        Table& table = database[name];
        size_t rows_printed = 0;
        if (table.bst_on && col_idx == table.indexed_column) { // If a BST exists for the conditional column, it should be sequenced via the BST



            if (op == "=") {
                auto equal_it = table.user_bst.find(entry);

                if (equal_it != table.user_bst.end()) {
                    if (quiet_mode) {
                        rows_printed += equal_it->second.size();
                    }
                    else {
                        for (size_t i = 0; i < equal_it->second.size(); ++i) { // PROBLEM: THE BST HAS ROWS THAT NO LONGER EXIST IN THE TABLE
                            for (size_t j = 0; j < requested_cols.size(); ++j) {
                                cout << table.data[equal_it->second[i]][table.columns[requested_cols[j]]] << " ";
                            }
                            ++rows_printed; // Increment after every change in row
                            cout << "\n"; // Newline after every row
                        }
                    }
                    
                }
            }
            else if (op == "<") {

                auto lower_bound_bst = table.user_bst.lower_bound(entry);
                for (auto it = table.user_bst.begin(); it != lower_bound_bst; ++it) {
                    if (quiet_mode) {
                        rows_printed += it->second.size();
                    }
                    else {
                        for (auto i : it->second) {
                            for (size_t j = 0; j < requested_cols.size(); ++j) {
                                cout << table.data[i][table.columns[requested_cols[j]]] << " ";
                            }
                            ++rows_printed; // Increment after every change in row
                            cout << "\n"; // Newline after every row
                        }
                    }
                }
            }
            else {
                auto upper_bound_bst = table.user_bst.upper_bound(entry);
                for (auto it = upper_bound_bst; it != table.user_bst.end(); ++it) {
                    if (quiet_mode) {
                        rows_printed += it->second.size();
                    }
                    else {
                        for (auto i : it->second) {
                            for (size_t j = 0; j < requested_cols.size(); ++j) {
                                cout << table.data[i][table.columns[requested_cols[j]]] << " ";
                            }
                            ++rows_printed; // Increment after every change in row
                            cout << "\n"; // Newline after every row
                        }
                    }
                }
            }
        }
        else {
            for (size_t i = 0; i < table.data.size(); ++i) {
                if (op == "=" && table.data[i][col_idx] == entry) { // Causing error with TableEntry comparison
                    if (!quiet_mode) {
                        for (size_t j = 0; j < requested_cols.size(); ++j) {
                            cout << table.data[i][table.columns[requested_cols[j]]] << " ";
                        }
                        cout << "\n";
                    }
                    ++rows_printed;
                }
                if (op == "<" && table.data[i][col_idx] < entry) {
                    if (!quiet_mode) {
                        for (size_t j = 0; j < requested_cols.size(); ++j) {
                            cout << table.data[i][table.columns[requested_cols[j]]] << " ";
                        }
                        cout << "\n";
                    }
                    ++rows_printed;
                }
                if (op == ">" && table.data[i][col_idx] > entry) {
                    if (!quiet_mode) {
                        for (size_t j = 0; j < requested_cols.size(); ++j) {
                            cout << table.data[i][table.columns[requested_cols[j]]] << " ";
                        }
                        cout << "\n";
                    }
                    ++rows_printed;
                }
            }
        }
        cout << "Printed " << rows_printed << " matching rows from " << name << "\n";
    }


    void t_delete() { // use .erase(remove_if()) using functor, pass in const vector<TableEntry, size_t>&, initialize with specified column idx (regenerate hash/bst afterwards)
        string name;
        cin >> name >> name;
        string colname;
        cin >> colname >> colname;

        if (database.find(name) == database.end()) {
            print_error(2, "DELETE", name);
            return;
        }
        if (database[name].columns.find(colname) == database[name].columns.end()) {
            print_error("DELETE", name, colname);
            return;
        }
        
        string op;
        cin >> op;
        

        size_t col_idx = database[name].columns[colname];
        switch(database[name].types[col_idx]) {
            case EntryType::Bool: {
                bool value;
                cin >> value;
                TableEntry entry(value);
                t_delete_helper(name, col_idx, entry, op);
                break;
            }
            case EntryType::Double: {
                double value;
                cin >> value;
                TableEntry entry(value);
                t_delete_helper(name, col_idx, entry, op);
                break;
            }
            case EntryType::Int: {
                int value;
                cin >> value;
                TableEntry entry(value);
                t_delete_helper(name, col_idx, entry, op);
                break;
            }
            case EntryType::String: {
                string value;
                cin >> value;
                TableEntry entry(value);
                t_delete_helper(name, col_idx, entry, op);
                break;
            }
        }
    }

    void t_delete_helper(const string& name, size_t index, const TableEntry& entry, const string& op) {
        Table& table = database[name];
        size_t rows_removed = table.data.size();
        if (op == "<") {
            table.data.erase(remove_if(table.data.begin(), table.data.end(), CompLess(index, entry)), table.data.end());
        }
        else if (op == ">") {
            table.data.erase(remove_if(table.data.begin(), table.data.end(), CompGreater(index, entry)), table.data.end());
        }
        else { // op == "="
            table.data.erase(remove_if(table.data.begin(), table.data.end(), CompEqual(index, entry)), table.data.end());
        }
        rows_removed -= table.data.size(); // Edited
        if (rows_removed != 0 && table.bst_on) { generate_bst(table.indexed_column, name); }
        else if (rows_removed != 0 && table.hash_on) { generate_hash(table.indexed_column, name); }
        
        cout << "Deleted " << rows_removed << " rows from " << name << "\n";
    }

    void t_generate() {
        string name;
        cin >> name >> name;
        string index_type;
        cin >> index_type;
        string colname;
        cin >> colname >> colname >> colname;
        if (database.find(name) == database.end()) {
            print_error(2, "GENERATE", name);
            return;
        }
        Table& table = database[name];
        if (table.columns.find(colname) == table.columns.end()) {
            print_error("GENERATE", name, colname);
            return;
        }
        size_t col_num = table.columns[colname];
        if (index_type == "hash") {
            generate_hash(col_num, name);
            cout << "Created hash index for table " << name << " on column " << colname << ", with " << table.user_hash.size() << " distinct keys\n";
        }
        else if (index_type == "bst") {
            generate_bst(col_num, name);
            cout << "Created bst index for table " << name << " on column " << colname << ", with " << table.user_bst.size() << " distinct keys\n";
        }
    } 

    void generate_hash(size_t col_num, const string& name) { // Check what happens if hash or bst already exists
        delete_index(name);
        Table& table = database[name];
        for (size_t i = 0; i < table.data.size(); ++i) {
            table.user_hash[table.data[i][col_num]].push_back(i);
        }
        table.hash_on = true;
        table.bst_on = false;
        table.indexed_column = col_num;
    }


    void generate_bst(size_t col_num, const string& name) {
        delete_index(name);
        Table& table = database[name];
        for (size_t i = 0; i < table.data.size(); ++i) {
            table.user_bst[table.data[i][col_num]].push_back(i);
        }
        table.bst_on = true;
        table.hash_on = false;
        table.indexed_column = col_num;
    }

    void delete_index(const string& name) { // Clears a pre-existing hash/bst
        Table& table = database[name];
        table.user_hash.clear();
        table.hash_on = false;
        table.user_bst.clear();
        table.bst_on = false;
    }

    void print_error(const string& command, const string& table_name, const string& col_name) {
        string junk;
        getline(cin, junk);
        cout << "Error during " << command << ": " << col_name << " does not name a column in " << table_name << "\n";
    }

    void print_error(int error, const string& command, const string& table_name) {
        string junk;
        getline(cin, junk);
        if (error == 1) {
            cout << "Error during " << command << ": Cannot create already existing table " << table_name << "\n";
        }
        else if (error == 2) {
            cout << "Error during " << command << ": " << table_name << " does not name a table in the database\n";
        }
        else {
            cout << "Unrecognized error\n";
        }
    }

    void print_error() {
        string junk;
        getline(cin, junk);
        cout << "Error: unrecognized command\n";
    }
    

    unordered_map<string, Table> database;
    bool quiet_mode = false;

};


int main(int argc, char *argv[]) {
    std::ios_base::sync_with_stdio(false);
    cout << boolalpha;
    cin >> boolalpha;
    SQL sql;
    sql.get_options(argc, argv);
    sql.read_commands();
    return 0;
}
