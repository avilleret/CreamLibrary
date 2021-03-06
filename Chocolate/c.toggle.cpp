/*
 * CicmWrapper
 *
 * A wrapper for Pure Data
 *
 * Copyright (C) 2013 Pierre Guillot, CICM - Université Paris 8
 * All rights reserved.
 *
 * Website  : http://www.mshparisnord.fr/HoaLibrary/
 * Contacts : cicm.mshparisnord@gmail.com
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2 of the License.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include "../c.library.h"

typedef struct _toggle
{
	t_ebox      j_box;
    t_outlet*   f_out;
	t_rgba		f_color_background;
	t_rgba		f_color_border;
	t_rgba		f_color_cross;
    char        f_active;
} t_toggle;

t_eclass *toggle_class;

static void toggle_output(t_toggle *x)
{
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
    outlet_float((t_outlet*)x->f_out, (float)x->f_active);
    if(ebox_getsender((t_ebox *) x))
        pd_float(ebox_getsender((t_ebox *) x), (float)x->f_active);
}

static void toggle_getdrawparams(t_toggle *x, t_object *patcherview, t_edrawparams *params)
{
	params->d_borderthickness   = 2;
	params->d_cornersize        = 2;
    params->d_bordercolor       = x->f_color_border;
    params->d_boxfillcolor      = x->f_color_background;
}

static void toggle_oksize(t_toggle *x, t_rect *newrect)
{
    newrect->width = pd_clip_min(newrect->width, 15.);
    newrect->height = pd_clip_min(newrect->height, 15.);
}

static void toggle_set(t_toggle *x, float f)
{
    if(f == 0)
        x->f_active = 0;
    else
        x->f_active = 1;
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
}

static void toggle_float(t_toggle *x, float f)
{
    if(f == 0)
        x->f_active = 0;
    else
        x->f_active = 1;
    toggle_output(x);
}

static void toggle_bang(t_toggle *x)
{
    if(x->f_active == 1)
        x->f_active = 0;
    else
        x->f_active = 1;
    toggle_output(x);
}


static t_pd_err toggle_notify(t_toggle *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
	if (msg == cream_sym_attr_modified)
	{
		if(s == cream_sym_bgcolor || s == cream_sym_bdcolor || s == cream_sym_crcolor)
		{
			ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
		}
        ebox_redraw((t_ebox *)x);
	}
	return 0;
}

static void draw_background(t_toggle *x, t_object *view, t_rect *rect)
{
	t_elayer *g = ebox_start_layer((t_ebox *)x, cream_sym_background_layer, rect->width, rect->height);
	if(g)
	{
        if(x->f_active)
        {
            egraphics_set_color_rgba(g, &x->f_color_cross);
            egraphics_set_line_width(g, 2);
            egraphics_line_fast(g, 0, 0, rect->width, rect->height);
            egraphics_line_fast(g, 0, rect->height, rect->width, 0);
        }
        ebox_end_layer((t_ebox*)x, cream_sym_background_layer);
	}
	ebox_paint_layer((t_ebox *)x, cream_sym_background_layer, 0., 0.);
}

static void toggle_paint(t_toggle *x, t_object *view)
{
    t_rect rect;
    ebox_get_rect_for_view((t_ebox *)x, &rect);
    draw_background(x, view, &rect);
}

static void toggle_mousedown(t_toggle *x, t_object *patcherview, t_pt pt, long modifiers)
{
    toggle_bang(x);
}

static void toggle_preset(t_toggle *x, t_binbuf *b)
{
    t_atom av[2];
    atom_setsym(av, &s_float);
    atom_setfloat(av+1, x->f_active);
    binbuf_add(b, 2, av);
}

static void *toggle_new(t_symbol *s, int argc, t_atom *argv)
{
    t_toggle *x = (t_toggle *)eobj_new(toggle_class);
    t_binbuf* d = binbuf_via_atoms(argc, argv);
    
    if(x && d)
    {
        ebox_new((t_ebox *)x, 0 | EBOX_GROWLINK);
        x->f_active = 0;
        x->f_out = (t_outlet *)floatout(x);
        ebox_attrprocess_viabinbuf(x, d);
        ebox_ready((t_ebox *)x);
    }
    
    return (x);
}

extern "C" void setup_c0x2etoggle(void)
{
    t_eclass *c;
    
    c = eclass_new("c.toggle", (method)toggle_new, (method)ebox_free, (short)sizeof(t_toggle), 0L, A_GIMME, 0);
    
    eclass_init(c, 0);
    cream_initclass(c);
    
    eclass_addmethod(c, (method) toggle_paint,           "paint",            A_NULL, 0);
    eclass_addmethod(c, (method) toggle_notify,          "notify",           A_NULL, 0);
    eclass_addmethod(c, (method) toggle_getdrawparams,   "getdrawparams",    A_NULL, 0);
    eclass_addmethod(c, (method) toggle_oksize,          "oksize",           A_NULL, 0);
    eclass_addmethod(c, (method) toggle_float,           "float",            A_FLOAT,0);
    eclass_addmethod(c, (method) toggle_set,             "set",              A_FLOAT,0);
    eclass_addmethod(c, (method) toggle_bang,            "bang",             A_NULL, 0);
    eclass_addmethod(c, (method) toggle_mousedown,       "mousedown",        A_NULL, 0);
    eclass_addmethod(c, (method) toggle_preset,          "preset",           A_NULL, 0);
    
    CLASS_ATTR_INVISIBLE            (c, "fontname", 1);
    CLASS_ATTR_INVISIBLE            (c, "fontweight", 1);
    CLASS_ATTR_INVISIBLE            (c, "fontslant", 1);
    CLASS_ATTR_INVISIBLE            (c, "fontsize", 1);
    CLASS_ATTR_DEFAULT              (c, "size", 0, "15. 15.");
    
    CLASS_ATTR_RGBA                 (c, "bgcolor", 0, t_toggle, f_color_background);
    CLASS_ATTR_LABEL                (c, "bgcolor", 0, "Background Color");
    CLASS_ATTR_ORDER                (c, "bgcolor", 0, "1");
    CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bgcolor", 0, "0.75 0.75 0.75 1.");
    CLASS_ATTR_STYLE                (c, "bgcolor", 0, "color");
    
    CLASS_ATTR_RGBA                 (c, "bdcolor", 0, t_toggle, f_color_border);
    CLASS_ATTR_LABEL                (c, "bdcolor", 0, "Border Color");
    CLASS_ATTR_ORDER                (c, "bdcolor", 0, "2");
    CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bdcolor", 0, "0.5 0.5 0.5 1.");
    CLASS_ATTR_STYLE                (c, "bdcolor", 0, "color");
    
    CLASS_ATTR_RGBA                 (c, "crcolor", 0, t_toggle, f_color_cross);
    CLASS_ATTR_LABEL                (c, "crcolor", 0, "Cross Color");
    CLASS_ATTR_ORDER                (c, "crcolor", 0, "3");
    CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "crcolor", 0, "0.5 0.5 0.5 1.");
    CLASS_ATTR_STYLE                (c, "crcolor", 0, "color");
    
    eclass_register(CLASS_BOX, c);
    toggle_class = c;
}







