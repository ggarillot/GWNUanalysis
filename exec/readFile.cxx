#include "DataFileReader.h"
#include "json.hpp"

#include <TColor.h>
#include <bitset>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <set>
#include <string>
#include <vector>

#include <CLI11.hpp>

namespace fs = std::filesystem;

void readFile(fs::path path, std::string tdcConfigFileName, fs::path outputDirectory, bool isDebug)
{
    if (path.extension() != ".bin")
        return;

    std::cout << "Processing " << path.string() << " ..." << std::endl;

    std::string outputFileName = path.stem().string() + ".root";

    auto dataFileStream = std::fstream(path.string(), std::ios::binary | std::ios::in);

    if (!dataFileStream.is_open())
    {
        std::cout << "ERROR : file " << path.string() << "cannot be opened" << std::endl;
        return;
    }

    std::unique_ptr<DataFileReader> dataFileReader = nullptr;
    std::unique_ptr<std::ostream>   debugStream = nullptr;

    if (isDebug)
    {
        std::string outputDebug = path.stem().string() + ".txt";
        debugStream = std::make_unique<std::ofstream>(outputDebug);
    }

    dataFileReader = std::make_unique<DataFileReader>(debugStream.get());

    std::ifstream configFile(tdcConfigFileName);
    const auto    configJson = nlohmann::json::parse(configFile);

    const auto TDCconfigJson = configJson.at("TDCconfig");

    dataFileReader->readTDCConfig(TDCconfigJson);

    dataFileReader->openOutputFile(outputFileName);

    try
    {
        dataFileReader->readDataFile(dataFileStream);
        fs::rename(outputFileName, outputDirectory / outputFileName);
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
        fs::remove(outputFileName);
    }
    dataFileReader->closeOutputFile();
}

int main(int argc, char** argv)
{
    CLI::App app;

    std::string inputName{};
    std::string outputDirectoryName{};
    std::string geometryFileName{};

    bool isDebug = false;

    app.add_option("-i", inputName, "input file or directory")->required();
    app.add_option("-o", outputDirectoryName, "output directory")->default_val(".");
    app.add_option("-g", geometryFileName, "geometry file name")->required();
    app.add_flag("-t", isDebug, "enable text debug");

    CLI11_PARSE(app, argc, argv);

    fs::path inputFileOrDirectoryName{inputName};
    fs::path geometryFileNamePath{geometryFileName};

    fs::path outputDirectory{outputDirectoryName};

    if (!fs::exists(outputDirectory))
    {
        std::cout << "outputDirectory not existing : creating " << outputDirectory << std::endl;
        fs::create_directories(outputDirectory);
    }

    if (!fs::exists(geometryFileNamePath))
    {
        std::cout << "ERROR : geometry file not existing" << std::endl;
        return 1;
    }

    if (fs::exists(inputFileOrDirectoryName))
    {
        if (fs::is_directory(inputFileOrDirectoryName))
        {
            for (auto const& entry : fs::directory_iterator{inputFileOrDirectoryName})
            {
                if (!entry.is_regular_file())
                    continue;

                readFile(entry.path(), geometryFileName, outputDirectory, isDebug);
            }
        }
        else if (fs::is_regular_file(inputFileOrDirectoryName))
        {
            readFile(inputFileOrDirectoryName, geometryFileName, outputDirectory, isDebug);
        }
    }

    return 0;
}
