#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <fstream>
#include <string_view>

using namespace std;

namespace img_lib {

PACKED_STRUCT_BEGIN BitmapFileHeader {
    uint8_t type[2]{'B', 'M'};
    uint32_t size{0};
    uint32_t reserved{0};
    uint32_t offset_data{54};
    // поля заголовка Bitmap File Header
}
PACKED_STRUCT_END

PACKED_STRUCT_BEGIN BitmapInfoHeader {
    uint32_t size{40};
    int32_t width{0};
    int32_t height{0};
    uint16_t planes{1};
    uint16_t bit_count{24};
    uint32_t compression{0};
    uint32_t size_image{0};
    int32_t x_pixels_per_meter{11811};
    int32_t y_pixels_per_meter{11811};
    uint32_t colors_used{0};
    uint32_t colors_important{0x1000000};
    // поля заголовка Bitmap Info Header
}
PACKED_STRUCT_END

// функция вычисления отступа по ширине
static int GetBMPStride(int w) {
    return 4 * ((w * 3 + 3) / 4);
}

// напишите эту функцию
bool SaveBMP(const Path& file, const Image& image) {
    ofstream out(file, ios::binary);
    
    int w = image.GetWidth();
    int h = image.GetHeight();
    int stride = GetBMPStride(w);
    
    BitmapFileHeader file_h;
    file_h.size = 54 + (stride * h);
    BitmapInfoHeader info_h;
    info_h.width = w;
    info_h.height = h;
    info_h.size_image = stride * h;
    
    
    out.write(reinterpret_cast<char*>(&file_h), sizeof(file_h));
    out.write(reinterpret_cast<char*>(&info_h), sizeof(info_h));

    
    std::vector<char> buff(stride);

    for (int y = h - 1; y >= 0; --y) {
        const Color* line = image.GetLine(y);
        for (int x = 0; x < w; ++x) {
            buff[x * 3] = static_cast<char>(line[x].b);
            buff[x * 3 + 1] = static_cast<char>(line[x].g);
            buff[x * 3 + 2] = static_cast<char>(line[x].r);
        }
        out.write(buff.data(), stride);
    }
    
    return out.good();
};

// напишите эту функцию
Image LoadBMP(const Path& file) {
    ifstream ifs(file, ios::binary);
    
    BitmapFileHeader file_h;
    BitmapInfoHeader info_h;
    
    ifs.read((char*)&file_h, sizeof(file_h));
    ifs.read((char*)&info_h, sizeof(info_h));

    if(file_h.type[0] != 'B' && file_h.type[1] != 'M') {
        return {};
    }
    
    ifs.seekg(file_h.offset_data, ifs.beg);
    
    int w = info_h.width;
    int h = info_h.height;
    
    int stride = GetBMPStride(w);
    
    Image result(w, h, Color::Black());
    std::vector<char> buff(stride);
    

    for (int y = h - 1; y >= 0; --y) {
        ifs.read(buff.data(), stride);
        Color* line = result.GetLine(y);

        for (int x = 0; x < w; ++x) {
            line[x].b = static_cast<byte>(buff[x * 3]);
            line[x].g = static_cast<byte>(buff[x * 3 + 1]);
            line[x].r = static_cast<byte>(buff[x * 3 + 2]);
        }
    }

    
    return result;
};

}  // namespace img_lib
