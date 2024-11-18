//C0F4DFE8B340D81183C208F70F9D2D797908754D
#ifndef SILLY_H
#define SILLY_H

#include "TableEntry.h"
#include <map>
#include <vector>
#include <unordered_map>

struct Table {
    
    Table() {}

    Table(std::string name_in, size_t cols) 
     : name(name_in), num_cols(cols), hash_on(false), bst_on(false) {
        //columns.reserve(cols);
        types.reserve(cols);

    }

    std::vector<std::vector<TableEntry>> data;
    //std::vector<string> columns;
    //unordered_map???
    std::unordered_map<std::string, size_t> columns;
    std::unordered_map<TableEntry, std::vector<size_t>> user_hash;
    std::map<TableEntry, std::vector<size_t>> user_bst;
    std::vector<EntryType> types;
    std::string name;
    size_t indexed_column = 0;
    size_t num_cols = 0;
    bool hash_on = false;
    bool bst_on = false;
};

struct CompEqual {
    CompEqual(size_t idx_in, TableEntry val_in)
     : index(idx_in), val(val_in) {}

    bool operator()(const std::vector<TableEntry>& row) {
        return row[index] == val;
    }

    private: 
    size_t index;
    TableEntry val;
};

struct CompLess {
    CompLess(size_t idx_in, TableEntry val_in)
     : index(idx_in), val(val_in) {}

    bool operator()(const std::vector<TableEntry>& row) {
        return row[index] < val;
    }
    private:
    size_t index;
    TableEntry val;

};

struct CompGreater {
    CompGreater(size_t idx_in, TableEntry val_in)
     : index(idx_in), val(val_in) {}

    bool operator()(const std::vector<TableEntry>& row) {
        return row[index] > val;
    }

    private:
    size_t index;
    TableEntry val;
};



#endif
