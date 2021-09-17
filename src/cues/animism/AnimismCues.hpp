#pragma once

#include "cvglCues.hpp"

MapOSC cue_black_silent(cueArgs args);
MapOSC cue_black_click(cueArgs args);
MapOSC cue_white_click(cueArgs args);
MapOSC cue_white_silent(cueArgs args);
MapOSC cue_wire_slug(cueArgs args);

MapOSC cue_fog_hands(cueArgs args);

MapOSC cue_grass(cueArgs args);

MapOSC cue_heart1(cueArgs args);


class AnimismCues : public cvglCues
{

public:

    AnimismCues()
    {
        m_cue = "-1";

        setScene1();
        //  Scene 2 is without camera or electronics
        setScene3();


        setCue("5D",
               "grass tests",
               "",
               &cue_wire_slug );

    }


    void setScene1()
    {
        setCue("-1",
               "start black",
               "0A - first sound with ensemble",
               &cue_black_silent );

        setCue("0A",
               "start black click",
               "0B white screen on cam 1",
               &cue_black_click ); // what is the click?

        setCue("0B",
               "camera 1 white + click",
               "",
               &cue_white_click ); // what is the click?

        setCue("1A",
               "wire slug on leaf, high light levels, lower aperature, for more focus",
               "leaf-creature",
               &cue_wire_slug ); // *

        setCue("1B",
               "wire slug on leaf, leaf-creatures nibble on head",
               "back to wire material, then pan",
               &cue_wire_slug ); // maybe no change in electronics here?

        setCue("1C",
               "wire slug, then pan to shells",
               "leaves pushing shells",
               &cue_wire_slug );  // maybe no change in electronics here?

        setCue("1D",
               "leaves pushing shells",
               "back to wires",
               &cue_wire_slug ); // maybe no change in electronics here?

        setCue("1E",
               "wire slug on leaf",
               "sync moment with ensemble",
               &cue_wire_slug ); // * new

        setCue("1F",
               "sync moment with ensemble",
               "wire slug on leaf",
               &cue_wire_slug ); //maybe same as 1E

        setCue("1G",
               "wire slug on leaf",
               "slug rips apart",
               &cue_wire_slug ); // back to 1B

        setCue("1H",
               "slug rips apart",
               "brass section",
               &cue_wire_slug ); // maybe no change here? cresc in ensemble

        setCue("1I",
               "brass section",
               "leaf surrouded by gel",
               &cue_white_silent ); // no electronics... just change of camera

        setCue("1J",
               "leaf surrouded by gel",
               "curling wave",
               &cue_wire_slug ); // * new

        setCue("1K",
               "curling wave",
               "singing D",
               &cue_wire_slug ); // * change or same as 1J

        setCue("1L",
               "singing D",
               "scene 2",
               &cue_black_silent ); // silent electronics
    }


    void setScene3()
    {
        setCue("3A",
               "start black, shhh",
               "fog-hands",
               &cue_black_silent );

        setCue("3B",
               "fog-hands",
               "fog-hands + strings",
               &cue_fog_hands ); // cam 2, live processing of ensemble?

        setCue("3C",
               "fog-hands + strings",
               "cam 1, origami",
               &cue_fog_hands ); // cam 2, live processing of ensemble?

        setCue("3D",
               "cam 1, origami, brass section",
               "percussion enters",
               &cue_white_silent );

        setCue("3E",
               "brass + perc",
               "transition to heartbeat",
               &cue_white_silent );

        setCue("3F",
               "transition to heartbeat",
               "heartbeat, scene 4",
               &cue_white_silent );
    }

    void setScene4()
    {
        setCue("4A",
               "start black",
               "cut to heart video",
               &cue_black_silent );

        setCue("4B",
               "heart",
               "heart",
               &cue_heart1 );

        setCue("4C",
               "heart",
               "heart",
               &cue_heart1 );

        setCue("4D",
               "heart",
               "heart",
               &cue_heart1 );

        setCue("4E",
               "heart",
               "heart",
               &cue_heart1 );

        setCue("4F",
               "desync -> lungs",
               "lungs",
               &cue_heart1 );

        setCue("4G",
               "lungs",
               "landscape, scene 5",
               &cue_heart1 );


    }
};
