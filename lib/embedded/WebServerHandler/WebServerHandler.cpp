#include "WebServerHandler.h"

WebServerHandler::WebServerHandler()
{

}

WebServerHandler::~WebServerHandler()
{

}

int WebServerHandler::processLineDataRequest(AsyncWebServerRequest *request, String &buffer, const LineData lineData[], size_t arrSize)
{
    StaticJsonDocument<512> doc;
    int line = 0;
    int voltage = 0;
    
    if(!request->hasParam("line"))
    {
        return -1;
    }
    String input = request->getParam("line")->value();
    Serial.println(input);
    String value[12];
    Vector<String> valueVec;
    valueVec.setStorage(value);
    parser(input, ',', valueVec);
    JsonArray data = doc.createNestedArray("line-data");
    
    for (auto &temp : valueVec)
    {
        if(isNumber(temp))
        {
            line = temp.toInt();
            if (line > 64)
            {
                return -1;
            }
            JsonObject data_0 = data.createNestedObject();
            data_0["line"] = line;
            data_0["voltage"] = lineData[line].voltage;
            data_0["current"] = lineData[line].current;
            data_0["power"] = lineData[line].power;
        }
        else
        {
            return -1;
        }
    }
    
    serializeJson(doc, buffer);

    return 1;
}

int WebServerHandler::processRelayRequest(const String &input, String &buffer, Vector<Command> &commandList)
{
    StaticJsonDocument<1024> doc;

    DeserializationError error = deserializeJson(doc, input);

    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return -1;
    }

    if(!doc.containsKey("number"))
    {
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

    Command command;
    JsonArray line = doc["line"];
    JsonArray value = doc["value"];
    int number = doc["number"]; // 30

    if (number <= 0)
    {
        return -1;
    }

    command.type = RELAY;
    command.relayData.number = number;

    int i = 0;
    for(JsonVariant v : line) 
    {
        command.relayData.lineList[i] = v.as<int>();
        i++;
    }
    i = 0;
    for(JsonVariant v : value) 
    {
        command.relayData.valueList[i] = v.as<int>();
        i++;
    }
    i = 0;

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

void WebServerHandler::parser(const String &input, char delimiter, Vector<String> &valueVec)
{
    int index = 0;
    int lastIndex = 0;
    int i = 0;
    bool isContinue = true;
    while(isContinue)
    {
        if (i >= valueVec.max_size())
        {
            break;
        }
        lastIndex = input.indexOf(delimiter, index);
        String value = input.substring(index, lastIndex);
        valueVec.push_back(value);
        i++;
        if (lastIndex <= 0)
        {
            isContinue = false;
        }
        index = lastIndex+1;
    }
}