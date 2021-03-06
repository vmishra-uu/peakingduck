////////////////////////////////////////////////////////////////////
//                                                                //
//    Copyright (c) 2019-20, UK Atomic Energy Authority (UKAEA)   //
//                                                                //
////////////////////////////////////////////////////////////////////


/*!
    @file
    Defines the file only utilities of the library.

    @copyright UK Atomic Energy Authority (UKAEA) - 2019-20
*/
#ifndef UTIL_FILE_HPP
#define UTIL_FILE_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>

#include "common.hpp"
#include "util/stream.hpp"

PEAKINGDUCK_NAMESPACE_START(peakingduck)
PEAKINGDUCK_NAMESPACE_START(util)
    
    /*!
        a function to check if a file exists on disk (fast approach)
        @brief uses POSIX stat to check
        @param filename the name of the file.
        @return True if file exists, false otherwise
    */
    inline bool file_exists_quick(const std::string& filename){
        struct stat buffer;
        return (stat (filename.c_str(), &buffer) == 0);
    }
    
    /*!
        A function to check if a file exists on disk (not so quick approach, but still fast)
        @brief uses C file
        @param filename the name of the file.
        @return True if file exists, false otherwise
    */
    inline bool file_exists(const std::string& filename) {
        if (FILE *file = fopen(filename.c_str(), "r")){
            fclose(file);
            return true;
        }
        else{
            return false;
        }
    }
    
PEAKINGDUCK_NAMESPACE_END //util
PEAKINGDUCK_NAMESPACE_END //peakingduck

#endif //UTIL_FILE_HPP
