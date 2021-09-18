#pragma once

#include "cvglCues.hpp"

MapOSC cue_nothing(cueArgs args);

MapOSC cue_black_silent(cueArgs args);
MapOSC cue_cam1_silent(cueArgs args);

MapOSC cue_cam1_silent(cueArgs args);
MapOSC cue_cam2_silent(cueArgs args);

MapOSC cue_black_click(cueArgs args);

MapOSC cue_cam1_click(cueArgs args);


MapOSC cue_wire_slug(cueArgs args);


MapOSC cue_fog_hands_field(cueArgs args);
MapOSC cue_fog_hands(cueArgs args);
MapOSC cue_origami_bioplastic_mirror(cueArgs args);

MapOSC cue_string_creature(cueArgs args);


MapOSC cue_heart_fadein(cueArgs args);
MapOSC cue_heart_continued(cueArgs args);

MapOSC cue_lungs(cueArgs args);



MapOSC cue_grass(cueArgs args);

class AnimismCues : public cvglCues
{

public:

    AnimismCues()
    {
        m_cue = "-1";

        setScene1();

        //  Scene 2 is without camera or electronics

        setScene3();
        setScene4();


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
               "wire slug on leaf, light from behind, raised up to be more depth angle",
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
               "cut to cam1, origami all white",
               &cue_wire_slug ); // maybe no change here? cresc in ensemble

        setCue("1I",
               "origami, overexposed & out of focus, gradually focus in, then pan down to goldleaf, then pan to thread on white paper",
               "cut to cam2, leaf surrouded by gel (prepare cam2 during brass/origami section!)",
               &cue_cam1_silent ); // no electronics... just change of camera
// add field recording here?

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
               "scene 3 (no electronics or camear in scene 2)",
               &cue_black_silent ); // silent electronics
    }


    void setScene3()
    {
        setCue("3A",
               "start black, shhh",
               "fog-hands (cam2)",
               &cue_black_silent );

        setCue("3B",
               "fog-hands (snares), start black, hands on black, fog, moving very slowly and cloudlike, grandually becoming bigger and bigger",
               "fog-hands",
               &cue_fog_hands_field );

        setCue("3C",
               "fog-hands + strings + field rec? but no plants",
               "cam 1, origami",
               &cue_fog_hands ); // continuing field recording

        setCue("3D", // first cut field recording when brass enters, then fade back in ? or change type of field rec
               "brass section (+ field rec?), cam1 white -> origami -> bioplastic skin / becoming mirrored -> dome thread",
               "percussion enters",
               &cue_origami_bioplastic_mirror ); // transition to double image
// add field recording to scene 1??


        setCue("3E",
               "dome thread (with tuba and 808)",
               "transition to heartbeat",
               &cue_string_creature ); // add field rec?

        setCue("3F",
               "dome thread (with tuba and 808), transitioning to heartbeat",
               "heartbeat, scene 4",
               &cue_string_creature );
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
               &cue_heart_fadein );

        setCue("4C",
               "heart",
               "heart",
               &cue_heart_continued );

        setCue("4D",
               "heart",
               "heart",
               &cue_heart_continued );

        setCue("4E",
               "heart",
               "heart",
               &cue_heart_continued );

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
