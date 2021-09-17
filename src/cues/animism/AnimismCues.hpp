#pragma once

#include "cvglCues.hpp"

MapOSC cue_black_silent(cueArgs args);
MapOSC cue_black_click(cueArgs args);
MapOSC cue_white_click(cueArgs args);
MapOSC cue_wire_slug(cueArgs args);
MapOSC cue_grass(cueArgs args);

class AnimismCues : public cvglCues
{

public:

    AnimismCues()
    {
        m_cue = "1A";

        setCue("-1",
               "start black",
               "0A - first sound with ensemble",
               &cue_black_silent );

        setCue("0A",
               "start black click",
               "0B white screen on cam 1",
               &cue_black_click );

        setCue("0B",
               "camera 1 white + click",
               "",
               &cue_white_click );

        setCue("1A",
               "wire slug on leaf, high light levels, lower aperature, for more focus",
               "",
               &cue_wire_slug );

        setCue("5D",
               "grass tests",
               "",
               &cue_wire_slug );

        //m_cueFunctions.emplace("10", bind(&AnimismCues::cue_grass, this, _1, _2) );

/*
        using namespace std::placeholders;
        m_cueFunctions.emplace("-1", bind(&AnimismCues::cue_black_silent, this, _1, _2) );
        m_cueFunctions.emplace("0A", bind(&AnimismCues::cue_black_click, this, _1, _2) );
        m_cueFunctions.emplace("0B", bind(&AnimismCues::cue_white_click, this, _1, _2) );

        m_cueFunctions.emplace("1A", bind(&AnimismCues::cue_wire_slug, this, _1, _2) );

        m_cueFunctions.emplace("10", bind(&AnimismCues::cue_grass, this, _1, _2) );
  */
  }


};
