/**
	@file
	sc_pinfo_textures - an object to get all texture names in a patch
	Connor Rawls - cwrawls@asu.edu
    Synthesis Center, ASU
    2019
*/

#include "ext.h"							// standard Max include, always required
#include "ext_obex.h"						// required for new style Max object
#include "jit.common.h"
//#include "jit.gl.h"

/* ===================== OBJECT STRUCT ========================= */
typedef struct _sc_pinfo_textures
{
    t_object    ob;
    long        parent_start;
    long        use_recursion;
    t_symbol    context;
    void*       out;
} t_sc_pinfo_textures;

/* ==================== FUNCTION PROTOTYPES ==================== */

void *t_sc_pinfo_textures_new(t_symbol* s, long argc, t_atom* argv);
void t_sc_pinfo_textures_free(t_sc_pinfo_textures* x);
void t_sc_pinfo_textures_assist(t_sc_pinfo_textures* x, void* b, long m, long a, char* s);
void t_sc_pinfo_textures_bang(t_sc_pinfo_textures* x);
long t_sc_pinfo_textures_search_sub(t_object* patcher, t_symbol c);
long t_sc_pinfo_textures_get_sub_names(t_atom** list, t_object* patcher, t_symbol c);

//attribute accessors
void sc_pinfo_textures_get_parent(t_sc_pinfo_textures *x, t_object *attr, long *argc, t_atom **argv);
void sc_pinfo_textures_get_recursion(t_sc_pinfo_textures *x, t_object *attr, long *argc, t_atom **argv);
void sc_pinfo_textures_get_context(t_sc_pinfo_textures *x, t_object *attr, long *argc, t_atom **argv);

//attribute mutators
void sc_pinfo_textures_set_parent(t_sc_pinfo_textures *x, void *attr, long argc, t_atom *argv);
void sc_pinfo_textures_set_recursion(t_sc_pinfo_textures *x, void *attr, long argc, t_atom *argv);
void sc_pinfo_textures_set_context(t_sc_pinfo_textures *x, void *attr, long argc, t_atom *argv);

/* ==================== CLASS REGISTRATION ====================== */

void *sc_pinfo_textures_class;

void ext_main(void *r)
{
    t_class* c;
    c = class_new("sc.pinfo.textures", (method)t_sc_pinfo_textures_new, (method)t_sc_pinfo_textures_free, (long)sizeof(t_sc_pinfo_textures), 0L, A_GIMME, 0);
    
    class_addmethod(c, (method)t_sc_pinfo_textures_bang, "bang", 0);
    class_addmethod(c, (method)t_sc_pinfo_textures_assist, "assist", A_CANT, 0);
    
    CLASS_ATTR_LONG(c, "top_level_start", 0, t_sc_pinfo_textures, parent_start);
    CLASS_ATTR_STYLE(c, "top_level_start", 0, "onoff");
    CLASS_ATTR_ACCESSORS(c, "top_level_start", sc_pinfo_textures_get_parent, sc_pinfo_textures_set_parent);
    
    CLASS_ATTR_LONG(c, "check_children", 0, t_sc_pinfo_textures, use_recursion);
    CLASS_ATTR_STYLE(c, "check_children", 0, "onoff");
    CLASS_ATTR_ACCESSORS(c, "check_children", sc_pinfo_textures_get_recursion, sc_pinfo_textures_set_recursion);
    
    CLASS_ATTR_SYM(c, "context", 0, t_sc_pinfo_textures, context);
    CLASS_ATTR_STYLE(c, "context", 0, "text");
    CLASS_ATTR_ACCESSORS(c, "context", sc_pinfo_textures_get_context, sc_pinfo_textures_set_context);
    
    class_register(CLASS_BOX, c);
    sc_pinfo_textures_class = c;
}

void t_sc_pinfo_textures_assist(t_sc_pinfo_textures* x, void* b, long m, long a, char* s) {
    if(m == ASSIST_INLET) {
        sprintf(s, "Bang to get texture name list");
    } else {
        sprintf(s, "Texture name list output");
    }
}


//ATTRIBUTE MUTATORS

void sc_pinfo_textures_set_parent(t_sc_pinfo_textures *x, void *attr, long argc, t_atom *argv){
    if(argv && argc) {
        long tempP = 0;
        switch (atom_gettype(argv)) {
            case A_LONG:
            tempP = atom_getlong(argv);
            break;
            default:
            object_error((t_object*)x, "Bad value for top_level_start, expected 1 or 0");
            return;
            break;
        }
        tempP = (tempP >= 1) ? 1 : 0;
        x->parent_start = tempP;
    }
}

