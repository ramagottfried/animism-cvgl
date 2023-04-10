#pragma once

#include "cvglCues.hpp"
#include "AnimismCues.hpp"


class ZhdkCues : public cvglCues
{

public:


    ZhdkCues()
    {
        // screen is setup by cam1 so init cue has to be cam 1
        m_cue = "0";

        /*
        setCue("-1",
               "start black",
               "0A - first sound with ensemble",
               &cue_black_silent );
    */

        setCue("0", "plastisphere", "",
               [](cueArgs args)-> MapOSC {
            MapOSC out( args.data.getOSC() );
            return out;
        });

        setCue("3E",
               "string creature",
               "transition to heartbeat",
               &cue_string_creature );

        setHeartLungCues();
        setFuzzballCues();

    }

    void setHeartLungCues()
    {

            setCue("4A",
                   "start black, move cam 1 to heart",
                   "cut to heart video (cam 2)",
                   &cue_black_silent );

            setCue("4B",
                   "heart sound fades in (cam 2)",
                   "heart overlay cam 1 fades in",
                   &cue_heart_fadein );

            setCue("4C",
                   "heart overlay cam 1 fades in",
                   "desync -> lungs",
                   &cue_heart_fadein_overlay );
    /*
            setCue("4D",
                   "heart",
                   "heart",
                   &cue_heart_continued );

            setCue("4E",
                   "heart",
                   "heart",
                   &cue_heart_continued );
    */

            setCue("4F",
                   "desync -> lungs",
                   "landscape, scene 5, holding breath",
                   &cue_lungs );

    }

    void setFuzzballCues()
    {

        // cam 1
        setCue("5A",
               "holding breath, fuzzball fade in with field rec",
               "grass creature",
               &cue_fuzzball_fadein );

        setCue("5F",
               "antler creature",
               "burdock/corona children roll out",
               &cue_antlers ); // mirror v2

        setCue("5G",
               "burdock/corona children roll out",
               "fuzzball climbs up to water",
               &cue_burdock_roll_out ); // gran attack, mirror fades out

    }

};
