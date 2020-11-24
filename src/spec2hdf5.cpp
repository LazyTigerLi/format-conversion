#include "format_conversion.h"

namespace fs = std::filesystem;
using namespace H5;

static int yearBegin = INT_MIN, yearEnd = INT_MAX, dayBegin = INT_MIN, dayEnd = INT_MAX;
static char component = 'Z';

float* readSpec(std::string filename, SEGSPEC *head)
{
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    file.read((char*)head, sizeof(SEGSPEC));
    int count = head->nspec * head->nseg * 2;
    float *data = new float[count];
    file.read((char*)data, sizeof(float) * count);
    file.close();
    return data;
}

static void dfsWrite(const fs::path& p, H5Object *file, bool isRoot)
{
    std::string name = p.filename().c_str();
    fs::directory_entry entry(p);
    if (entry.status().type() == fs::file_type::directory)
    {
        Group *group = nullptr;
        if (isRoot)                                 // first level, e.g. TA
            group = static_cast<Group*>(file);
        else                                        // second level, e.g. TA.121A
        {
            int i = 0;
            for (i = name.size() - 1; i >= 0; --i)
                if (name[i] == '.')
                    break;
            std::string groupName = name.substr(i + 1, name.size() - i);     // name group as "121A"
            group = new Group(file->createGroup(groupName));
        }
	    fs::directory_iterator filelist(p);
	    for (auto& it: filelist)
        {
            fs::path temp = p / it.path().filename();
            dfsWrite(temp, group, false);
        }
        delete group;
    }
    else                                    // sgespec file, its name is like "TA.Z10C.01.2018.001.LHZ.segspec"
    {
        int i = 0;
        int year = std::stoi(name.substr(11, 4));
        int day = std::stoi(name.substr(16, 3));
        int comp = name[22];
        if (year < yearBegin)
            yearBegin = year;
        if (year > yearEnd)
            yearEnd = year;
        if (year == yearBegin && day < dayBegin)
            dayBegin = day;
        if (year == yearEnd && day > dayEnd)
            dayEnd = day;
        std::string datasetName = name.substr(3, 11).append(".spec");
        // name dataset as "Z10C.01.2018.001.spec", TA and Z are common attributes
        SEGSPEC *head = new SEGSPEC;
        float *data = readSpec(p, head);
        hsize_t count = head->nspec * head->nseg * 2;
        hsize_t dim[1] = {count};
        DataSpace space(1, dim);
        DataSet *dataset = new DataSet(file->createDataSet(
                datasetName, PredType::NATIVE_FLOAT, space));
        dataset->write(data, PredType::NATIVE_FLOAT);

        dim[1] = 1;
        space = DataSpace(1, dim);
        hsize_t dim1[1] = {sizeof(SEGSPEC)};
        Attribute attr = dataset->createAttribute("head", ArrayType(PredType::NATIVE_CHAR, 1, dim1), space);
        attr.write(ArrayType(PredType::NATIVE_CHAR, 1, dim1), (void*)head);
        delete head;
        delete data;
        delete dataset;
    }
}

void spec2hdf5(const fs::path& p)
{
    if (!fs::exists(p))
    {
        std::cerr << "No path" << std::endl;
        exit(0);
    }
    H5File *outfile = new H5File("result.h5", H5F_ACC_TRUNC);
    dfsWrite(p, outfile, true);
    outfile->close();
    delete outfile;
}

// int main(int argc, char *argv[])
// {
//     createHeadType();
//     spec2hdf5(fs::path(argv[1]));
//     return 0;
// }