void sc_pinfo_textures_set_recursion(t_sc_pinfo_textures *x, void *attr, long argc, t_atom *argv){
    if(argv && argc) {
        long tempP = 0;
        switch (atom_gettype(argv)) {
            case A_LONG:
            tempP = atom_getlong(argv);
            break;
            default:
            object_error((t_object*)x, "Bad value for check_children, expected 1 or 0");
            return;
            break;
        }
        tempP = (tempP >= 1) ? 1 : 0;
        x->use_recursion = tempP;
    }
}

void sc_pinfo_textures_set_context(t_sc_pinfo_textures *x, void *attr, long argc, t_atom *argv){
    if(argv && argc) {
        char temp[255];
        long ltemp;
        t_symbol ctemp = x->context;
        switch(atom_gettype(argv)) {
            case A_SYM:
                ctemp = *(atom_getsym(argv));
                break;
            case A_LONG:
            ltemp = atom_getlong(argv);
            sprintf(temp,"%lld",ltemp);
                ctemp = *(gensym(temp));
                break;
            default:
                object_error((t_object*)x, "Bad value for context, expected a symbol");
                return;
                break;
        }
        
        x->context = (strcmp(ctemp.s_name, "0") == 0) ? *gensym("") : ctemp;
    }
}

//attribute accessors
void sc_pinfo_textures_get_parent(t_sc_pinfo_textures *x, t_object *attr, long *argc, t_atom **argv){
    char alloc;
    long p = 0;
    atom_alloc(argc, argv, &alloc);
    p = x->parent_start;
    atom_setlong(*argv, p);
}

void sc_pinfo_textures_get_recursion(t_sc_pinfo_textures *x, t_object *attr, long *argc, t_atom **argv){
    char alloc;
    long r = 0;
    atom_alloc(argc, argv, &alloc);
    r = x->use_recursion;
    atom_setlong(*argv, r);
}

void sc_pinfo_textures_get_context(t_sc_pinfo_textures *x, t_object *attr, long *argc, t_atom **argv) {
    char alloc;
    t_symbol c = x->context;
    atom_alloc(argc, argv, &alloc);
    atom_setsym(*argv, &c);
}


//MAIN OBJECT CALCULATION
void t_sc_pinfo_textures_bang(t_sc_pinfo_textures* x) {
    t_object* my_patcher = NULL;
    
    object_obex_lookup(x, gensym("#P"), &my_patcher);
    
    if(x->parent_start) {
        t_object* parent_temp;
        do{
            parent_temp = jpatcher_get_parentpatcher(my_patcher);
            if(parent_temp) {
                my_patcher = parent_temp;
            }
        } while(parent_temp);
    }
    
    //object_post((t_object*)x, "My patcher is at addr %lx", my_patcher);
    
    t_object* box, *obj;
    
    int tex_count = 0;
    
    for(box = jpatcher_get_firstobject(my_patcher); box; box = jbox_get_nextobject(box)) {
        obj = jbox_get_object(box);
        if(obj)
        {
            //object_post((t_object*)x, "Object: %s", object_classname(obj)->s_name);
            if(strcmp(object_classname(obj)->s_name, "jit.gl.texture") == 0 && ((strcmp(jit_attr_getsym(obj, gensym("drawto"))->s_name, x->context.s_name) == 0 || strcmp(x->context.s_name, "") == 0))) {
                tex_count++;
            }
            
            if(strcmp(object_classname(obj)->s_name, "jpatcher") == 0 && x->use_recursion) {
                tex_count += t_sc_pinfo_textures_search_sub(obj, x->context);
            }
        } else {
            //object_post((t_object*)x, "Null object");
        }
    }
    
    
    if(tex_count > 0) {
        void* tex_list = sysmem_newptr(sizeof(t_atom) * tex_count);
        t_atom* temp = tex_list;
        for(box = jpatcher_get_firstobject(my_patcher); box; box = jbox_get_nextobject(box)) {
            obj = jbox_get_object(box);
            if(obj)
            {
                //object_post((t_object*)x, "Object: %s", object_classname(obj)->s_name);
                //if(strcmp(object_classname(obj)->s_name, "jit.gl.texture") == 0)
                if(strcmp(object_classname(obj)->s_name, "jit.gl.texture") == 0 && ((strcmp(jit_attr_getsym(obj, gensym("drawto"))->s_name, x->context.s_name) == 0 || strcmp(x->context.s_name, "") == 0))){
                    //object_post((t_object*)x, "Found a texture object!");
                    t_symbol* tname = jit_attr_getsym((void*)obj, gensym("name"));
                    if(tname) {
                        //object_post((t_object*)x, "Tex Name: %s", tname->s_name);
                        atom_setsym(temp, tname);
                        temp++;
                    }
                }
                if(strcmp(object_classname(obj)->s_name, "jpatcher") == 0 && x->use_recursion) {
                    t_atom* subtemp = (t_atom*)sysmem_newptr(sizeof(t_atom) * t_sc_pinfo_textures_search_sub(obj, x->context));
                    long found_names = t_sc_pinfo_textures_get_sub_names(&subtemp, obj, x->context);
                    sysmem_copyptr(subtemp, temp, sizeof(t_atom) * found_names);
                    temp += found_names;
                    sysmem_freeptr(subtemp);
                    //object_post((t_object*)x, "Current atom: %s", atom_getsym(temp)->s_name);
                }
            } else {
                //object_post((t_object*)x, "Null object");
            }
        }
        outlet_list(x->out, gensym("tex_names"), tex_count, tex_list);
        
        sysmem_freeptr(tex_list);
    }
}

