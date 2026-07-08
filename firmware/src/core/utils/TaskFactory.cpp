#include "TaskFactory.h"
#include "GlobalResources.h"
#include "MainHelper.h"
#include "TaskManager.h"
#include "Utils.h"
#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <HTTPClient.h>
#include <LittleFS.h>

void TaskFactory::httpGetTask(const String &url, const String &filter, Task::ResponseCallback callback, Task::PreProcessCallback preProcess) {
    Log.infoln("🔵 Starting HTTP request for: %s", url.c_str());

    HTTPClient http;
    bool isHttps = url.startsWith("https://"); // Check if the URL is HTTPS
    http.useHTTP10(true);
    // Declare client outside the conditional blocks
    WiFiClient *client = nullptr;
    if (isHttps) {
        client = new WiFiClientSecure();
        static_cast<WiFiClientSecure *>(client)->setInsecure(); // Bypass SSL certificate validation
        http.begin(*client, url); // Use WiFiClientSecure for HTTPS
    } else {
        client = new WiFiClient();
        http.begin(*client, url); // Use WiFiClient for HTTP
    }

    http.setTimeout(10000); // 10-second timeout

    String response;
    int httpCode = http.GET();

    if (httpCode > 0) {
        JsonDocument doc;
        if (filter == "") { // No filter, so just get and pass
            response = http.getString();
        } else { // Filter, so let ArduinoJson do it's 'thing'
            JsonDocument filterdoc;
            DeserializationError errorf = deserializeJson(filterdoc, filter);

            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filterdoc), DeserializationOption::NestingLimit(16));

            if (!error) {
                response = doc.as<String>();
            } else {
                // Handle JSON deserialization error
                Log.errorln("Deserialization failed: %s", error.c_str());
            }
        }
    } else {
        Log.errorln("🔴 HTTP request failed, error code: %d", httpCode);
    }
    http.end();

    // Explicitly reset the objects
    http.~HTTPClient(); // Call the destructor
    new (&http) HTTPClient(); // Reinitialize using placement new

    if (isHttps) {
        static_cast<WiFiClientSecure *>(client)->~WiFiClientSecure(); // Call the destructor
        new (client) WiFiClientSecure(); // Reinitialize using placement new
    } else {
        static_cast<WiFiClient *>(client)->~WiFiClient(); // Call the destructor
        new (client) WiFiClient(); // Reinitialize using placement new
    }

    client->stop();
    delete client; // Clean up the client object

    if (preProcess) {
        preProcess(httpCode, response);
    }

    auto *responseData = new TaskManager::ResponseData{httpCode, response, callback};

    if (xQueueSend(TaskManager::responseQueue, &responseData, 0) != pdPASS) {
        Log.errorln("Failed to queue response");
        delete responseData; // Ensure cleanup if queueing fails
    }
#ifdef TASK_DEBUG
    ShowMemoryUsage::printSerial(true);
    Log.infoln("Task Stack High watermark: %u bytes\n", uxTaskGetStackHighWaterMark(NULL));
#endif
    TaskManager::activeRequests--;

#ifdef TASKMANAGER_DEBUG
    Log.infoln("Active requests now: %d", TaskManager::activeRequests);
    UBaseType_t highWater = uxTaskGetStackHighWaterMark(NULL);
    Log.infoln("Remaining task stack space: %d", highWater);
#endif
}

void TaskFactory::httpGetFileTask(const String &url, const String &fileName, Task::ResponseCallback callback) {
    Log.infoln("🔵 Starting HTTP request for: %s", url.c_str());
    {
        HTTPClient http;
        bool isHttps = url.startsWith("https://"); // Check if the URL is HTTPS

        http.setReuse(true);
        http.useHTTP10(true);
        // Declare client outside the conditional blocks
        WiFiClient *client = nullptr;
        if (isHttps) {
            client = new WiFiClientSecure();
            static_cast<WiFiClientSecure *>(client)->setInsecure(); // Bypass SSL certificate validation
            http.begin(*client, url); // Use WiFiClientSecure for HTTPS
        } else {
            client = new WiFiClient();
            http.begin(*client, url); // Use WiFiClient for HTTP
        }

        http.setTimeout(10000); // 10-second timeout

        String response = fileName;
        int httpCode = http.GET();

        if (httpCode > 0) {
            if (httpCode == HTTP_CODE_OK) {

                File file = LittleFS.open(fileName, FILE_WRITE);

                // get lenght of document (is -1 when Server sends no Content-Length header)
                int len = http.getSize();

                // create buffer for read
                uint8_t buff[128] = {0};

                // get tcp stream
                WiFiClient *stream = http.getStreamPtr();

                // read all data from server
                while (http.connected() && (len > 0 || len == -1)) {
                    // get available data size
                    size_t size = stream->available();
                    if (size) {
                        // read up to 128 byte
                        int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                        // write it to Serial
                        // Serial.write(buff, c);
                        file.write(buff, c);
                        if (len > 0) {
                            len -= c;
                        }
                    }
                    delay(1);
                }
                file.close();
            } else {
                Log.errorln("🔴 HTTP request failed, error code: %d", httpCode);
            }
        }
        http.end();

        // Explicitly reset the objects
        http.~HTTPClient(); // Call the destructor
        new (&http) HTTPClient(); // Reinitialize using placement new

        if (isHttps) {
            static_cast<WiFiClientSecure *>(client)->~WiFiClientSecure(); // Call the destructor
            new (client) WiFiClientSecure(); // Reinitialize using placement new
        } else {
            static_cast<WiFiClient *>(client)->~WiFiClient(); // Call the destructor
            new (client) WiFiClient(); // Reinitialize using placement new
        }

        client->stop();
        delete client; // Clean up the client object

        auto *responseData = new TaskManager::ResponseData{httpCode, response, callback};

        if (xQueueSend(TaskManager::responseQueue, &responseData, 0) != pdPASS) {
            Log.errorln("Failed to queue response");
            delete responseData; // Ensure cleanup if queueing fails
        }
    }
    TaskManager::activeRequests--;

#ifdef TASKMANAGER_DEBUG
    Log.infoln("Active requests now: %d", TaskManager::activeRequests);
    UBaseType_t highWater = uxTaskGetStackHighWaterMark(NULL);
    Log.infoln("Remaining task stack space: %d", highWater);
#endif
}
