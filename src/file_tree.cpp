#include "../include/format_conversion.h"

namespace fs = std::filesystem;

file_tree* createFileTree(const std::vector<fs::path>& filename)
{
    file_tree *tree = new file_tree;
    for (size_t i = 0; i < filename.size(); ++i)
    {
        auto node = tree;
        fs::path temp(filename[i]);
        temp.remove_filename();
        for (auto it = temp.begin(); it != temp.end(); ++it)
        {
            if (it->native() == "")
                break;
            if (node->find(it->native()) == node->end())
            {
                auto newNode = new file_tree;
                node->insert({it->native(), (void*)newNode});
            }
            node = static_cast<file_tree*>(std::get<void*>((*node)[it->native()]));
        }
        if (node->find(filename[i].filename()) == node->end())
            node->insert({filename[i].filename(), i});
    }
    return tree;
}