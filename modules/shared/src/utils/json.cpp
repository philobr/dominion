
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <string>

std::string documentToString(const rapidjson::Document &doc)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();
}
