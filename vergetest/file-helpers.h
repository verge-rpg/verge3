#pragma once

#include "../src/log.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

inline bool _test_file_exists(const std::string& name)
{
    const std::ifstream f(name.c_str());
    return f.good();
}

inline bool dir_exists(const std::string& dirName_in)
{
    const DWORD file_type = GetFileAttributesA(dirName_in.c_str());
    if (file_type == INVALID_FILE_ATTRIBUTES)
    {
        return false; // something is wrong with your path!
    }

    if (file_type & FILE_ATTRIBUTE_DIRECTORY)
    {
        return true; // this is a directory!
    }

    return false; // this is not a directory!
}

inline std::string read_file_into_string(const std::string& path)
{
    std::ifstream input_file(path);
    if (!input_file.is_open())
    {
        std::cerr << "Could not open the file - '" << path << "'" << std::endl;
        exit(EXIT_FAILURE);
    }
    return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}

inline bool compare_two_files(std::string filepath_1, std::string filepath_2)
{
    std::istringstream file_1;
    file_1.str(read_file_into_string(filepath_1));
    std::istringstream file_2;
    file_2.str(read_file_into_string(filepath_2));

    std::string line_a;
    std::string line_b;
    while (std::getline(file_1, line_a))
    {
        std::getline(file_2, line_b);

        if (line_a != line_b)
        {
            MESSAGE("files mismatched on: ");
            MESSAGE(line_a);
            MESSAGE(line_b);
            return false;
        }
    }

    return true;
}

inline void delete_file_if_it_exists(std::string filename)
{
    if (_test_file_exists(filename))
    {
        LOG(filename + " exists.  Let's delete it.");

        int errno1 = remove(filename.c_str());
        if (errno1 != 0)
        {
            std::string msg
                = "Error deleting " + filename + " from previous test run: error code (" + std::to_string(errno1) + ")";
            MESSAGE(msg);
            LOG(msg);
            perror(msg.c_str());
            FAIL(msg);
        }
        else
        {
            std::string msg = filename + " from previous test run successfully deleted";
            MESSAGE(msg);
            LOG(msg);
            puts(msg.c_str());
        }
    }
}