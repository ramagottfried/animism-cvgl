
#include "cvglCues.hpp"

// called from analysis thread
MapOSC cvglCues::procDataAndMixer(const AnalysisData& data, cvglMixer& mixer)
{
    
    m_elapsed_section = sys_clock_t::now() - m_section_start;
    
    MapOSC out = m_cueFunctions[m_cue]( data, mixer,  m_input );
    
    m_newCue = false;
    
    return out;
}

// called from UDP input thread
MapOSC cvglCues::procDataAndMixer(const AnalysisData& data, cvglMixer& mixer, MapOSC& b)
{
    if( b.addressExists("/cue") )
    {
        int cue = b.getMessage("/cue").getInt();
        if( cue != m_cue )
        {
           // cout << "setting new cue " << cue << " was " << m_cue << endl;

            m_newCue = true;
            m_section_start = sys_clock_t::now();
            m_cue = (cue >= 0 && cue < m_cueFunctions.size()) ? cue : 0;
            

            m_state_cache.addMessage("/played", 0);
            
        }
        
    }
    
    m_elapsed_section = sys_clock_t::now() - m_section_start;
        
    MapOSC out = m_cueFunctions[m_cue]( data, mixer, b );
    
    // this shoud copy bundle
    m_input = b; //OdotBundle((const OdotBundle)b);
    m_newCue = false;
    
    return out;
}
