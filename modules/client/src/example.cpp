#include <client/example.h>

namespace client {
    bool test()
    {
        return true;
    }
} // namespace client_unit_tests

#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace rapidjson {
    void test_rapidjson()
    {
        // 1. Create a JSON object with a "Hello" message
        rapidjson::Document doc;
        doc.SetObject();  // Initialize the document as an object

        rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
        doc.AddMember("greeting", "Hello, World!", allocator);

        // 2. Convert JSON object to a string
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        doc.Accept(writer);

        std::cout << "JSON Output: " << buffer.GetString() << std::endl;

        // 3. Parse the JSON string to retrieve the greeting message
        rapidjson::Document parsedDoc;
        if ( parsedDoc.Parse(buffer.GetString()).HasParseError() ) {
            std::cerr << "JSON parse error!" << std::endl;
            return;
        }

        if ( parsedDoc.HasMember("greeting") ) {
            std::cout << "Parsed greeting: " << parsedDoc["greeting"].GetString() << std::endl;
        }
        else {
            std::cerr << "Greeting not found in JSON!" << std::endl;
        }
    }
} // namespace rapidjson