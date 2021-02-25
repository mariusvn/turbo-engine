
#ifndef __TURBO_ENGINE_INSPECTOR_HPP__
#define __TURBO_ENGINE_INSPECTOR_HPP__

namespace debug {
    enum inspector_types {
        /// data type: float*
        FLOAT = 0,
        /// data type: int*
        INT = 1,
        /// data type: char**
        CSTRING = 2,
        /// data type: vector<int>*
        INT_VECTOR = 3,
        /// data type: vector<float>*
        FLOAT_VECTOR = 4,
        /// data type: float** (null terminating array of pointer to target)
        FLOAT2 = 5,
        /// data type: float** (null terminating array of pointer to target)
        FLOAT3 = 6,
        /// data type: float** (null terminating array of pointer to target)
        FLOAT4 = 7,
        /// data type: int** (null terminating array of pointer to target)
        INT2 = 8,
        /// data type: int** (null terminating array of pointer to target)
        INT3 = 9,
        /// data type: int** (null terminating array of pointer to target)
        INT4 = 10,
        /// data type: float* (min/max required)
        FLOAT_RANGE = 11,
        /// data type: int* (min/max required)
        INT_RANGE = 12,
        /// data type: const char*
        UI_TEXT = 13,
        /// data type: no used (void|null)
        UI_SEPARATOR = 14,
    };

    /**
     * @brief DEBUG ONLY - Structure that describe how the inspector should display and
     * observe the target variable
     */
    struct InspectorObserver {
        InspectorObserver(void* data, inspector_types type, const char* name):
            data(data), type(type), name(name) {}

        void* data;
        inspector_types type;
        const char* name;
        double min = 0;
        double max = 0;
    };
}

#endif
