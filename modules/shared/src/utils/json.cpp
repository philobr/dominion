
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <string>

std::string document_to_string(const rapidjson::Document &doc)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();
}
