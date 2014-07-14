//
//  epd_ugen.c
//  Camomile
//
//  Created by Guillot Pierre on 14/07/2014.
//
//

#include "epd_ugen.h"
#include "string.h"

EXTERN t_sample *sys_soundout;
EXTERN t_sample *sys_soundin;
// Changes
/*
    m_pd.c and m_pd.h :
    t_pdinstance* pd_getinstance()
 
    d_ugen.c and m_pd.h :
    EXTERN t_signal** get_signal_freelist();
    EXTERN t_signal* get_signal_freeborrowed();
    EXTERN t_signal* get_signal_usedlist();
*/

t_epd_process *epd_process_new()
{
    sys_lock();
    t_epd_process *x = (t_epd_process *)getbytes(sizeof(*x));
    x->p_top        = pd_getinstance();
    x->p_instance   = pdinstance_new();
    x->p_canvas     = NULL;
    x->p_phase      = 0;
    
    x->p_vector_size    = 0;
    x->p_ninputs        = 0;
    x->p_noutputs       = 0;
    x->p_input_pd       = NULL;
    x->p_output_pd      = NULL;
    x->p_sysin_pd       = NULL;
    x->p_sysout_pd      = NULL;
    
    pd_setinstance(x->p_top);
    sys_unlock();
    return x;
}

void epd_process_free(t_epd_process *x)
{
    epd_process_dspstop(x);
    sys_lock();
    pd_setinstance(x->p_instance);
    if(x->p_canvas)
        libpd_closefile(x->p_canvas);
    pd_setinstance(x->p_top);
    // freebytes !!
    sys_unlock();
}

int epd_process_open(t_epd_process* x, char* name, char* dir)
{
    sys_lock();
    pd_setinstance(x->p_instance);
    if(x->p_canvas)
        libpd_closefile(x->p_canvas);
    x->p_canvas = NULL;
    x->p_canvas = libpd_openfile(name, dir);
    pd_setinstance(x->p_top);
    sys_unlock();
    if(x->p_canvas)
        return 0;
    else
        return 1;    
}

int epd_process_dspstart(t_epd_process *x, int nins, int nouts, float samplerate, float vectorsize)
{
    t_signal* list;
    t_signal** freelist;
    t_gobj          *y;
    t_symbol*  sym_cin = gensym("c.in~");
    t_symbol*  sym_cout = gensym("c.out~");
    t_cinbis* cin;
    t_coutbis* cout;
    int nsys_ins = 2;
    int nsys_outs = 2;
    if (x->p_ninputs > nsys_ins)
    {
        nsys_ins = x->p_ninputs;
    }
    if (x->p_noutputs > nsys_outs)
    {
        nsys_outs = x->p_output_pd;
    }
    
    if(x->p_input_pd)
    {
        freebytes(x->p_input_pd, sizeof(float) * x->p_vector_size * x->p_ninputs);
        x->p_input_pd = NULL;
    }
    if(x->p_output_pd)
    {
        freebytes(x->p_output_pd, sizeof(float) * x->p_vector_size * x->p_noutputs);
        x->p_output_pd = NULL;
    }
    
    x->p_ninputs    = nins;
    x->p_noutputs   = nouts;
    if(x->p_ninputs < 0)
        x->p_ninputs = 0;
    if(x->p_noutputs < 0)
        x->p_noutputs = 0;
    
    x->p_vector_size = vectorsize;
    x->p_input_pd  = (float *)getbytes(sizeof(float) * x->p_vector_size * nsys_ins);
    x->p_output_pd = (float *)getbytes(sizeof(float) * x->p_vector_size * nsys_outs);
    
    x->p_sysin_pd  = (t_sample *)getbytes(sizeof(t_sample) * DEFDACBLKSIZE * nsys_ins);
    x->p_sysout_pd = (t_sample *)getbytes(sizeof(t_sample) * DEFDACBLKSIZE * nsys_outs);
    
    if(x->p_input_pd && x->p_output_pd && x->p_sysin_pd && x->p_sysout_pd)
    {
        memset(x->p_input_pd, 0, sizeof(float) * x->p_vector_size * nsys_ins);
        memset(x->p_output_pd, 0, sizeof(float) * x->p_vector_size * nsys_outs);
        
        memset(x->p_sysin_pd, 0, sizeof(t_sample) * DEFDACBLKSIZE * nsys_ins);
        memset(x->p_sysout_pd, 0, sizeof(t_sample) * DEFDACBLKSIZE * nsys_outs);
        
        sys_lock();
        pd_setinstance(x->p_instance);
        libpd_init_audio(x->p_ninputs, x->p_noutputs, samplerate);
        
        if(x->p_canvas)
        {
            for(y = x->p_canvas->gl_list; y; y = y->g_next)
            {
                if(y->g_pd->c_name == sym_cin)
                {
                    cin = (t_cinbis *)y;
                    cin->x_inputs = x->p_sysin_pd;
                }
                else if(y->g_pd->c_name == sym_cout)
                {
                    cout = (t_coutbis *)y;
                    cout->x_outputs = x->p_sysout_pd;
                }
            }
        }
        
        libpd_start_message(1);
        libpd_add_float(1.f);
        libpd_finish_message("pd", "dsp");
        
        freelist = get_signal_freelist();
        for(int i = 0; i <= MAXLOGSIG; i++)
        {
            x->p_sigfreelist[i] = freelist[i];
        }
        setzero_signal_freelist();
        
        x->p_sigfreeborrowed = get_signal_freeborrowed();
        setzero_signal_freeborrowed();
        
        x->p_sigusedlist = get_signal_usedlist();
        setzero_signal_usedlist();
        
        pd_setinstance(x->p_top);
        sys_unlock();
        
        return 0;
    }
    
    return 1;
}

