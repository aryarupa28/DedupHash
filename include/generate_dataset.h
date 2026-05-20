#ifndef GENERATE_DATASET_H
#define GENERATE_DATASET_H

#include <string>

void generate_dataset(const std::string& file_name);
void deduplicate_and_save(const std::string& input_file, const std::string& output_file);

#endif // GENERATE_DATASET_H
