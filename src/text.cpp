#include <text.hpp>
#include <leptonica/allheaders.h>

namespace rnt
{
    namespace txt
    {
        Reader::Reader() : api(new tesseract::TessBaseAPI) {
            api->Init(NULL, "eng");
        }

        Reader::~Reader() {
            api->End();
        }

        std::string Reader::get_text(std::string file_name) {
            std::string ret;
            std::unique_ptr<Pix> image(pixRead(file_name.c_str()));
            api->SetImage(image.get());
            ret = api->GetUTF8Text();
            return ret;
        }
    } // namespace txt
    
} // namespace rnt
