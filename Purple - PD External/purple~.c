//  Purple Pure Data - Mark Matamoros Fall 2015
//  This is a guitar (bass and electric) effects plugin with tremolo and two gain stages.

#include "m_pd.h"
#include <math.h>

static t_class *purple_class;

typedef struct _purple
{
    t_object x_obj;
    t_float x_f;
    
    t_float input;
    t_float dist1;
    t_float dist2;
    t_float lfoFrequency;
    t_float lfoDepth;
    
    t_float twopi;
    t_float sample_rate;
    
    t_float lfo_phase;
    t_float lfo_phase_inc;
    
}
t_purple;

static t_int *purple_perform(t_int *w)
{
    t_purple *x = (t_purple *)(w[1]);
    t_float *in = (t_float *)(w[2]);
    t_float *out = (t_float *)(w[3]);

    int n = (int)(w[4]);
    
    //calculate sine wave step
    x->lfo_phase_inc = x->lfoFrequency/x->sample_rate;

    
    while (n--)
    {
        //original signal
        float f = *(in++);
        
        //progression through sine wave steps
        x->lfo_phase += x->lfo_phase_inc;
        
        //input level
        f = x->input * f;
        
        //limit inputted frequency values 0-25
        if (x->lfoFrequency > 25.0f)
        {
            x->lfoFrequency = 25.0f;
        }
        
        if (x->lfoFrequency < 0.0f)
        {
            x->lfoFrequency = 0.0f;
        }
        
        //activate tremolo
        if (x->lfoFrequency > 0)
        {
            //make a copy of the original signal
            float g = f;
            
            //limit inputted depth values 0-10
            if (x->lfoDepth < 0.0f)
            {
                x->lfoDepth = 0.0f;
            }
            
            if (x->lfoDepth > 10.0f)
            {
                x->lfoDepth = 10.0f;
            }
            
            //wet tremolo signal
            f = f * ((sinf(x->twopi * x->lfo_phase)) + 1) * 0.5f;
 
            
            //for depth adjustment: add wet signal with a dry signal
            f = (f * (x->lfoDepth * 0.1)) + (g * (1.0f - (x->lfoDepth * 0.1)));
        }
        
        //first gain stage
        //limit parameter values 0-10
        if (x->dist1 > 10)
        {
            x->dist1 = 10;
        }
        
        if (x->dist1 < 0)
        {
            x->dist1 = 0;
        }
        
        //first gain algorithm
        //Based on Tom Erbe's soft Distortion
        f = (x->dist1 / 2) * 1.5f * f - 0.5f * f * f * f;
        
        //clip to 1
        if (f > 1.0f)
        {
            f = 1.0f;
        }
        
        if (f < -1.0f)
        {
            f = -1.0f;
        }
        
        //second gain stage
        //limit parameter values 0-10
        
        if (x->dist2 > 10)
        {
            x->dist2 = 10;
        }
        
        if (x->dist2 < 0)
        {
            x->dist2 = 0;
        }
        
        //second gain alogorithm
        //Basd on Charle Dodge's Distortion - "Computer Music" p147 - "4x^3+3x"
        f = (x->dist2 / 2) * 4 * f * f * f - 3 * f;
        
        //clip to 1
        if (f > 1.0f)
        {
            f = 1.0f;
        }
        
        if (f < -1.0f)
        {
            f = -1.0f;
        }
        
        *out++ = f;
    }
    return (w+5);
}

void purple_input (t_purple *x, t_floatarg g)
{
    x->input = g;
    //post("Input 1: %f", g);
}

void purple_lfoFrequency (t_purple *x, t_floatarg g)
{
    x->lfoFrequency = g;
    //post("LFO Frequency: %f", g);
}

void purple_lfoDepth (t_purple *x, t_floatarg g)
{
    x->lfoDepth = g;
    //post("LFO Depth: %f", g);
}
void purple_dist1 (t_purple *x, t_floatarg g)
{
    x->dist1 = g;
    //post("Distortion 1: %f", g);
}

void purple_dist2 (t_purple *x, t_floatarg g)
{
    x->dist2 = g;
    //post("Distortion 2: %f", g);
}

static void purple_dsp(t_purple *x, t_signal **sp)
{
    x->sample_rate = sp[0]->s_sr;
    dsp_add(purple_perform, 4, x, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n);
}

static void *purple_new(void)
{
    t_purple *x = (t_purple *)pd_new(purple_class);
    
    //inlets
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("input"));
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("lfoFrequency"));
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("lfoDepth"));
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("dist1"));
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("dist2"));
    
    //outlets
    outlet_new(&x->x_obj, gensym("signal"));
    
    x->x_f = 0;
    
    x->input = 0.0f;
    
    //calculate 2*pi
    x->twopi = atanf(1.0f) * 8.0;
    
    //initialize phase
    x->lfo_phase = 0.0f;
    
    return (x);
}

void purple_tilde_setup(void)
{
    purple_class = class_new(gensym("purple~"), (t_newmethod)purple_new, 0,
                               sizeof(t_purple), 0, A_DEFFLOAT, 0);
    CLASS_MAINSIGNALIN(purple_class, t_purple, x_f);
    class_addmethod(purple_class, (t_method)purple_input, gensym("input"), A_FLOAT, 0);
    class_addmethod(purple_class, (t_method)purple_lfoFrequency, gensym("lfoFrequency"), A_FLOAT, 0);
    class_addmethod(purple_class, (t_method)purple_lfoDepth, gensym("lfoDepth"), A_FLOAT, 0);
    class_addmethod(purple_class, (t_method)purple_dist1, gensym("dist1"), A_FLOAT, 0);
    class_addmethod(purple_class, (t_method)purple_dist2, gensym("dist2"), A_FLOAT, 0);
    class_addmethod(purple_class, (t_method)purple_dsp, gensym("dsp"), 0);
}