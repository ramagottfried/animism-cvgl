
#include "cvglCues.hpp"

// called from analysis thread
MapOSC cvglCues::procDataAndMixer(const AnalysisData& data)
{
    
    m_elapsed_section = sys_clock_t::now() - m_section_start;
    
    MapOSC out = m_cueFunctions[m_cue]( data, m_input );
    
    isNewCue = false;
    
    return out;
}

// called from UDP input thread
MapOSC cvglCues::procDataAndMixer(const AnalysisData& data, MapOSC& b)
{
    if( b.addressExists("/cue") )
    {
        int cue = b.getMessage("/cue").getInt();
        if( cue != m_cue )
        {

            isNewCue = true;
            m_section_start = sys_clock_t::now();
            m_cue = (cue >= 0 && cue < (int)m_cueFunctions.size()) ? cue : 0;
            
            cout << "setting new cue " << cue << " was " << m_cue << endl;

            m_state_cache.addMessage("/played", 0);
            
        }
        
    }
    
    m_elapsed_section = sys_clock_t::now() - m_section_start;
        
    MapOSC out = m_cueFunctions[m_cue]( data, b );
    
    bool ret = isNewCue;

    // this shoud copy bundle
    m_input = b; //OdotBundle((const OdotBundle)b);
    isNewCue = false;
    
    return ret ? out : MapOSC();
}
