#include "resources.h"
// contains hex dumps of images and font in arrays
#include "resources/packed_resources.c"

ALLEGRO_FONT * load_packed_font(unsigned char *mem, int mem_size, int font_size)
{
    ALLEGRO_FILE *mem_file = al_open_memfile(mem, mem_size, "r");
    // "The filename (second arg) is only used to find possible additional files next to a font file."
    // ? But works with just a blank string
    ALLEGRO_FONT *font = al_load_ttf_font_f(mem_file, "", font_size, 0); // last variable flags
    // https://www.allegro.cc/manual/5/al_load_ttf_font_f
    // "The file handle is owned by the returned ALLEGRO_FONT object and must not be freed by the caller,
    //  as FreeType expects to be able to read from it at a later time."
    return font;
}

ALLEGRO_BITMAP * load_packed_bitmap(unsigned char *mem, int mem_size)
{
    // fail nicely :)
    if (mem == NULL)
        return NULL;
    ALLEGRO_FILE *mem_file = al_open_memfile(mem, mem_size, "r");
    // file type determined by second arg
    ALLEGRO_BITMAP *bm = al_load_bitmap_f(mem_file, ".png");
    al_fclose(mem_file);

    return bm;
}

static void init_sprite(object *o, int object_n)
{
    int size1, size2;
    unsigned char *mem1, *mem2;

    // so can call al_destroy_bitmap on them later,
    // even if they're unused
    o->sprite1 = o->sprite2 = o->sprite3 = NULL;

    switch (object_n) {
        case APPLE:
            mem1 = mem2 = apple_png;
            size1 = size2 = apple_png_size;
            break;
        case BIRD:
            mem1 = bird1_png;
            mem2 = bird2_png;
            size1 = bird1_png_size;
            size2 = bird2_png_size;
            break;
        case PROJECTILE:
            mem1 = mem2 = book_png;
            size1 = size2 = book_png_size;
            break;
        case TREE:
            mem1 = tree_png;
            mem2 = NULL;
            size1 = tree_png_size; size2 = 0;
            break;
        case NEWTON:
            mem1 = newton1_png;
            mem2 = newton2_png;
            size1 = size2 = newton1_png_size;
            o->sprite3 = load_packed_bitmap(newton_asleep_png, newton_asleep_png_size);
            break;
        case GROUND:
            mem1 = ground_png;
            mem2 = NULL;
            size1 = size2 = ground_png_size;
            break;
    }

    // load_packed_bitmap will just return NULL if given mem = NULL :)
    o->sprite1 = load_packed_bitmap(mem1, size1);
    o->sprite2 = load_packed_bitmap(mem2, size2);
    if ((!o->sprite1 && mem1 != NULL ) || (!o->sprite2 && mem2 != NULL))
        die("couldn't load sprites for objects[%d]\n", object_n);
    o->width = al_get_bitmap_width(o->sprite1);
    o->height = al_get_bitmap_height(o->sprite1);
}

void load_resources(ALLEGRO_FONT **font, intro_resource_struct *intro_resources, object *objects)
{
    *font = load_packed_font(Arial_ttf, Arial_ttf_size, 36);
    if (!(*font))
        die("couldn't load font\n");
    intro_resources->font = load_packed_font(Arial_ttf, Arial_ttf_size, 20);
    intro_resources->instructions1 = load_packed_bitmap(instructions1_png, instructions1_png_size);
    intro_resources->instructions2 = load_packed_bitmap(instructions2_png, instructions2_png_size);

    enum object_ctr i;
    for (i = 0; i < LAST_MOVER; i++)
        init_sprite(&objects[i], i);
    // not handled above
    init_sprite(&objects[TREE], TREE);
    init_sprite(&objects[GROUND], GROUND);
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
    al_destroy_bitmap(objects[GROUND].sprite1);
    al_destroy_bitmap(objects[TREE].sprite1);

    al_destroy_bitmap(intro_resources->instructions2);
    al_destroy_bitmap(intro_resources->instructions1);

    al_destroy_font(font);
}
