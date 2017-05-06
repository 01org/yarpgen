/*
Copyright (c) 2017, Intel Corporation

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <map>

namespace yarpgen {

struct Options {
    enum StandardID {
        C99, C11, MAX_CStandardID,
        CXX98, CXX03, CXX11, CXX14, CXX17, MAX_CXXStandardID,
        OpenCL_1_0, OpenCL_1_1, OpenCL_1_2, OpenCL_2_0, OpenCL_2_1, OpenCL_2_2, MAX_OpenCLStandardID
    };

    static const std::map<std::string, StandardID> str_to_standard;

    Options() : standard_id(CXX14), mode_64bit(true) {}

    StandardID standard_id;
    bool mode_64bit;
};

extern Options *options;
}