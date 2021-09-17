#pragma once

#include "cvglCues.hpp"

class AnimismCues : public cvglCues
{

public:

    MapOSC cue_black_silent(const AnalysisData& data, MapOSC& b);
    MapOSC cue_black_click(const AnalysisData& data, MapOSC& b);
    MapOSC cue_white_click(const AnalysisData& data, MapOSC& b);

    MapOSC cue_wire_slug(const AnalysisData& data, MapOSC& b);

    MapOSC cue_grass(const AnalysisData& data, MapOSC& b);

    AnimismCues()
    {
        m_cue = "1A";

       // setCue("1A", "hey", "ho", [&](const AnalysisData& data, MapOSC& b) -> MapOSC { return cue_wire_slug(data, b); } );

        using namespace std::placeholders;
        m_cueFunctions.emplace("-1", bind(&AnimismCues::cue_black_silent, this, _1, _2) );
        m_cueFunctions.emplace("0A", bind(&AnimismCues::cue_black_click, this, _1, _2) );
        m_cueFunctions.emplace("0B", bind(&AnimismCues::cue_white_click, this, _1, _2) );

        m_cueFunctions.emplace("1A", bind(&AnimismCues::cue_wire_slug, this, _1, _2) );

        m_cueFunctions.emplace("10", bind(&AnimismCues::cue_grass, this, _1, _2) );
    }


};
