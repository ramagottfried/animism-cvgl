#pragma once

#include "cvglCues.hpp"

/*

    clean up scene 3 electronics
    track field rec usage, is it too much?
    check field rec levels

    eventually check scene 1 wire gran, is it too similar to antler creature?

    fuzz for ensemble, how will this work exactly?

    check camera changes

    lighting check

    does fuzzball need flashlight on it?

    are we using the gold foil?
    i think so... panning down from paper

    add noise to distortion cue
        is there noise with the trumpet?



*/


/*
 * korg notes:
 * hz1 LP -1 is off, -0.4 nice, stay < 0
 * hz2 HP 1 is off, 0.5 is nice, stay > 0
 * maths offset effects both
 *
 * sarah is the opposite
 * vcf1_hz is hp
 * vcf2_hz is lp
 *
 */



MapOSC cue_nothing(cueArgs args);

MapOSC cue_black_silent(cueArgs args);
MapOSC cue_cam1_silent(cueArgs args);

MapOSC cue_cam1_silent(cueArgs args);
MapOSC cue_cam2_silent(cueArgs args);

MapOSC cue_black_click(cueArgs args);

MapOSC cue_cam1_click(cueArgs args);


MapOSC cue_wire_slug(cueArgs args);
MapOSC cue_wire_slug2(cueArgs args);

MapOSC cue_wire_leaf_eating(cueArgs args);

MapOSC cue_leaf_shell_nest(cueArgs args);
MapOSC cue_leaf_shell_nest_m(cueArgs args);

MapOSC cue_sync_wires(cueArgs args);
MapOSC cue_sync_wires_m(cueArgs args);

// similar to sync_wires, but less attack with movement
// more envirnmental
// maybe cut korg from this, and expand background layers

MapOSC cue_leaf_in_slug(cueArgs args);
MapOSC cue_leaf_in_slug_end(cueArgs args);

MapOSC cue_thunderclap_field(cueArgs args);

MapOSC cue_black_fadeout_field(cueArgs args);

MapOSC cue_fog_hands_field(cueArgs args);
MapOSC cue_fog_hands(cueArgs args);
MapOSC cue_origami_bioplastic_mirror(cueArgs args);

MapOSC cue_string_creature(cueArgs args);


MapOSC cue_heart_fadein(cueArgs args);
MapOSC cue_heart_fadein_overlay(cueArgs args);

MapOSC cue_heart_continued(cueArgs args);

MapOSC cue_lungs(cueArgs args);

MapOSC cue_fuzzball_fadein( cueArgs args );
MapOSC cue_fuzzball( cueArgs args );
MapOSC cue_fuzzball_fadeout( cueArgs args );

MapOSC cue_grass(cueArgs args);
MapOSC cue_grass_fadeout( cueArgs args );

MapOSC cue_moon_hands(cueArgs args);
MapOSC cue_moon_hands_fadeout(cueArgs args);

MapOSC cue_antlers( cueArgs args );
MapOSC cue_burdock_roll_out(cueArgs args);

MapOSC cue_pond_edge( cueArgs args );
MapOSC cue_underwater( cueArgs args );
MapOSC cue_underwater_deep( cueArgs args );
MapOSC cue_floating_fisheye_blinks(cueArgs args);


MapOSC cue_forest( cueArgs args );
MapOSC cue_branch_rips_and_falls(cueArgs args);
MapOSC cue_forest_frogs( cueArgs args );
MapOSC cue_forest_loops( cueArgs args );
MapOSC cue_forest_loops_triZ( cueArgs args );


MapOSC cue_slow_sunburst_mirror( cueArgs args ); // fade in mirror
MapOSC cue_slow_sunburst_fadeout_mirror( cueArgs args );

MapOSC cue_slow_sunburst( cueArgs args ); // fade out tris
MapOSC cue_slow_sunburst_mirror_flow( cueArgs args ); // fade flow
MapOSC cue_slow_sunburst_mirror_flow2( cueArgs args ); // fade out mirror and increase flow


MapOSC cue_slow_sunburst_noise_start( cueArgs args );
MapOSC cue_slow_sunburst_distortion( cueArgs args );
MapOSC cue_vignette_circle_drop( cueArgs args );

class AnimismCues : public cvglCues
{

public:

    AnimismCues()
    {
        // screen is setup by cam1 so init cue has to be cam 1
        m_cue = "0B";

        setCue("-1",
               "start black",
               "0A - first sound with ensemble",
               &cue_black_silent );

        setScene1();
        setScene2();
        setScene3();
        setScene4();
        setScene5();

    }

