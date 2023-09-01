// https://github.com/adafruit/web-bdftopcf
//

#include <vector>
#include <codecvt>
#include <algorithm>
#include <fstream>  
#include <streambuf>

typedef std::vector<int>    int_list;
std::vector<int>   _wchars;

int init_wchar_list(int_list* list, const char* chars, int first_chr, int end_chr)
{
    int i, first = ' ', last = '~';
    int wchr_count = 0;

    if ((first_chr != 0) && (end_chr != 0))
    {
        first = first_chr;
        last = end_chr;

        if (last < first) {
            i = first;
            first = last;
            last = i;
        }

        for (i = first; i <= last; i++)
        {
            list->push_back(i);
        }
    }

    // chars are gbk/ansi coded.
    if (chars != NULL)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            
        // Convert UTF-8 string to wstring
        std::wstring wstr = converter.from_bytes(chars);

        // Store wstring as a vector of int
        for (auto it = wstr.begin(); it != wstr.end(); ++it) {
            if ((*it == L'\r') || (*it == L'\n') || (*it == L'\t'))
                continue;	// drop special chars
            list->push_back(*it);
        }
    }

    // Sort the vector
    std::sort(list->begin(), list->end());

    // Remove duplicates
    auto new_end = std::unique(list->begin(), list->end());
    list->erase(new_end, list->end());

    return list->size();
}

extern "C" {
    int char_dict_load(const char* dict_file);
    int char_dict_count();
    int char_dict_wchar_in_dict(unsigned int wchar);
}

int char_dict_load(const char* dict_file)
{
    std::ifstream input_file(dict_file);

    if (!input_file.is_open()) {
        return NULL;
    }

    // Read the entire content of the file into a std::string
    std::string file_content((std::istreambuf_iterator<char>(input_file)),
        std::istreambuf_iterator<char>());

    // Close the file
    input_file.close();

    const char* chars = file_content.c_str();
    int first_chr = ' ';
    int end_chr = '~';

    if (init_wchar_list(&_wchars, chars, first_chr, end_chr) > 0)
    {// ok
        return 1;
    }
    
    // failed
    return 0;
}

int char_dict_count()
{
    return _wchars.size();
}

int char_dict_wchar_in_dict(unsigned int wchar)
{
    auto result = std::find(_wchars.begin(), _wchars.end(), wchar);

    if (result != _wchars.end()) {
        return 1;   // founded
    }
    else {
        return 0;   // not found
    }
}

#include <iostream>
#include <sstream>
#include <string>
#include <lib/lz4.h>

extern "C" {
    void lz4_reset();
    void lz4_putUInt16(int isMSB, int value);
    int lz4_compress();
    const unsigned char* lz4_get_output(int* out_length);
}
class ByteArrayStream {
public:
    ByteArrayStream() = default;

    template <typename T>
    ByteArrayStream& operator<<(const T& value) {
        const uint8_t* begin = reinterpret_cast<const uint8_t*>(&value);
        const uint8_t* end = begin + sizeof(value);
        data_.insert(data_.end(), begin, end);
        return *this;
    }

    template <typename T>
    ByteArrayStream& operator>>(T& value) {
        if (currentPosition_ + sizeof(value) <= data_.size()) {
            std::memcpy(&value, data_.data() + currentPosition_, sizeof(value));
            currentPosition_ += sizeof(value);
        }
        return *this;
    }

    // Write a block of bytes to the stream
    size_t write(const void* data, size_t size) {
        const uint8_t* bytes = static_cast<const uint8_t*>(data);
        data_.insert(data_.end(), bytes, bytes + size);

        return size;
    }

    // Read a block of bytes from the stream
    size_t read(void* data, size_t size) {
        size_t real_size;

        if (currentPosition_ >= data_.size())
            return 0;

        if (currentPosition_ + size <= data_.size())
            real_size = size;
        else
            real_size = data_.size() - (currentPosition_ + size);

        std::memcpy(data, data_.data() + currentPosition_, real_size);
        currentPosition_ += real_size;

        return real_size;
    }

    void reset() {
        currentPosition_ = 0;
    }

    const uint8_t* getData() {
        return data_.data();
    }

    const uint8_t* getBuf() {
        return data_.data();
    }

    int getSize() {
        return data_.size();
    }
private:
    std::vector<uint8_t> data_;
    size_t currentPosition_ = 0;
};

