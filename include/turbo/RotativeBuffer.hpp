

#ifndef __TURBO_ENGINE_ROTATIVEBUFFER_HPP__
#define __TURBO_ENGINE_ROTATIVEBUFFER_HPP__

#include <stdexcept>
#include <sstream>
#include <string.h>

namespace turbo {
    template<typename T = unsigned char>
    class RotativeBuffer {
    public:
        RotativeBuffer(const T &initial_value, unsigned short size) {
            if (size <= 0) {
                throw std::runtime_error("Rotative buffer size must be >= 0");
            }
            this->size = size;
            this->buffer = new T[size];
            for (unsigned short i = 0; i < size; i++) {
                this->buffer[i] = initial_value;
            }
        }
        RotativeBuffer() {
            this->size = 10;
            this->buffer = new T[10]();
        }
        ~RotativeBuffer() {
            delete this->buffer;
        }

        T &get_data(unsigned short position) const {
            if (position >= size) {
                throw std::out_of_range("Rotative buffer is too small");
            } else if (position < 0) {
                throw std::out_of_range("position must be >= 0");
            }
            return this->buffer[position];
        }
        T* get_array() const {
            T* res = new T[size];
            unsigned short ptr = 0;
            for (int i = position; i < size; i++) {
                res[ptr] = this->buffer[i];
                ptr++;
            }
            for (int i = 0; i < position; i++) {
                res[ptr] = this->buffer[i];
                ptr++;
            }
            return res;
        }
        void turn() {
            this->position++;
            if (this->position > size)
                this->position = 0;
        }
        void set_first(const T &val) {
            this->buffer[position] = val;
        }
        void set_last(const T &val) {
            if (position >= 0) {
                this->buffer[position - 1] = val;
            } else {
                this->buffer[size - 1] = val;
            }
        }

        unsigned short get_size() const {
            return this->size;
        }

        const T &operator[] (unsigned short pos) {
            return this->get_data(pos);
        }

        operator const char* () {
            std::ostringstream strstr;
            strstr << "{";

            for (int i = position; i < size; i++) {
                strstr << this->buffer[i] << ",";
            }
            for (int i = 0; i < position - 1; i++) {
                strstr << this->buffer[i] << ",";
            }
            if (position != 0)
                strstr << this->buffer[position - 1];
            strstr << "}";
            const char* ret = strstr.str().c_str();
            ret = strdup(ret);
            return ret;
        }

    protected:
        T* buffer = nullptr;
        unsigned short size;
        unsigned short position = 0;
    };
}

#endif
