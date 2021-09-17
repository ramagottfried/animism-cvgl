#pragma once

#include "cvglCues.hpp"

MapOSC cue_black_silent(cueArgs args);
MapOSC cue_cam1_silent(cueArgs args);

MapOSC cue_cam1_silent(cueArgs args);
MapOSC cue_cam2_silent(cueArgs args);

MapOSC cue_black_click(cueArgs args);

MapOSC cue_cam1_click(cueArgs args);

MapOSC cue_wire_slug(cueArgs args);

MapOSC cue_fog_hands(cueArgs args);
MapOSC cue_fog_hands_field(cueArgs args);

MapOSC cue_grass(cueArgs args);

MapOSC cue_heart1(cueArgs args);
MapOSC cue_lungs(cueArgs args);


MapOSC cue_string_creature(cueArgs args);


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
               &cue_cam1_click ); // what is the click?

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
               &cue_cam1_silent ); // no electronics... just change of camera

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
               "fog-hands (snares)",
               "fog-hands",
               &cue_cam2_silent ); // cam 2, live processing of ensemble?

        setCue("3C",
               "fog-hands + strings + field rec? but no plants",
               "cam 1, origami",
               &cue_fog_hands_field ); // cam 2, live processing of ensemble?

        setCue("3D",
               "cam 1, origami, brass section, field rec cut",
               "percussion enters",
               &cue_cam1_silent ); // transition to double image

        setCue("3E",
               "brass + perc with string creature",
               "transition to heartbeat",
               &cue_string_creature ); // add field rec?

        setCue("3F",
               "transition to heartbeat",
               "heartbeat, scene 4",
               &cue_cam1_silent );
    }

    void setScene4()
    {
        setCue("4A",
               "start black, quickly move cam 1 to heart",
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
               &cue_lungs );

        setCue("4G",
               "lungs",
               "landscape, scene 5",
               &cue_lungs );


    }
};
