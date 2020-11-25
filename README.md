# format-conversion

支持sac文件到hdf5的转换，以及hdf5文件的读取

### 使用方法

```
cd format-conversion
./cmake .
make
```

即可得到格式转换的静态库`libformat_conversion.a`。也可修改`CMakeLists.txt`生成动态库。

注意在构建项目前需要将`CMakeList.txt`中的`set(PROJECT_PATH /home/user/format-conversion)`替换为项目实际路径。

另外要求gcc的版本不低于gcc-8，且已安装hdf5相关的库。对于debian系的发行版，可尝试`sudo apt install hdf5`进行安装。



目前提供的接口在`include/format_conversion.h`中。请先暂时不要使用segspec格式相关的接口（尚未进行测试）。使用时，需要将`format_conversion.h`添加到头文件中，并且在编译时需要链接`libformat_conversion.a`和`libstdc++fs.a`这两个库。



### 接口说明

- void sac2hdf5(const std::filesystem::path& p)

  将路径p下的所有sac格式的文件转换成一个hdf5文件，例如`sac2hdf5(std::filesystem::path("TA"))`。

- void data2hdf5(const std::vector\<std::filesystem::path\>& p, 

  ​							const std::vector<SACHEAD*>& head,

  ​        					const std::vector<float*>& data, 

  ​        					const std::string& h5filename)

  将若干条sac数据转换成一个hdf5文件。p表示每条sac数据在hdf5文件中的位置，head和data分别表示每条sac数据的头部和数据，h5filename表示目标hdf5文件的文件名。

  例如，如果希望最终的hdf5文件是如下结构：

  TA

  ​		TA.121A

  ​					TA.121A.TA.121A.--.LHZ.2019.001.sacum

  ​					TA.121A.TA.121A.--.LHZ.2019.002.sacum

  ​		TA.XXXX

  ​					....

  则输入的p应为{ "TA/TA.121A/TA.121A.TA.121A.--.LHZ.2019.001.sacum",  "TA/TA.121A/TA.121A.TA.121A.--.LHZ.2019.002.sacum", ...}，类似于此，只是vector中的数据应为path类型。

- void data2hdf5(const file_tree \*tree, const std::vector<SACHEAD*>& head,

  ​        					const std::vector<float*>& data, 

  ​        					const std::string& h5filename)

  该函数的功能与上个函数一致，只是尝试对性能进行优化。在调用该函数前，需要先调用函数file_tree* createFileTree(const std::vector\<std::filesystem::path\>& filename)，将要写入的文件路径进行分析，得到一棵file_tree，然后再调用data2hdf5函数。这是为了减少H5::group的open和close次数，来尝试减少开销。

- void readAllData2Sac(const std::string& filename, std::vector\<std::string>* names,

  ​        								std::vector<SACHEAD*>* heads, std::vector<float* >* data)

  该函数将文件名为filename的hdf5文件中的sac数据全部读出。