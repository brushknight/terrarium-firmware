#ifndef TERRARIUM_I2C
#define TERRARIUM_I2C

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "data_structures.h"

namespace I2C
{

    static const char *TAG = "i2c-master";

    class IlluminationCommand
    {
    public:
        int brightness; // 0-100
        Color color;
        IlluminationCommand() {}
        IlluminationCommand(int b, Color c)
        {
            brightness = b;
            color = c;
        }
        static int jsonSize()
        {
            return 48 + Color::jsonSize();
        }
        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());
            doc["brightness"] = brightness;
            doc["color"] = color.toJSON();
            return doc;
        }
        static IlluminationCommand fromJSON(std::string json)
        {
            DynamicJsonDocument doc(jsonSize());
            deserializeJson(doc, json);
            return IlluminationCommand::fromJSONObj(doc);
        }
        static IlluminationCommand fromJSONObj(DynamicJsonDocument doc)
        {
            IlluminationCommand command;
            command.brightness = doc["brightness"];
            command.color = Color::fromJSONObj(doc["color"]);
            return command;
        }
    };

    class Message
    {
    public:
        int bytes = 0;
        int type; // command = 0 | response = 1
        IlluminationCommand illumination;
        Message() {}
        Message(int b, Color c)
        {
            type = 1;
            illumination = IlluminationCommand(b, c);
        }
        static int jsonSize()
        {
            return 48 + IlluminationCommand::jsonSize();
        }
        DynamicJsonDocument toJSON()
        {
            DynamicJsonDocument doc(jsonSize());
            doc["type"] = type;
            doc["illumination"] = illumination.toJSON();
            return doc;
        }
        static Message fromJSON(std::string json)
        {
            ESP_LOGI(TAG, "Message JSON string recived %s", json);
            DynamicJsonDocument doc(jsonSize());
            deserializeJson(doc, json);

            return Message::fromJSONObj(doc);
        }
        static Message fromJSONObj(DynamicJsonDocument doc)
        {
            Message message;
            message.type = doc["type"];
            // if (doc.containsKey("type"))
            // {
            //     event.type = doc["type"].as<std::string>();
            // }
            message.illumination = IlluminationCommand::fromJSONObj(doc["illumination"]);
            return message;
        }
    };

    esp_err_t i2c_master_init(void);
    esp_err_t i2c_slave_init(void);
    esp_err_t i2c_master_send(uint16_t i2cID, uint8_t message[], int len);
    void write(uint16_t i2cID, Message message);
    Message read();

}

#endif