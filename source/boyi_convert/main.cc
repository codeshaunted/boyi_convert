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
#include <bimg/bimg.h>
#include <bimg/decode.h>
#include <bimg/encode.h>
#include <bx/bx.h>
#include <bx/allocator.h>
#include <bx/file.h>
#include "zstd.h"
#include "cxxopts.hpp"

#define BOYI_HEADER_SIZE 36

int main(int argc, char* argv[]) {
    try {
        cxxopts::Options options("boyi_convert", "Convert World of Goo 2 .image files");

        options.add_options()
            ("h,help", "Print usage")
            ("input_path", "Input file path", cxxopts::value<std::string>())
            ("o,output_path", "Output file path", cxxopts::value<std::string>())
        ;
        options.parse_positional({"input_path"});
        auto options_result = options.parse(argc, argv);

        if (options_result.count("help")) {
            std::cout << options.help() << std::endl;
            return 0;
        }

        if (options_result.count("input_path")) {
            std::string input_path = options_result["input_path"].as<std::string>();

            std::ifstream input_file(input_path, std::ios::binary | std::ios::ate);
            if (!input_file.is_open()) {
                std::cout << "error opening input file: " << input_path << std::endl;
                return 1;
            }

            size_t input_size = input_file.tellg();
            input_file.seekg(0, std::ios::beg);

            uint8_t* input_data = new uint8_t[input_size];
            if (!input_data) {
                std::cout << "memory allocation failed" << std::endl;
                input_file.close();
                return 1;
            }

            // read the file into the buffer
            if (!input_file.read(reinterpret_cast<char*>(input_data), input_size)) {
                std::cout << "error reading input file" << std::endl;
                delete[] input_data;
                input_file.close();
                return false;
            }

            size_t decompressed_size = ZSTD_getFrameContentSize(input_data + BOYI_HEADER_SIZE, input_size - BOYI_HEADER_SIZE);
            uint8_t* decompressed_data = new uint8_t[decompressed_size];
            ZSTD_decompress(decompressed_data, decompressed_size, input_data + BOYI_HEADER_SIZE, input_size - BOYI_HEADER_SIZE);

            bx::DefaultAllocator allocator;
            bimg::ImageContainer* output = bimg::imageParse(&allocator, decompressed_data, decompressed_size, bimg::TextureFormat::Count);
            delete[] input_data;

            std::string output_path;
            if (options_result.count("output_path")) {
                output_path = options_result["output_path"].as<std::string>();
            } else {
                if (input_path.ends_with(".image")) {
                    output_path = input_path.substr(0, input_path.size() - 6);
                }

                output_path += ".png";
            }

            bx::FileWriter writer;
            bx::open(&writer, output_path.c_str(), false);
            bimg::ImageMip mip;
			bimg::imageGetRawData(*output, 0, 0, output->m_data, output->m_size, mip);
			bimg::imageWritePng(&writer, mip.m_width, mip.m_height, mip.m_width*4, mip.m_data, output->m_format, false);

            bx::close(&writer);

            std::cout << "converted image successfully saved to '" << output_path << "'" << std::endl;
            return 0;
        } else {
            std::cout << "input_path is required";
            std::cout << options.help() << std::endl;
        }
    } catch (const cxxopts::exceptions::exception& e) {
        std::cout << "error parsing options: " << e.what() << std::endl;
        return 1;
    }
    


    /*
    if (argc < 1) {
        return 1;
    }

    char* input_file_path = argv[1];
    std::ifstream input_file(input_file_path, std::ios::binary | std::ios::ate);
    if (!input_file.is_open()) {
        std::cerr << "Error opening input file: " << input_file_path << std::endl;
        return 1;
    }

    // get the file size
    uint32_t input_file_size = input_file.tellg();
    input_file.seekg(0, std::ios::beg);

    // allocate memory for the buffer
    uint8_t* input_file_data = new uint8_t[input_file_size];
    if (!input_file_data) {
        std::cerr << "Memory allocation failed." << std::endl;
        input_file.close();
        return false;
    }

    // read the file into the buffer
    if (!input_file.read(reinterpret_cast<char*>(input_file_data), input_file_size)) {
        std::cerr << "Error reading input file." << std::endl;
        delete[] input_file_data;
        input_file.close();
        return false;
    }

    size_t decompressed_size = ZSTD_getFrameContentSize(input_file_data + 36, input_file_size - 36);
    uint8_t* decompressed_data = new uint8_t[decompressed_size];

    ZSTD_decompress(decompressed_data, decompressed_size, input_file_data + 36, input_file_size - 36);

    bx::DefaultAllocator allocator;

    bimg::ImageContainer* output = bimg::imageParse(&allocator, decompressed_data, decompressed_size, bimg::TextureFormat::Count);
    delete[] input_file_data;

    bx::FileWriter writer;
    bx::open(&writer, "output.png", false);
    bimg::ImageMip mip;
				bimg::imageGetRawData(*output, 0, 0, output->m_data, output->m_size, mip);
				bimg::imageWritePng(&writer
					, mip.m_width
					, mip.m_height
					, mip.m_width*4
					, mip.m_data
					, output->m_format
					, false
					);

    bx::close(&writer);

    std::cout << "DONE";

    input_file.close();



    return 0;*/
}
