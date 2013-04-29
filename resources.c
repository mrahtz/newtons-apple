#include "resources.h"

// this isn't needed anywhere else
// TODO does this really belong here?
static void load_respawn(object *o, int n)
{
    switch(n) {
        case APPLE:
            o->respawn_interval = APPLE_RESPAWN_INTERVAL;
            break;
        case PROJECTILE:
            o->respawn_interval = (int) PROJECTILE_RESPAWN_INTERVAL * rand_between(1.0, 2.0);
            break;
        case BIRD:
            o->respawn_interval = (int) BIRD_RESPAWN_INTERVAL * rand_between(1.0, 2.0);
            break;
    }
}

void load_resources(ALLEGRO_FONT **font, intro_resource_struct *intro_resources, object *objects)
{
    *font = al_load_ttf_font("Arial.ttf",
                            36,     // size
                            0);     // flags
    if (!(*font))
        die("couldn't load font\n");
    intro_resources->font = al_load_ttf_font("Arial.ttf",
                                             20,
                                             0);

    intro_resources->instructions1 = al_load_bitmap("instructions1.png");
    intro_resources->instructions2 = al_load_bitmap("instructions2.png");
    intro_resources->tree = al_load_bitmap("tree.png");

    enum object_ctr i;
    for (i = 0; i < LAST_MOVER; i++) {
        init_sprite(&objects[i], i);
        load_respawn(&objects[i], i);
    }
    objects[GROUND].sprite1 = al_load_bitmap("ground.png");
    objects[GROUND].sprite2 = objects[GROUND].sprite3 = NULL;
    objects[GROUND].width = al_get_bitmap_width(objects[GROUND].sprite1);
    objects[GROUND].height = al_get_bitmap_height(objects[GROUND].sprite1);
    objects[GROUND].x_pos = 0;
    objects[GROUND].y_pos = CANVAS_HEIGHT-1 -
                            al_get_bitmap_height(objects[GROUND].sprite1) + 1;
}

void free_resources(ALLEGRO_FONT *font, intro_resource_struct *intro_resources, object *objects)
{
    enum object_ctr i;
    for (i = 0; i < LAST_MOVER; i++) {
        object o = objects[i];
        // some of these may be NULL, but al_destroy_bitmap is ok with that :)
        al_destroy_bitmap(o.sprite1);
        al_destroy_bitmap(o.sprite2);
        al_destroy_bitmap(o.sprite3);
    }

    al_destroy_bitmap(intro_resources->tree);
    al_destroy_bitmap(intro_resources->instructions2);
    al_destroy_bitmap(intro_resources->instructions1);

    al_destroy_font(font);
}