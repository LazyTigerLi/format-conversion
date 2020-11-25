#ifndef FORMAT_CONVERSION
#define FORMAT_CONVERSION 1

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "H5Cpp.h"
#include "sac.h"
#include "segspec.h"


typedef std::unordered_map<std::string, std::variant<void*, size_t>> file_tree;

float* readSac(std::string filename, SACHEAD *head);
void sac2hdf5(const std::filesystem::path& p);
void data2hdf5(const std::vector<std::filesystem::path>& p, const std::vector<SACHEAD*>& head,
                const std::vector<float*>& data, 
                const std::string& h5filename);
void data2hdf5(const file_tree *tree, const std::vector<SACHEAD*>& head,
                const std::vector<float*>& data, 
                const std::string& h5filename);
float* readSpec(std::string filename, SEGSPEC *head);
void spec2hdf5(const std::filesystem::path& p);
void readAllData2Spec(const std::string& filename, std::vector<std::string>* names,
                std::vector<SEGSPEC*>* heads, std::vector<float* >* data);
void readData2Spec(const std::string& filename, std::string& stationName,
            const std::string& locName, int year, int day,
            std::string& name,
            SEGSPEC* head, 
            float* data);
void readAllData2Sac(const std::string& filename, std::vector<std::string>* names,
                std::vector<SACHEAD*>* heads, std::vector<float* >* data);

file_tree* createFileTree(const std::vector<std::filesystem::path>& filename);

inline bool pathExists(hid_t id, const std::string& path)
{
    return H5Lexists(id, path.c_str(), H5P_DEFAULT ) > 0;
}

#endif