ByteArrayStream inStream;
ByteArrayStream outStream;

/// read several bytes and store at "data", return number of actually read bytes (return only zero if end of data reached)
size_t getBytesFromIn(void* data, size_t numBytes, void* userPtr)
{
    if (data && numBytes > 0)
    {
        size_t actual = inStream.read(data, numBytes);
        return actual;
    }
    return 0;
}


/// write a block of bytes
void sendBytesToOut(const void* data, size_t numBytes, void* userPtr)
{
    if (data && numBytes > 0)
    {
        outStream.write(data, numBytes);
    }
}

void lz4_reset()
{
    inStream.reset();
    outStream.reset();
}

void lz4_putUInt16(int isMSB, int value)
{
    if (isMSB)
    {
        inStream << (uint8_t)(value >> 8);
        inStream << (uint8_t)(value & 0xff);
    }
    else
    {
        inStream << (uint8_t)(value & 0xff);
        inStream << (uint8_t)(value >> 8);
    }
}

int lz4Decompress(const uint8_t* compressedData, size_t compressedSize,
    uint8_t* decompressedData, size_t maxDecompressedSize);

int lz4_decompress(const uint8_t* compressedData, size_t compressedSize, size_t maxDecompressedSize)
{
    uint8_t* decompressedData = (uint8_t*)malloc(maxDecompressedSize);
    
    int result = LZ4_decompress_safe((const char *)compressedData, (char*)decompressedData, compressedSize, maxDecompressedSize);

    result = memcmp(decompressedData, inStream.getData(), maxDecompressedSize);

    free(decompressedData);

    return result;
}
int lz4_compress()
{
    // compress level
    unsigned short maxChainLength = 65535; // "unlimited" because search window contains only 2^16 bytes
    
    // load dictionary
    std::vector<unsigned char> preload;

    // legacy format ? (not recommended, but smaller files if input < 8 MB)
    bool useLegacy = true;

    std::vector<uint8_t> compressedData(LZ4_compressBound(inStream.getSize()));

    int compressedSize = LZ4_compress_default(
        (const char *)inStream.getData(), (char*)compressedData.data(),
        inStream.getSize(), compressedData.size()
    );

    if (compressedSize <= 0) {
        // Compression failed
        compressedData.clear();
        compressedSize = 0;
    }
    else {
        compressedData.resize(compressedSize);
        outStream.write(compressedData.data(), compressedSize);

        int result = lz4_decompress(compressedData.data(), compressedSize, inStream.getSize());
        if (result != 0)
        {// compressed, decompressed are not the same.
            compressedSize = 0;
        }
    }

    return compressedSize;
}

const unsigned char* lz4_get_output(int* out_length)
{
    *out_length = outStream.getSize();
    return outStream.getBuf();
}




// Helper function to read an integer from a byte buffer
uint32_t readInt(const uint8_t* buffer) {
    return ((uint32_t)buffer[0] << 24) |
        ((uint32_t)buffer[1] << 16) |
        ((uint32_t)buffer[2] << 8) |
        (uint32_t)buffer[3];
}

// Simple LZ4 decompression function
int lz4Decompress(const uint8_t* compressedData, size_t compressedSize,
    uint8_t* decompressedData, size_t maxDecompressedSize) {
    const uint8_t* src = compressedData;
    uint8_t* dest = decompressedData;

    while (src < compressedData + compressedSize) {
        uint8_t token = *src++;

        uint8_t literalLength = (token >> 4) & 0x0F;
        uint8_t matchLength = token & 0x0F;

        if (literalLength == 0x0F) {
            while (*src == 0xFF) {
                literalLength += 0xFF;
                src++;
            }
            literalLength += *src++;
        }

        for (int i = 0; i < literalLength; i++) {
            *dest++ = *src++;
        }

        if (src >= compressedData + compressedSize) {
            break;
        }

        uint32_t offset = readInt(src);
        src += 4;

        if (matchLength == 0x0F) {
            while (*src == 0xFF) {
                matchLength += 0xFF;
                src++;
            }
            matchLength += *src++;
        }
        matchLength += 4;

        const uint8_t* copySrc = dest - offset;
        for (int i = 0; i < matchLength; i++) {
            *dest++ = *copySrc++;
        }
    }

    return dest - decompressedData;
}
