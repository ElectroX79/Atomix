module;
#include <vector>

module atomix.data.data_table;
import atomix.data.data_type;
import atomix.bounds;


[[nodiscard]] atomix::DataTable atomix::DataTable::extract(const size_t begin, const size_t end) const {
    DataTable td;
    atomix::bounds::check_index_interval(begin, end, this->columns_.size());
    td.columns_.reserve(end - begin);
    td.columns_.insert(td.columns_.end(), this->columns_.begin() + begin, this->columns_.begin() + end);
    return td;
}

void atomix::DataTable::append(const DataTable &t_data) {
    if (this == &t_data) {
        DataTable aux = t_data;
        columns_.insert(columns_.end(), aux.columns_.begin(), aux.columns_.end());
        return;
    }
    columns_.insert(columns_.end(), t_data.columns_.begin(), t_data.columns_.end());

}


void atomix::DataTable::erase(const size_t begin, const size_t end) {
    bounds::check_index_interval(begin, end, columns_.size());
    columns_.erase(columns_.begin() + begin, columns_.begin() + end);
}