    // add field recording to scene 1??

    void setScene1()
    {

        setCue("0A",
               "start black click",
               "white screen on cam 1",
               &cue_black_click ); // what is the click? ernst, but also add a click in the elctronics? could be fun

        setCue("0B",
               "white + click (cam 1)",
               "wire slug on leaf (cam 2)",
               &cue_cam1_click ); // what is the click? ernst

        setCue("1A",
               "wire slug on leaf",
               "leaf with shells overlay off",
               &cue_wire_slug );

        setCue("1B",
               "leaf with shells overlay off",
               "wire slug",
               &cue_cam2_silent );

        setCue("1C",
               "wire slug on leaf",
               "white - string - origami - bioplastic",
               &cue_wire_slug );
        /*
        setCue("1A",
               "wire slug on leaf",
               "leaf comes alive",
               &cue_wire_slug );

        setCue("1B",
               "leaf-creature moves around slug (electronics out)",
               "transition to shells",
               &cue_cam2_silent );

        setCue("1C",
               "transition to shells *arrive before ensemble finishes*",
               "leaf with shells",
               &cue_wire_slug2 );

        setCue("1D",
               "flowing leaf with shells, start pan back during long pause before next cue",
               "pan back to wires, delayed until long chord",
               &cue_cam2_silent  );

        setCue("1E",
               "wire sounds, start in long chord",
               "sync moment with ensemble",
               &cue_wire_slug );

        setCue("1F",
               "sync moment with ensemble",
               "wire slug on leaf",
               &cue_sync_wires_m ); // closer but not quite right yet

        setCue("1G",
               "wire slug on leaf",
               "leaves transform, pan down to goldleaf",
               &cue_wire_slug ); // back to 1B

        // climax part
        setCue("1H",
               "leaves transform, pan down to goldleaf",
               "cut to cam1, origami all white",
               &cue_wire_slug );
*/
        setCue("1I",
               "origami, overexposed & out of focus, gradually focus in, then pan down to goldleaf, then pan to thread on white paper",
               "cut to cam2, leaf surrouded by gel (prepare cam2 during brass/origami section!)",
               &cue_cam1_silent ); // no electronics... just change of camera
// add field recording here?

        setCue("1J",
               "leaf stuck in bioplastic venus flytrap",
               "curling wave",
               &cue_leaf_in_slug ); // * new scene, based on sync wires
/*
        setCue("1K",
               "curling wave, then bioplastic gel eats leaf",
               "singing D",
               &cue_leaf_in_slug_end ); // * change or same as 1J
*/
        setCue("1L",
               "singing D",
               "next cue 2I, last sequence of scene 2 (thunderclap)",
               &cue_black_silent ); // silent electronics
    }

    // move plant scene 1 platform off the puppet stage so that the hands are ready for scene 3
    void setScene2()
    {
        setCue("2I",
               "thunderclap",
               "black (scene 3)",
               &cue_thunderclap_field );
    }

    void setScene3()
    {
        setCue("3A",
               "start black, shhh",
               "fog-hands (cam 2)",
               &cue_black_silent ); //cue_black_fadeout_field

        // cam 2
        setCue("3B",
               "fog-hands (snares), moving very slowly and cloudlike, grandually becoming bigger and bigger",
               "fog-hands",
               &cue_fog_hands  ); // arne fade in electronics maybe

        // strings go into harmonics + oboe pedal gliss

        // can probably just cut this and all repeatin cues from the electronics, and then tell chatschatur which ones to cue us on
        /*
        setCue("3C",
               "fog-hands continued (strings & oboe)",
               "cam 1, origami",
               &cue_fog_hands );
        */

        // cuts to cam 1, brass solo
        // add fuzz to brass here?
        // electronic lead in to perc would be good
        // or none?

        setCue("3D",
               "brass section (+ field rec?), cam1 white -> origami -> bioplastic skin / becoming mirrored -> string creature",
               "percussion enters",
               &cue_origami_bioplastic_mirror ); // transition to double image        

        setCue("3E",
               "string creature (with tuba and 808)",
               "transition to heartbeat",
               &cue_string_creature );
/*
        setCue("3F",
               "dome thread (with tuba and 808), transitioning to heartbeat",
               "heartbeat, scene 4",
               &cue_string_creature ); // no change in electronics? then cut this cue
*/

    }

    void setScene4()
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
/*
        setCue("4G",
               "lungs",
               "landscape, scene 5, holding breath",
               &cue_lungs );
*/

    }

