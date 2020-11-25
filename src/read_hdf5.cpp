#include "format_conversion.h"

using namespace H5;

struct segspec
{
    std::vector<std::string> *names;
    std::vector<SEGSPEC*> *heads;
    std::vector<float* > *data;
};

struct sac
{
    std::vector<std::string> *names;
    std::vector<SACHEAD*> *heads;
    std::vector<float* > *data;
};

static int getAllData2Spec(H5Object &obj, const H5std_string attrName,
            const H5O_info2_t *oInfo, void *operatorData)
{
    auto names = static_cast<segspec*>(operatorData)->names;
    auto heads = static_cast<segspec*>(operatorData)->heads;
    auto data = static_cast<segspec*>(operatorData)->data;
    if (attrName.find("spec") != attrName.npos)
    {
        names->push_back(attrName);
        DataSet dataset = obj.openDataSet(attrName);
        DataSpace dataspace = dataset.getSpace();
        hsize_t dims_out[1];
        dataspace.getSimpleExtentDims(dims_out, nullptr);
        float *dataout = new float[dims_out[0]];
        dataset.read((void*)(dataout), PredType::NATIVE_FLOAT);
        data->push_back(dataout);
        
        SEGSPEC* hd = new SEGSPEC;
        Attribute head = dataset.openAttribute("head");
        head.read(head.getDataType(), (void*)(hd));
        heads->push_back(hd); 
    }
    return 0;
}

static int getAllData2Sac(H5Object &obj, const H5std_string attrName,
            const H5O_info2_t *oInfo, void *operatorData)
{
    auto names = static_cast<sac*>(operatorData)->names;
    auto heads = static_cast<sac*>(operatorData)->heads;
    auto data = static_cast<sac*>(operatorData)->data;
    if (attrName.find("sac") != attrName.npos)
    {
        names->push_back(attrName);
        DataSet dataset = obj.openDataSet(attrName);
        DataSpace dataspace = dataset.getSpace();
        hsize_t dims_out[1];
        dataspace.getSimpleExtentDims(dims_out, nullptr);
        float *dataout = new float[dims_out[0]];
        dataset.read((void*)(dataout), PredType::NATIVE_FLOAT);
        data->push_back(dataout);
        
        SACHEAD* hd = new SACHEAD;
        Attribute head = dataset.openAttribute("head");
        head.read(head.getDataType(), (void*)(hd));
        heads->push_back(hd); 
    }
    return 0;
}


void readAllData2Spec(const std::string& filename, std::vector<std::string>* names,
                std::vector<SEGSPEC*>* heads, std::vector<float* >* data)
{
    segspec seg = {names, heads, data};
    H5File f(filename, H5F_ACC_RDONLY);
    f.visit(H5_INDEX_NAME, H5_ITER_INC, getAllData2Spec, (void*)&seg, H5O_INFO_BASIC);
    f.close();
}

void readAllData2Sac(const std::string& filename, std::vector<std::string>* names,
                std::vector<SACHEAD*>* heads, std::vector<float* >* data)
{
    sac s = {names, heads, data};
    H5File f(filename, H5F_ACC_RDONLY);
    f.visit(H5_INDEX_NAME, H5_ITER_INC, getAllData2Sac, (void*)&s, H5O_INFO_BASIC);
    f.close();
}

void readData2Spec(const std::string& filename, std::string& stationName,
            const std::string& locName, int year, int day,
            std::string& name,
            SEGSPEC* head, 
            float* data)
{
    H5File f(filename, H5F_ACC_RDONLY);
    Group *station = nullptr;
    DataSet *dataset = nullptr;
    if (pathExists(f.getId(), stationName))
        station = new Group(f.openGroup(stationName));
    else
    {
        std::cerr << "Group not found\n";
        std::exit(EXIT_FAILURE);
    }
    //Z10C.01.2018.001.spec
    std::string temp = std::to_string(day);
    std::string dayStr = "";
    for (int i = 0; i < 3 - temp.size(); ++i)
        dayStr.append("0");
    dayStr.append(temp);
    name = stationName.append(".").append(locName).append(".").append(
                std::to_string(year)).append(".").append(dayStr).append(".spec");
    if (pathExists(station->getId(), name))
        dataset = new DataSet(station->openDataSet(name));
    else
    {
        std::cerr << "Dataset not found\n";
        std::exit(EXIT_FAILURE);
    }
    DataSpace dataspace = dataset->getSpace();
    hsize_t dims_out[1];
    dataspace.getSimpleExtentDims(dims_out, nullptr);
    data = new float[dims_out[0]];
    dataset->read((void*)(data), PredType::NATIVE_FLOAT);
    
    head = new SEGSPEC;
    Attribute hd = dataset->openAttribute("head");
    hd.read(hd.getDataType(), (void*)(head));
    f.close();
}