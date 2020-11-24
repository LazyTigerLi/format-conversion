#include "../include/format_conversion.h"
#include <ctime>
#include <algorithm>
#include <string.h> 
#include <cassert>
// #include "format_conversion.h"

namespace fs = std::filesystem;
using namespace H5;

std::vector<SACHEAD*> heads;
std::vector<float*> allData;
std::vector<fs::path> names;


void getData(const fs::path p)
{
    std::string name = p.filename().c_str();
    fs::directory_entry entry(p);
    if (entry.status().type() == fs::file_type::directory)
    {
	    fs::directory_iterator filelist(p);
	    for (auto& it: filelist)
        {
            fs::path temp = p / it.path().filename();
            getData(temp);
        }
    }
    else                                
    {
        SACHEAD *head = new SACHEAD;
        float *data = readSac(p, head);
        heads.push_back(head);
        allData.push_back(data);
        names.push_back(p.relative_path());
    }
}

void getFilenames(const fs::path p)
{
    std::string name = p.filename().c_str();
    fs::directory_entry entry(p);
    if (entry.status().type() == fs::file_type::directory)
    {
	    fs::directory_iterator filelist(p);
	    for (auto& it: filelist)
        {
            fs::path temp = p / it.path().filename();
            getFilenames(temp);
        }
    }
    else                                
        names.push_back(p.relative_path());
}

void write2File(const std::vector<fs::path>& p, const std::vector<SACHEAD*>& head,
                const std::vector<float*>& data)
{
    for (int i = 0; i < p.size(); ++i)
    {
        std::ofstream file(fs::path("123") / p[i], std::ios::out | std::ios::binary);
        file.write((char*)head[i], sizeof(SACHEAD));
        file.write((char*)data[i], sizeof(float) * head[i]->npts);
        file.close();
    }
}

void writeTest()
{
    fs::path p("TA");
    getData(p);
    clock_t begin = clock();
    std::string filename = "result.h5";
    // data2hdf5(names, heads, allData, filename);
    auto tree = createFileTree(names);
    data2hdf5(tree, heads, allData, filename);
    clock_t end = clock();
    std::cout << (double)(end - begin) / CLOCKS_PER_SEC << std::endl;

    fs::create_directories("123/TA/TA.121A");
    // fs::create_directories("123/TA/TA.121A1");
    // fs::create_directories("123/TA/TA.121A2");
    // fs::create_directories("123/TA/TA.121A3");
    begin = clock();
    write2File(names, heads, allData);
    end = clock();
    std::cout << (double)(end - begin) / CLOCKS_PER_SEC << std::endl;

    for (int i = 0; i < heads.size(); ++i)
    {
        delete heads[i];
        delete allData[i];
    }
    names.clear();
    heads.clear();
    allData.clear();
}

void readTest()
{
    fs::path p("TA");
    clock_t begin = clock();
    getData(p);
    clock_t end = clock();
    std::cout << double(end - begin) / CLOCKS_PER_SEC << std::endl;
    // for (auto p: names)
        // std::cout << p << std::endl;
    std::string filename("result.h5");
    std::vector<std::string> n;
    std::vector<SACHEAD*> h;
    std::vector<float*> d;
    begin = clock();
    readAllData2Sac(filename, &n, &h, &d);
    end = clock();
    std::cout << double(end - begin) / CLOCKS_PER_SEC << std::endl;
    for (int i = 0; i < n.size(); ++i)
    {
        auto it = std::find(names.begin(), names.end(), n[i]);
        int index = it - names.begin();
        // std::cout << index << std::endl;
        if (n[i] == names[index])
        {
            assert(!memcmp((void*)heads[index], (void*)h[i], sizeof(SACHEAD)));
            assert(!memcmp((void*)allData[index], (void*)d[i], sizeof(float) * heads[i]->npts));
            // std::cout << memcmp((void*)heads[index], (void*)h[i], sizeof(SACHEAD)) << std::endl;
            // std::cout << memcmp((void*)allData[index], (void*)d[i], sizeof(float) * heads[i]->npts) << std::endl;
        }
    }
}

void dfs(const file_tree *tree)
{
    for (auto node = tree->begin(); node != tree->end(); ++node)
    {
        std::cout << node->first << std::endl;
        if (node->second.index() == 0)
            dfs(static_cast<file_tree*>(std::get<void*>(node->second)));
        else
            std::cout << std::get<size_t>(node->second) << std::endl;
    }
}

int main()
{
    // writeTest();
    // getFilenames(fs::path("TA"));
    // auto tree = createFileTree(names);
    // dfs(tree);

    // data2hdf5(tree, heads, allData, "result.h5");
    // std::cout << "123\n";
    readTest();
    return 0;
}