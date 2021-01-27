#include "cvglCues.hpp"

MapOSC cvglCues::cue0( const AnalysisData& data, cvglMixer& mixer, MapOSC& b )
{
    //b.print();
    MapOSC out;
    out.addMessage("/descr", "prelude: setting up, putting on gloves");
    
    const double elapsed_section = m_elapsed_section.count();
    
    
    if( m_newCue )
    {
        mixer.set_pregain("/perc",      -6);
        mixer.set_pregain("/gran",      -70);
        mixer.set_pregain("/res",       -70);
        mixer.set_pregain("/filter",    -70);

        mixer.addPregainMidi(out);
        
        out.addMessage("/trigger/perc/drum/trigger",  1);
        out.addMessage("/trigger/perc/drum/motor",  0);
        out.addMessage("/trigger/perc/drum/rate",  0.74);
        out.addMessage("/trigger/perc/drum/buffer/index",  9);
        out.addMessage("/trigger/perc/drum/duration",  -1);
        out.addMessage("/trigger/perc/drum/amp",  1);
        out.addMessage("/trigger/perc/drum/pan",  0);
        out.addMessage("/trigger/perc/drum/xshape",  0);
        out.addMessage("/trigger/perc/drum/xshape",  0);
        out.addMessage("/trigger/perc/drum/yshape",  -5);
        out.addMessage("/trigger/perc/clock/*/ms", 0);

    }
    
    mixer.applyAddGains(out);
    
    // b messages get sent to CV and GL params
    b.addMessage("/video/black",  0);
 

    return out;
}