void epd_process_dspsuspend(t_epd_process *x)
{
    int nsys_ins = 2;
    int nsys_outs = 2;
    if (x->p_ninputs > nsys_ins)
    {
        nsys_ins = x->p_ninputs;
    }
    if (x->p_noutputs > nsys_outs)
    {
        nsys_outs = x->p_output_pd;
    }
    
    if(x->p_input_pd)
        memset(x->p_input_pd, 0, sizeof(float) * x->p_vector_size * nsys_ins);
    if(x->p_output_pd)
        memset(x->p_output_pd, 0, sizeof(float) * x->p_vector_size * nsys_outs);
}

void epd_process_dspstop(t_epd_process *x)
{
    int i;
    t_signal **svec, *sig, *sig2;
    int nsys_ins = 2;
    int nsys_outs = 2;
    if (x->p_ninputs > nsys_ins)
    {
        nsys_ins = x->p_ninputs;
    }
    if (x->p_noutputs > nsys_outs)
    {
        nsys_outs = x->p_output_pd;
    }
    
    if(x->p_input_pd)
    {
        freebytes(x->p_input_pd, sizeof(float) * x->p_vector_size * nsys_ins);
        x->p_input_pd = NULL;
    }
    if(x->p_output_pd)
    {
        freebytes(x->p_output_pd, sizeof(float) * x->p_vector_size * nsys_outs);
        x->p_output_pd = NULL;
    }
    
    if(x->p_sysin_pd)
    {
        freebytes(x->p_sysin_pd, sizeof(t_sample) * DEFDACBLKSIZE * nsys_ins);
        x->p_sysin_pd = NULL;
    }
    if(x->p_sysout_pd)
    {
        freebytes(x->p_sysout_pd, sizeof(t_sample) * DEFDACBLKSIZE * nsys_outs);
        x->p_sysout_pd = NULL;
    }
    
    while(sig = x->p_sigusedlist)
    {
        x->p_sigusedlist = sig->s_nextused;
        if (!sig->s_isborrowed)
            t_freebytes(sig->s_vec, sig->s_vecsize * sizeof (*sig->s_vec));
        t_freebytes(sig, sizeof *sig);
    }
    for (i = 0; i <= MAXLOGSIG; i++)
        x->p_sigfreelist[i] = 0;
    x->p_sigfreeborrowed = 0;
}

void eprocess_sched_tick(t_pdinstance *x)
{
    /*
    double next_sys_time = x->p_systime + sys_time_per_dsp_tick;
    int countdown = 5000;
    while (x->p_clock_setlist && x->p_clock_setlist->c_settime < next_sys_time)
    {
        t_clock *c = x->p_clock_setlist;
        pd_this->pd_systime = c->c_settime;
        clock_unset(pd_this->pd_clock_setlist);
        outlet_setstacklim();
        (*c->c_fn)(c->c_owner);
        if (!countdown--)
        {
            countdown = 5000;
            sys_pollgui();
        }
        if (sys_quit)
            return;
    }
    x->pd_systime = next_sys_time;
     */
    if(x->pd_dspchainsize)
    {
        t_int *ip;
        for(ip = x->pd_dspchain; ip; )
            ip = (*(t_perfroutine)(*ip))(ip);
        //dsp_phase++;
    }
    //sched_diddsp++;
}

void epd_process_process(t_epd_process *x, const float** inputs, float** outputs)
{
    int i, j, k;
    int ticks = x->p_vector_size / DEFDACBLKSIZE;
    t_sample *p0, *p1;
    int nsys_ins = 2;
    int nsys_outs = 2;
    t_sample* ins = x->p_sysin_pd;
    t_sample* outs = x->p_sysout_pd;
    if (x->p_ninputs > nsys_ins)
    {
        nsys_ins = x->p_ninputs;
    }
    if (x->p_noutputs > nsys_outs)
    {
        nsys_outs = x->p_output_pd;
    }
    
    
    if (x->p_sysout_pd && x->p_sysin_pd)
    {

#ifdef __APPLE__
    for(int i = 0; i < x->p_ninputs; i++)
    {
        cblas_scopy(x->p_vector_size, inputs[i], 1, x->p_input_pd+i, x->p_ninputs);
    }
#endif
    for (i = 0; i < ticks; i++)
    {
        /*
        for (j = 0, p0 = x->p_sysin_pd; j < DEFDACBLKSIZE; j++, p0++)
        {
            for (k = 0, p1 = p0; k < nsys_ins; k++, p1 += DEFDACBLKSIZE)
            {
                *p1 = * x->p_input_pd++;
            }
        }
        */
        //memset(x->p_sysout_pd, 0, nsys_outs * DEFDACBLKSIZE * sizeof(t_sample));
        
        eprocess_sched_tick(x->p_instance);
        
        for (j = 0, p0 = outs; j < DEFDACBLKSIZE; j++, p0++)
        {
            for (k = 0, p1 = p0; k < nsys_outs; k++, p1 += DEFDACBLKSIZE)
            {
                *x->p_output_pd++ = *p1;
            }
        }
    }
#ifdef __APPLE__
    for(int i = 0; i < x->p_noutputs; i++)
    {
        cblas_scopy(x->p_vector_size, x->p_output_pd+i, x->p_noutputs, outputs[i], 1);
    }
#endif
    }
}


