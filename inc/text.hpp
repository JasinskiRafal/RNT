#pragma once
#include <memory>

#include <tesseract/baseapi.h>

namespace rnt
{
    namespace txt
    {
        class Reader {
            std::unique_ptr<tesseract::TessBaseAPI> api;

            public:
            Reader();
            Reader(const Reader&) = delete;
            void operator=(const Reader&) = delete;
            Reader(Reader&&) = delete;
            void operator=(Reader&&) = delete;
            ~Reader();

            std::string get_text(std::string file_name);
        };
    } // namespace txt
    
} // namespace rnt
