#include "WebServerHandler.h"

WebServerHandler::WebServerHandler()
{

}

WebServerHandler::~WebServerHandler()
{

}

int WebServerHandler::processLineDataRequest(AsyncWebServerRequest *request, String &buffer, const LineData lineData[], size_t arrSize)
{
    StaticJsonDocument<64> doc;
    int line = 0;
    int voltage = 0;
    
    if(!request->hasParam("line"))
    {
        return -1;
    }
    String input = request->getParam("line")->value();

    if(isNumber(input))
    {
        line = input.toInt();
    }
    else
    {
        return -1;
    }

    if (line > 64)
    {
        return -1;
    }

    doc["line"] = line;
    doc["voltage"] = lineData[line].voltage;
    doc["current"] = lineData[line].current;
    doc["power"] = lineData[line].power;
    serializeJson(doc, buffer);

    return 1;
}

int WebServerHandler::processRelayRequest(const String &input, String &buffer, Vector<Command> &commandList)
{
    StaticJsonDocument<128> doc;

    DeserializationError error = deserializeJson(doc, input);

    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return -1;
    }

    if(!doc.containsKey("line"))
    {
        return -1;
    }

    if(!doc.containsKey("value"))
    {
        return -1;
    }

    int line = doc["line"]; // 1
    int value = doc["value"]; // 1
    Command command;
    command.commandType = RELAY;
    command.line = line;
    command.value = value;
    commandList.push_back(command);

    StaticJsonDocument<16> docResponse;

    docResponse["status"] = 1;

    serializeJson(docResponse, buffer);

    return 1;
}


bool WebServerHandler::isNumber(const String &input)
{
    for (char const &ch : input) {
        if (std::isdigit(ch) == 0) 
            return false;
    }
    return true;
}