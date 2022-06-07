#include "light.h"

namespace Light
{

    LightEvent events[MAX_LIGHT_EVENTS];
    int eventsSetCount = 0;

    void setup(ClimateConfig config)
    {
        int eventIndex = 0;
        // load light events
        for (int i = 0; i < MAX_LIGHT_EVENTS; i++)
        {
            if (config.lightEvents[i].isSet())
            {
                events[eventIndex] = config.lightEvents[i];
                eventIndex++;
            }
        }
        eventsSetCount = eventIndex;
    }
    EventData control(int hour, int minute)
    {
        EventData data = EventData();
        std::string now = Utils::hourMinuteToString(hour, minute);
        // iterate over events
        for (int i = 0; i < eventsSetCount; i++)
        {
            LightEvent event = events[i];

            if (RealTime::checkScheduleTimeWindow(now, event.since, event.until))
            {
                pinMode(event.relay, OUTPUT);
                if (event.intencity > 0)
                {
                    digitalWrite(event.relay, HIGH);
                }
                else
                {
                    digitalWrite(event.relay, LOW);
                }
                data.active[eventsSetCount] = event;
            }
        }

        return data;
    }

}