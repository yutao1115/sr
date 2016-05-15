#pragma once
#include <memory>
#include <stdint.h>
#include <stdio.h>
#include <vector>


class BitmapLoader{
public:
    using OutT = std::unique_ptr<std::vector<uint32_t> >;

#pragma pack(1)
    struct FILEHEADER{
        uint16_t bfType;
        uint32_t bfSize;
        uint32_t bfReserved1;
        uint32_t bfOffBits;

        uint32_t biSize;
        int32_t  biWidth;
        int32_t  biHeight;
        uint16_t biPlanes;
        uint16_t biBitCount;
        uint32_t biCompression;
        uint32_t biSizeImage;
        int32_t  biXPelsPerMeter;
        int32_t  biYPelsPerMeter;
        uint32_t biClrUsed;
        uint32_t biClrImportant;
    };
#pragma pack()

    static OutT load(const char* path){
        FILE* fp = fopen(path,"rb");
        if(fp){
            std::unique_ptr<FILE,int (*)(FILE*)> fpPtr(fp,fclose);
            FILEHEADER header;
            if( 1 == fread(&header,sizeof(FILEHEADER),1,fp) ){
               auto width  = header.biWidth;
               auto height = abs(header.biHeight);
               if( header.biBitCount != 32 ||
                   header.biCompression != 0 ||
                   header.biClrUsed  != 0 )
               {
                   return nullptr;
               }

               auto outBuf = std::make_unique<std::vector<uint32_t>>();
               auto uint32Size= width * height;
               outBuf->reserve(uint32Size);
               if( 1 == fread(outBuf->data(),uint32Size,1,fp) )
                  return std::move(outBuf);
            }
        }
        return nullptr;
    }
};