// check field recording sequence

    void setScene5()
    {
        // cam 1
        setCue("5A",
               "holding breath, fuzzball fade in with field rec",
               "fuzzball",
               &cue_fuzzball_fadein );
/*
        setCue("5B",
               "fuzzball",
               "fade before grass",
               &cue_fuzzball );

        setCue("5B",
               "fuzzball fade before grass",
               "grass creature",
               &cue_fuzzball_fadeout ); // maybe combine the fade out with the fade in of the grass into one cue
*/
        setCue("5C",
               "grass creature",
               "fuzzball continues on",
               &cue_grass ); // fade out field recording here?
/*

        setCue("5D",
               "fuzzball continues on",
               "moonrise with birds",
               &cue_grass_fadeout ); // fade field recording back in ?
        setCue("5E",
               "moonrise with birds",
               "moonrise fadeout, fuzzball continues",
               &cue_moon_hands ); // contour filter?

        setCue("5E_b",
               "moonrise fadeout, fuzzball continues",
               "antler creature",
               &cue_moon_hands_fadeout );
*/
        setCue("5F",
               "antler creature",
               "burdock/corona children roll out",
               &cue_antlers ); // mirror v2

        setCue("5G",
               "burdock/corona children roll out",
               "fuzzball climbs up to water",
               &cue_burdock_roll_out ); // gran attack, mirror fades out

        setCue("5H",
               "fuzzball climbs up to water, then camera continues to mirror on water",
               "underwater",
               &cue_pond_edge ); // no change?
/*
        setCue("5I",
               "at water's edge",
               "blur and dive underwater",
               &cue_pond_edge ); // no change?
*/
        // with drones? maybe quiet noisy texture drone...
                // respond to light on water?? not sure if this is possible with the camera angle

        setCue("5J",
               "underwater, paste white dots and arrow",
               "in the deep black",
               &cue_underwater );

        // cut to cam2 or prerec for floating bright eye
        // shift pitches a little here
        setCue("5K",
               "in the deep black",
               "floating fish eye blinks",
               &cue_underwater_deep );
        // sink sinewaves to low drones, hold for a while, and gradually fade out (over 30"?)

        setCue("5L",
               "floating fish eye blinks",
               "descend into the forest",
               &cue_floating_fisheye_blinks );
        // cut low sine drones


        // slow to daytime, slow fade of williston field rec, 30s
        setCue("5M",
               "very slowly descend into plant, forcus on first leaf (top) and then very slowly continue downward",
               "branch rips off and falls",
               &cue_forest );


        setCue("5N",
               "branch rips off and falls, (still near top of plant) actions in ensemble, -? grain bugs",
               "frogs and other pond and forest creatures",
               &cue_branch_rips_and_falls );  // move fast on downbeat
// can merge Q and N cues for electronics
        setCue("5O",
               "frogs and other pond and forest creatures",
               "natural sounds begin to loop",
               &cue_forest_frogs );             // or traffic bugs here?


        // slowly fade in different types of mirror images?+

        // might work better than pure mirror fragments
        // could also pre-record this part and make hardcore forwards/backwards loops etc.
        setCue("5P",
               "natural sounds begin to loop, fade in mirror, mirror stays until ending, first use for bugs, then in plants, slow expansion",
               "fade out mirror",
               &cue_forest_loops ); // fast insect traffic becomes cars
/*
        setCue("5P_b",
               "loops, xfade to mirror",
               "slow spectral sunburst",
               &cue_slow_sunburst_mirror );
*/
        setCue("5Q",
               "fade out mirror",
               "luma",
               &cue_slow_sunburst_fadeout_mirror  ); //cue_slow_sunburst
/*
        setCue("5Q_b",
               "fade in mirror, squeeze plant to make animals with mirror image",
               "mirror flow",
               &cue_slow_sunburst_mirror );
*/
        setCue("5Q_c",
               " flow lumakey",
               "more flow",
               &cue_slow_sunburst_mirror_flow );


        setCue("5Q_d",
               "more flow",
               "becoming white noise",
               &cue_slow_sunburst_mirror_flow2 );


        setCue("5R",
               "becoming white noise",
               "white noise",
               &cue_slow_sunburst_noise_start ); // fades in iceland again, speeding up to noise

        setCue("5S",
               "white noise",
               "screen vignette with trumpet solo",
               &cue_slow_sunburst_distortion );

        setCue("5T",
               "screen vignette with trumpet solo, circle falls to ground for screen drop, prepare for puppet performance (electronics end here)",
               "screen drop",
               &cue_vignette_circle_drop );

    }
};
