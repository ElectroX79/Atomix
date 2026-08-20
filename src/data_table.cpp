#include <../include/data/data_table.hpp>
#include <vector>

#include "security_check.hpp"


atomix::DataTable atomix::DataTable::extract(const size_t begin, const size_t end) const {
    DataTable td;
    atomix::bounds::check_index_interval(begin, end, this->columns_.size());
    td.columns_.reserve(end - begin);
    td.columns_.insert(td.columns_.end(), this->columns_.begin() + begin, this->columns_.begin() + end);
    return td;
}

atomix::DataTable atomix::DataTable::append(const DataTable &t_data) const {
    DataTable td;
    td.columns_ = this->columns_;
    td.columns_.insert(td.columns_.end(), t_data.columns_.begin(), t_data.columns_.end());
    return td;
}


atomix::DataTable atomix::DataTable::erase(const size_t begin, const size_t end) const {
    DataTable td;
    atomix::bounds::check_index_interval(begin, end, this->columns_.size());
    td.columns_.reserve(this->columns_.size() - (end - begin));
    if (begin > 0) {
        td.columns_.insert(td.columns_.begin(), this->columns_.begin(), this->columns_.begin() + begin);
    }
    if (end < this->columns_.size()) {
        td.columns_.insert(td.columns_.end(), this->columns_.begin() + end, this->columns_.end());
    }
    return td;
}
