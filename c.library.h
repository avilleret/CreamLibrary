/*
// Copyright (c) 2012-2014 Eliott Paris, Julien Colafrancesco & Pierre Guillot, CICM, Universite Paris 8.
// For information on usage and redistribution, and for a DISCLAIMER OF ALL
// WARRANTIES, see the file, "LICENSE.txt," in this distribution.
*/

#ifndef DEF_CREAM_LIBRARY
#define DEF_CREAM_LIBRARY

extern "C"
{
#include "ThirdParty/PureData/Sources/cicm_wrapper.h"
}

void cream_initclass(t_eclass* c);

extern "C" void cream_setup(void);
extern "C" void libpd_loadcream(void);

// Cinnamon
extern "C"  void setup_c0x2eatan_tilde(void);
extern "C"  void setup_c0x2esin_tilde(void);

// Caramel
extern "C" void setup_c0x2econvolve_tilde(void);
extern "C" void setup_c0x2ecount_tilde(void);
extern "C" void setup_c0x2efft_tilde(void);
extern "C" void setup_c0x2efreeverb_tilde(void);
extern "C" void setup_c0x2ematrix_tilde(void);

// Chocolate
extern "C" void setup_c0x2ebang(void);
extern "C" void setup_c0x2eblackboard(void);
extern "C" void setup_c0x2ebreakpoints(void);
extern "C" void setup_c0x2ecolorpanel(void);
extern "C" void setup_c0x2edsp_tilde(void);
extern "C" void setup_c0x2egain_tilde(void);
extern "C" void setup_c0x2eincdec(void);
extern "C" void setup_c0x2eknob(void);
extern "C" void setup_c0x2ematrix(void);
extern "C" void setup_c0x2emenu(void);
extern "C" void setup_c0x2emeter_tilde(void);
extern "C" void setup_c0x2enumber(void);
extern "C" void setup_c0x2enumber_tilde(void);
extern "C" void setup_c0x2eplane(void);
extern "C" void setup_c0x2epreset(void);
extern "C" void setup_c0x2eradio(void);
extern "C" void setup_c0x2erslider(void);
extern "C" void setup_c0x2escope_tilde(void);
extern "C" void setup_c0x2eslider(void);
extern "C" void setup_c0x2etab(void);
extern "C" void setup_c0x2etoggle(void);

// Coffee

extern "C" void setup_c0x2eloadmess(void);
extern "C" void setup_c0x2epak(void);
extern "C" void setup_c0x2epatcherargs(void);
extern "C" void setup_c0x2epatcherinfos(void);
extern "C" void setup_c0x2epatchermess(void);
extern "C" void setup_c0x2eprepend(void);
extern "C" void setup_c0x2emousestate(void);

static t_symbol* cream_sym_attr_modified        = gensym("attr_modified");

static t_symbol* cream_sym_background_layer     = gensym("background_layer");
static t_symbol* cream_sym_text_layer           = gensym("text_layer");
static t_symbol* cream_sym_selection_layer      = gensym("selection_layer");

static t_symbol* cream_sym_bacolor              = gensym("bacolor");
static t_symbol* cream_sym_bgcolor              = gensym("bgcolor");
static t_symbol* cream_sym_bdcolor 				= gensym("bdcolor");
static t_symbol* cream_sym_crcolor 				= gensym("crcolor");
static t_symbol* cream_sym_btcolor              = gensym("btcolor");

static t_symbol* cream_sym_fontsize             = gensym("fontsize");
static t_symbol* cream_sym_fontname             = gensym("fontname");
static t_symbol* cream_sym_fontweight           = gensym("fontweight");
static t_symbol* cream_sym_fontslant            = gensym("fontslant");

static t_symbol* cream_sym_preset               = gensym("preset");
static t_symbol* cream_sym_atpreset             = gensym("@preset");
static t_symbol* cream_sym_atindex              = gensym("@index");
static t_symbol* cream_sym_interpolate          = gensym("interpolate");

static t_symbol* cream_sym_nothing              = gensym("''");
static t_symbol* cream_sym_left_bracket         = gensym("[");
static t_symbol* cream_sym_right_bracket        = gensym("]");

#endif