long t_sc_pinfo_textures_search_sub(t_object* patcher, t_symbol c){
    t_object* box, *obj;
    
    long temp_count = 0;
    
    //post("While in another patcher...");
    
    for(box = jpatcher_get_firstobject(patcher); box; box = jbox_get_nextobject(box)) {
        obj = jbox_get_object(box);
        if(obj) {
            //post("Found %s", object_classname(obj)->s_name);
            //if(strcmp(object_classname(obj)->s_name, "jit.gl.texture") == 0)
            if(strcmp(object_classname(obj)->s_name, "jit.gl.texture") == 0 && ((strcmp(jit_attr_getsym(obj, gensym("drawto"))->s_name, c.s_name) == 0 || strcmp(c.s_name, "") == 0))){
                temp_count += 1;
            }
            if(strcmp(object_classname(obj)->s_name, "jpatcher") == 0) {
                temp_count += t_sc_pinfo_textures_search_sub(obj, c);
            }
        }
    }
    
    return temp_count;
}

long t_sc_pinfo_textures_get_sub_names(t_atom** list, t_object* patcher, t_symbol c) {
    
    long found_names = t_sc_pinfo_textures_search_sub(patcher, c);
    
    t_atom* temp_list = *list;
    
    t_object* box, *obj;
    
    for(box = jpatcher_get_firstobject(patcher); box; box = jbox_get_nextobject(box)) {
        obj = jbox_get_object(box);
        if(obj)
        {
            //object_post((t_object*)x, "Object: %s", object_classname(obj)->s_name);
            //if(strcmp(object_classname(obj)->s_name, "jit.gl.texture") == 0)
            if(strcmp(object_classname(obj)->s_name, "jit.gl.texture") == 0 && ((strcmp(jit_attr_getsym(obj, gensym("drawto"))->s_name, c.s_name) == 0 || strcmp(c.s_name, "") == 0))){
               // object_post((t_object*)x, "Found a texture object!");
                t_symbol* tname = jit_attr_getsym((void*)obj, gensym("name"));
                if(tname) {
                    //object_post((t_object*)x, "Tex Name: %s", tname->s_name);
                    atom_setsym(temp_list, tname);
                    temp_list++;
                    //post("%s", tname->s_name);
                }
            }
            if(strcmp(object_classname(obj)->s_name, "jpatcher") == 0) {
                t_atom* sublist = (t_atom*)sysmem_newptr(sizeof(t_atom) * t_sc_pinfo_textures_search_sub(obj, c));
                long new_names = t_sc_pinfo_textures_get_sub_names(&sublist, obj, c);
                sysmem_copyptr(sublist, temp_list, sizeof(t_atom) * new_names);
                temp_list += new_names;
                sysmem_freeptr(sublist);
            }
        } else {
            //object_post((t_object*)x, "Null object");
        }
    }
    
    return found_names;
}

void t_sc_pinfo_textures_free(t_sc_pinfo_textures* x) {
    ;
}

void *t_sc_pinfo_textures_new(t_symbol* s, long argc, t_atom* argv){
    t_sc_pinfo_textures* x = NULL;
    
    if((x = (t_sc_pinfo_textures*)object_alloc(sc_pinfo_textures_class))) {
        x->out = outlet_new(x, NULL);
        x->context = *gensym("");
        x->parent_start = 0;
        x->use_recursion = 0;
        attr_args_process(x, argc, argv);
    }
    
    return x;
}
