#include <text.hpp>
#include <leptonica/allheaders.h>

namespace rnt
{
    namespace txt
    {
        struct Reader::ReaderAPI
        {
            ReaderAPI() : tesseractAPI{new tesseract::TessBaseAPI} {}
            std::unique_ptr<tesseract::TessBaseAPI> tesseractAPI;
        };

        Reader::Reader(std::string language) : _api{new Reader::ReaderAPI}
        {
            _api->tesseractAPI->Init(NULL, language.c_str());
        }

        Reader::~Reader()
        {
            _api->tesseractAPI->End();
        }

        std::string Reader::get_text(std::string file_name)
        {
            std::string ret;
            std::unique_ptr<Pix> image(pixRead(file_name.c_str()));
            _api->tesseractAPI->SetImage(image.get());
            ret = _api->tesseractAPI->GetUTF8Text();
            return ret;
        }
    } // namespace txt

} // namespace rnt
