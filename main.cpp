#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/algorithm.hpp>
#include <sys/stat.h>
#include <utility>
#include <vector>
#include <complex>
#include <iomanip>
#include <boost/algorithm/string/split.hpp>

namespace fs=boost::filesystem;
typedef uintmax_t file_size_in_bytes;
typedef std::string filename;
typedef std::pair<filename, file_size_in_bytes> file_size_pair;


void print_file_size_formatted(file_size_in_bytes size_in_bytes, const filename &file_path);

std::vector<std::string> getSubPaths(const filename &basic_string);

file_size_in_bytes getSize(const decltype(fs::current_path()) &path) {
    struct stat statbuf{};
    if (stat(path.string().c_str(), &statbuf) == -1) {
        std::cout << "Size on disk error" << std::endl;
        throw std::runtime_error(path.string() + ":Not an File");
    }
    return static_cast<file_size_in_bytes>(statbuf.st_blocks * 512);
}

bool sort_by_file_size_in_bytes_second_field_of_pair(const file_size_pair &a,
                                                     const file_size_pair &b) {
    return (a.second > b.second);

}

std::vector<file_size_pair> get_vector_sorted_by_file_size_descending(
        std::vector<std::pair<std::string, file_size_in_bytes>> &unsortedVector) {
    sort(unsortedVector.begin(), unsortedVector.end(),
         sort_by_file_size_in_bytes_second_field_of_pair);
    return unsortedVector;
}

file_size_in_bytes getSizeForDirectory(const decltype(fs::current_path()) &path) {
    file_size_in_bytes file_size_in_bytes1{0};
    for (const auto &p: fs::directory_iterator(path)) {
        if (fs::is_regular_file(p)) {
            file_size_in_bytes1 += getSize(p);
        } else if (fs::is_directory(p)) {
            if (fs::is_symlink(p)) {
                continue;
            }
            file_size_in_bytes1 += getSizeForDirectory(p.path());
        }
    }
    return file_size_in_bytes1;
}

std::vector<file_size_pair> getAllInDirectory() {
    auto path = fs::current_path();
    std::vector<file_size_pair> list_of_directory;
    for (const auto &p: fs::directory_iterator(path)) {
        if (fs::is_regular_file(p)) {
            auto file_size = getSize(p);
            auto pair = std::make_pair(static_cast<filename>(p.path().string()), file_size);
            list_of_directory.push_back(pair);
        } else if (fs::is_directory(p)) {
            auto file_size = getSizeForDirectory(p.path());
            auto pair = std::make_pair(static_cast<filename>(p.path().string()), file_size);
            list_of_directory.push_back(pair);

        }
    }
    return list_of_directory;
}

void printSizeTree(std::vector<file_size_pair> vector) {
    const std::vector<file_size_pair> sortedVector = get_vector_sorted_by_file_size_descending(
            vector);
    for (const file_size_pair &sorted_pair: sortedVector) {
        print_file_size_formatted(sorted_pair.second, sorted_pair.first);
    }
}


void print_file_size_formatted(const file_size_in_bytes size_in_bytes, const filename &file_path) {
    std::vector<std::string> file_path_splitted = getSubPaths(file_path);

    int number_of_digits = static_cast<int>(std::log10(size_in_bytes));
    if (number_of_digits >= 9) {
        std::cout << std::setw(0) << round(float(size_in_bytes) / 10e8) << "GB" << std::setw(20 - number_of_digits + 9)
                  << "  "
                  << file_path_splitted[file_path_splitted.size() - 1]
                  << std::endl;
    } else if (number_of_digits >= 6) {
        std::cout << std::setw(0) << round(float(size_in_bytes) / 10e5) << "MB" << std::setw(20 - number_of_digits + 6)
                  << "  "
                  << file_path_splitted[file_path_splitted.size() - 1]
                  << std::endl;
    } else if (number_of_digits >= 3) {
        std::cout << std::setw(0) << round(float(size_in_bytes) / 10e2) << "KB" << std::setw(20 - number_of_digits + 3)
                  << "  "
                  << file_path_splitted[file_path_splitted.size() - 1]
                  << std::endl;
    } else {
        std::cout << std::setw(0) << size_in_bytes << "b" << std::setw(20 - number_of_digits + 1) << "  "
                  << file_path_splitted[file_path_splitted.size() - 1]
                  << std::endl;
    }
}


std::vector<std::string> getSubPaths(const filename &file_path) {
    std::vector<std::string> file_path_splitted;
    boost::algorithm::split(file_path_splitted, file_path, [](char c) {
        return c == fs::path::preferred_separator;
    });
    return file_path_splitted;
}


int main() {
    auto list_of_pairs = getAllInDirectory();
    printSizeTree(list_of_pairs);
    return 0;
}


