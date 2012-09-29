#include "earlobes.h"
#include "Modules.h"

short state;
Sample *in;
Sample *out;
unsigned int size;

MoogFilter moog(16000);
WaveTable lfo(16000);
Sample lfo_s;
WaveTable osc(16000);
Sample osc_s;
WaveTable osc2(16000);
Sample osc2_s;
Sample ring_s;

void init(unsigned int s)
{
    size=s;
    in = new Sample(size);
    out = new Sample(size);

    moog.SetCutoff(0.2);
    moog.SetResonance(0.4);
    WaveTable::WriteWaves();
    lfo.SetOctave(0);
    lfo.Trigger(0,0.1,0.1,1);
    osc.SetOctave(3);
    osc2.SetOctave(5);

    lfo_s.Allocate(size);
    osc_s.Allocate(size);
    osc2_s.Allocate(size);
    ring_s.Allocate(size);
}

float mix=0;

void set_param(int which,int amount)
{
    if (which==0) moog.SetCutoff(amount/100.0f);
    else if (which==1) moog.SetResonance(amount/100.0f*0.5);
    else 
    {
        mix=(amount-10)/100.0f;
        if (mix<0) mix=0;
    }
}

void process(short *data)
{
    for (unsigned int i=0; i<size; ++i)
    {
        (*in)[i] = (float)(data[i] / 32767.0f);
    }

//    moog.Process(size,*in,NULL,out,NULL,NULL);
    lfo.Process(size,lfo_s);
    for (unsigned int i=0; i<size; ++i) lfo_s[i]=(lfo_s[i]*0.5+0.5);

    osc.Process(size,osc_s);    
    for (unsigned int i=0; i<size; ++i) osc_s[i]=osc_s[i]*4040*lfo_s[i]+440;
    
    osc2.ProcessFM(size,osc2_s,osc_s);
 
    for (unsigned int i=0; i<size; ++i) ring_s[i]=osc2_s[i]*(*in)[i];

    if (mix>0) (*in).MulMix(ring_s,mix);

    moog.Process(size,*in,NULL,out,NULL,NULL);


    for (unsigned int i=0; i<size; ++i)
    {
        data[i] = (short)((*out)[i] * 32767.0f);
    }
}
