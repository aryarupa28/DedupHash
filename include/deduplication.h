#ifndef DEDUPLICATION_H
#define DEDUPLICATION_H

#include <vector>
#include <string>



std::tuple<std::vector<std::string>, int, size_t> deduplicate_and_count(const std::vector<std::string>& dataset);





// Other declarations
void save_deduplicated_data(const std::string& output_file, const std::vector<std::string>& deduplicated_data);

#endif // DEDUPLICATION_H
