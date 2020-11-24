#include "format_conversion.h"

namespace fs = std::filesystem;
using namespace H5;

float* readSac(std::string filename, SACHEAD *head)
{
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    file.read((char*)head, sizeof(SACHEAD));
    int count = head->npts;
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
        if (isRoot)
            group = static_cast<Group*>(file);
        else
        {
            std::string groupName = name;
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
    else                                
    {
        std::string datasetName = name;
        SACHEAD *head = new SACHEAD;
        float *data = readSac(p, head);
        DSetCreatPropList  *pList = new DSetCreatPropList;
        pList->setDeflate(9);
        hsize_t dim[1] = {hsize_t(head->npts)};
        DataSpace space(1, dim);
        DataSet *dataset = new DataSet(file->createDataSet(
                datasetName, PredType::NATIVE_FLOAT, space, *pList));
        dataset->write(data, PredType::NATIVE_FLOAT);
        dim[1] = 1;
        space = DataSpace(1, dim);
        hsize_t dim1[1] = {sizeof(SACHEAD)};
        Attribute attr = dataset->createAttribute("head", ArrayType(PredType::NATIVE_CHAR, 1, dim1), space);
        attr.write(ArrayType(PredType::NATIVE_CHAR, 1, dim1), (void*)head);
        delete head;
        delete data;
        delete dataset;
        delete pList;
    }
}

void sac2hdf5(const fs::path& p)
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

void data2hdf5(const std::vector<fs::path>& p, const std::vector<SACHEAD*>& head,
                const std::vector<float*>& data, 
                const std::string& h5filename)
{
    H5File *file = new H5File(h5filename, H5F_ACC_RDWR | H5F_ACC_CREAT);
    for (size_t i = 0; i < p.size(); ++i)
    {
        Group *group = file;
        fs::path temp(p[i]);
        temp.remove_filename();
        std::vector<Group*> groups;
        for (auto it = temp.begin(); it != temp.end(); ++it)
        {
            if (it->native() == "")
                    break;
            if (pathExists(group->getId(), it->c_str()))
            {
                group = new Group(group->openGroup(it->c_str()));
            }
            else
                group = new Group(group->createGroup(it->c_str()));
            groups.push_back(group);
        }
        hsize_t dim[1] = {hsize_t(head[i]->npts)};
        // DSetCreatPropList  *pList = new DSetCreatPropList;
        // hsize_t d[2] = {30, 30};
        // pList->setChunk(2, d);
        // pList->setDeflate(6);
        DataSpace space(1, dim);
        if (pathExists(group->getId(), p[i].filename().native()))
            H5Ldelete(group->getId(), p[i].filename().c_str(), H5P_DEFAULT);
        DataSet *dataset = new DataSet(group->createDataSet(
                p[i].filename().c_str(), PredType::NATIVE_FLOAT, space));
        dataset->write(data[i], PredType::NATIVE_FLOAT);
        dim[0] = 1;
        hsize_t dim1[1] = {sizeof(SACHEAD)};
        space = DataSpace(1, dim);
        Attribute attr = dataset->createAttribute("head", ArrayType(PredType::NATIVE_B8, 1, dim1), space);
        attr.write(ArrayType(PredType::NATIVE_B8, 1, dim1), (void*)head[i]);
        // delete pList;
        delete dataset;
        for (auto g: groups)
            delete g;
    }
    delete file;
}



static void dfsWrite(const file_tree *tree, const std::vector<SACHEAD*>& head,
                const std::vector<float*>& data, 
                Group *group)
{
    for (auto node = tree->begin(); node != tree->end(); ++node)
    {
        if (node->second.index() == 0)
        {
            Group *newGroup = nullptr;
            if (!pathExists(group->getId(), node->first))
                newGroup = new Group(group->createGroup(node->first));
            else
                newGroup = new Group(group->openGroup(node->first));
            dfsWrite(static_cast<file_tree*>(std::get<void*>(node->second))
            , head, data, newGroup);
            delete newGroup;
        }
        else
        {
            auto i = std::get<size_t>(node->second);
            hsize_t dim[1] = {hsize_t(head[i]->npts)};
            DataSpace space(1, dim);
            if (pathExists(group->getId(), node->first))
                H5Ldelete(group->getId(), node->first.data(), H5P_DEFAULT);
            DataSet *dataset = new DataSet(group->createDataSet(
                    node->first, PredType::NATIVE_FLOAT, space));
            dataset->write(data[i], PredType::NATIVE_FLOAT);
            dim[0] = 1;
            hsize_t dim1[1] = {sizeof(SACHEAD)};
            space = DataSpace(1, dim);
            Attribute attr = dataset->createAttribute("head", ArrayType(PredType::NATIVE_B8, 1, dim1), space);
            attr.write(ArrayType(PredType::NATIVE_B8, 1, dim1), (void*)head[i]);
            delete dataset;
        }
    }
}


void data2hdf5(const file_tree *tree, const std::vector<SACHEAD*>& head,
                const std::vector<float*>& data, 
                const std::string& h5filename)
{
    H5File *file = new H5File(h5filename, H5F_ACC_RDWR | H5F_ACC_CREAT);
    dfsWrite(tree, head, data, file);
    delete file;
}