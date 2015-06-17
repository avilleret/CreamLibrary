/*
 * PdEnhanced - Pure Data Enhanced
 *
 * An add-on for Pure Data
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


typedef struct  _patcherinfos
{
	t_eobj      j_box;
    t_canvas*   f_canvas;
	t_outlet*   f_out_name;
    t_outlet*   f_out_path;
    t_outlet*   f_out_coords;

    double      f_time;
} t_patcherinfos;

static t_eclass *patcherinfos_class;

static void patcherinfos_output(t_patcherinfos *x)
{
    t_atom av[4];
    if(x->f_canvas)
    {
        outlet_symbol(x->f_out_name, x->f_canvas->gl_name);
        outlet_symbol(x->f_out_path, canvas_getdir(x->f_canvas));
        
        atom_setfloat(av, x->f_canvas->gl_screenx1);
        atom_setfloat(av+1, x->f_canvas->gl_screeny1);
        atom_setfloat(av+2, x->f_canvas->gl_screenx2);
        atom_setfloat(av+3, x->f_canvas->gl_screeny2);
        outlet_anything(x->f_out_coords, gensym("windowsize"), 4, av);
        
        atom_setfloat(av, x->f_canvas->gl_xmargin);
        atom_setfloat(av+1, x->f_canvas->gl_ymargin);
        atom_setfloat(av+2, x->f_canvas->gl_pixwidth);
        atom_setfloat(av+3, x->f_canvas->gl_pixheight);
        outlet_anything(x->f_out_coords, gensym("canvassize"), 4, av);
    }
}


static void *patcherinfos_new(t_symbol *s, int argc, t_atom *argv)
{
    t_patcherinfos *x = (t_patcherinfos *)eobj_new(patcherinfos_class);
    if(x)
    {
        if(canvas_getcurrent())
        {
            x->f_canvas = glist_getcanvas(canvas_getcurrent());
        }
        else
        {
            x->f_canvas = NULL;
        }
        x->f_out_name = outlet_new((t_object *)x, &s_symbol);
        x->f_out_path = outlet_new((t_object *)x, &s_symbol);
        x->f_out_coords = outlet_new((t_object *)x, &s_list);
        x->f_time = clock_getsystime();
    }
    return x;
}

extern "C" void setup_c0x2epatcherinfos(void)
{
	t_eclass *c = eclass_new("c.patcherinfos", (method)patcherinfos_new, (method)eobj_free, (short)sizeof(t_patcherinfos), 0L, A_GIMME, 0);
    class_addcreator((t_newmethod)patcherinfos_new, gensym("c.canvasinfos"), A_GIMME, 0);
    eclass_addmethod(c, (method)patcherinfos_output,      "bang",       A_NULL, 0);
    eclass_register(CLASS_OBJ, c);
    patcherinfos_class = c;
}


