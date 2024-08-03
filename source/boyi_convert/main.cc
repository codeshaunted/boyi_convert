// codeshaunted - boyi_convert
// source/boyi_convert/main.cc
// contains entry point
// Copyright 2024 codeshaunted
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org / licenses / LICENSE - 2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.

#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

#include "bimg/bimg.h"
#include "bimg/decode.h"
#include "bx/bx.h"
#include "bx/allocator.h"
#include "bx/file.h"
#include "zstd.h"
#include "cxxopts.hpp"

#define BOYI_HEADER_SIZE 36

namespace fs = std::filesystem;

std::vector<fs::path> get_image_files(const fs::path& directory, bool recursive) {
    std::vector<fs::path> image_files;

    if (recursive) {
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".image") {
                image_files.push_back(entry.path());
            }
        }
    } else {
        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".image") {
                image_files.push_back(entry.path());
            }
        }
    }
    

    return image_files;
}

void convert_image(const std::string& input_path, const std::string& output_path, size_t i, size_t total) {
    std::ifstream input_file(input_path, std::ios::binary | std::ios::ate);
    if (!input_file.is_open()) {
        std::cout << "[" << i << "/" << total << "] error opening input file: '"<< input_path << "'" << std::endl;
        return;
    }

    size_t input_size = input_file.tellg();
    input_file.seekg(0, std::ios::beg);

    if (input_size == 0) {
        std::cout << "[" << i << "/" << total << "] failed to convert, file is empty: '" << input_path << "'" << std::endl;
        return;
    }

    uint8_t* input_data = new uint8_t[input_size];
    if (!input_data) {
        std::cout << "[" << i << "/" << total << "] memory allocation failed" << std::endl;
        input_file.close();
        return;
    }

    // read the file into the buffer
    if (!input_file.read(reinterpret_cast<char*>(input_data), input_size)) {
        std::cout << "[" << i << "/" << total << "] error reading input file" << std::endl;
        delete[] input_data;
        input_file.close();
        return;
    }

    size_t decompressed_size = ZSTD_getFrameContentSize(input_data + BOYI_HEADER_SIZE, input_size - BOYI_HEADER_SIZE);
    uint8_t* decompressed_data = new uint8_t[decompressed_size];
    ZSTD_decompress(decompressed_data, decompressed_size, input_data + BOYI_HEADER_SIZE, input_size - BOYI_HEADER_SIZE);

    bx::DefaultAllocator allocator;
    bimg::ImageContainer* output = bimg::imageParse(&allocator, decompressed_data, decompressed_size, bimg::TextureFormat::Count);
    delete[] input_data;

    bx::FileWriter writer;
    bx::open(&writer, output_path.c_str(), false);
    bimg::ImageMip mip;
	bimg::imageGetRawData(*output, 0, 0, output->m_data, output->m_size, mip);
	bimg::imageWritePng(&writer, mip.m_width, mip.m_height, mip.m_width*4, mip.m_data, output->m_format, false);

    bx::close(&writer);

    std::cout << "[" << i << "/" << total << "] converted image successfully saved to: '" << output_path << "'" << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        cxxopts::Options options("boyi_convert", "Convert World of Goo 2 .image files");

        options.add_options()
            ("h,help", "Print usage")
            ("input_path", "Input file or directory path", cxxopts::value<std::string>())
            ("o,output_path", "Output directory path", cxxopts::value<std::string>())
            ("r,recursive", "Recursively convert files in nested directories", cxxopts::value<bool>()->default_value("true")->implicit_value("true"))
        ;
        options.parse_positional({"input_path"});
        auto options_result = options.parse(argc, argv);

        if (options_result.count("help")) {
            std::cout << options.help() << std::endl;
            return 0;
        }

        if (options_result.count("input_path")) {
            std::string input_path = options_result["input_path"].as<std::string>();
            fs::path input_fs_path(input_path);
            std::vector<fs::path> image_files;

            if (fs::is_directory(input_fs_path)) {
                image_files = get_image_files(input_fs_path, options_result.count("recursive"));
            } else if (fs::is_regular_file(input_fs_path) && input_fs_path.extension() == ".image") {
                image_files.push_back(input_fs_path);
            } else {
                std::cerr << "invalid input path: " << input_path << std::endl;
                return 1;
            }

            std::string output_directory = options_result.count("output_path") ? options_result["output_path"].as<std::string>() : "";
            if (!output_directory.empty() && !fs::is_directory(output_directory)) {
                std::cerr << "invalid output directory: " << output_directory << std::endl;
                return 1;
            }

            size_t i = 1;
            for (const auto& image_file : image_files) {
                fs::path output_path;
                if (!output_directory.empty()) {
                    output_path = (fs::path(output_directory) / image_file.parent_path() / image_file.stem());
                } else {
                    output_path = (image_file.parent_path() / image_file.stem());
                }

                if (output_path.has_parent_path()) {
                    fs::create_directories(output_path.parent_path());
                }
                std::string output_path_string = output_path.string() + ".png";
                convert_image(image_file.string(), output_path_string, i, image_files.size());
                ++i;
            }

            return 0;
        } else {
            std::cerr << "input_path is required" << std::endl;
            std::cerr << options.help() << std::endl;
        }
    } catch (const cxxopts::exceptions::exception& e) {
        std::cerr << "error parsing options: " << e.what() << std::endl;
        return 1;
    }
}
