#pragma once

#include <chrono>

#include <Eigen/Dense>
#include "cvglAnalysisData.hpp"
#include "MapOSC.hpp"
#include "cvglMixer.hpp"
#include "cvglRandom.hpp"
#include "cvglLookupFigure.hpp"

using namespace Eigen;

class cvglCues
{
public:
    
    MapOSC cue0(const AnalysisData& data, cvglMixer& mixer, MapOSC& b);
    MapOSC cue1(const AnalysisData& data, cvglMixer& mixer, MapOSC& b);

    
    cvglCues()
    {
        using namespace std::placeholders;
        
        m_cueFunctions.emplace_back( bind(&cvglCues::cue0, this, _1, _2, _3 ) );
        m_cueFunctions.emplace_back( bind(&cvglCues::cue1, this, _1, _2, _3 ) );

    
    }

    /**
     processes data and input settings, outputs OSC bundle to send to audio process
     */
    MapOSC procDataAndMixer(const AnalysisData& data, cvglMixer& mixer, MapOSC& b);
    MapOSC procDataAndMixer(const AnalysisData& data, cvglMixer& mixer);

    
private:

    typedef std::chrono::system_clock sys_clock_t;
    typedef std::chrono::time_point<sys_clock_t> timepoint_t;
    typedef std::chrono::duration<double> duration_t;
    typedef std::function< MapOSC(const AnalysisData&, cvglMixer&, MapOSC&) > cueFunction_t;
    
    MapOSC m_state_cache;
    
    long m_cue = 1;
    bool m_newCue = false;

    timepoint_t m_section_start = sys_clock_t::now();
    duration_t m_elapsed_section;
    
    vector<cueFunction_t> m_cueFunctions;
    
    cvglRandom m_rand_generator;

    MapOSC m_input;
    
    AnalysisData m_prev_data;
};
