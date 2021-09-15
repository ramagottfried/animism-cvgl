#pragma once

#include <chrono>
#include <iostream>

#include <Eigen/Dense>
#include "cvglAnalysisData.hpp"
#include "MapOSC.hpp"
//#include "cvglMixer.hpp"
#include "cvglRandom.hpp"
#include "cvglLookupFigure.hpp"

#ifndef CUE_FILE_NAMES
#define CUE_FILE_NAMES "test"
#endif

class cvglCues
{
public:

 //   void set_lambda_cues();


 //   MapOSC cue0(const AnalysisData& data, MapOSC& b);
 //   MapOSC cue1(const AnalysisData& data, MapOSC& b);

    
    cvglCues()
    {
        // not doing this right now but maybe at somepoint, could be a way to dynamically load
        // all the cues without having to manually type them in the header file.


     //   cout << "****** CUE_FILE_NAMES ***** " << CUE_FILE_NAMES << endl;


     //   using namespace std::placeholders;
     //   m_cueFunctions.emplace_back( bind(&cvglCues::cue0, this, _1, _2) );
     //   m_cueFunctions.emplace_back( bind(&cvglCues::cue1, this, _1, _2) );
  //      set_lambda_cues();

    
    }

    /**
     processes data and input settings, outputs OSC bundle to send to audio process
     */
    MapOSC procDataAndMixer(const AnalysisData& data, MapOSC& b);
    MapOSC procDataAndMixer(const AnalysisData& data);

    bool isNewCue = true;

    vector<string> getCueNames()
    {
        vector<string> cuenames;
        for( auto& c : m_cueFunctions )
        {
            cuenames.emplace_back( c.first );
        }

        sort(cuenames.begin(), cuenames.end());
        return cuenames;
    }

    
protected:

    typedef std::chrono::system_clock sys_clock_t;
    typedef std::chrono::time_point<sys_clock_t> timepoint_t;
    typedef std::chrono::duration<double> duration_t;
    typedef std::function< MapOSC(const AnalysisData&, MapOSC&) > cueFunction_t;
    
    MapOSC m_state_cache;
    
    string m_cue = "0";

    timepoint_t m_section_start = sys_clock_t::now();
    duration_t m_elapsed_section;
    
    unordered_map<string, cueFunction_t> m_cueFunctions;
    
    cvglRandom m_rand_generator;

    MapOSC m_input;
    
    AnalysisData m_prev_data;
};
