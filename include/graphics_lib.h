/* graphics_lib.h defines functions which hide
   complexities of allegro and allow
   the C programming labscripts to be used
   almost as is. This is done so that many of the complexities
   of C are hidden from the beginner.
   graphics_lib.h does not update the display after each drawing
   function, instead it expects the programmer to do that
   (using update_display).
   This will allow the ability to simulate multiple events.
   Author: Julian Miller, Department of Electronics
   University of York, Jan 2013.
*/


#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>



typedef struct colour_info
{
  unsigned r;
  unsigned g;
  unsigned b;
} COLOUR;

#define  PI  3.141592653589793

/* globals */
COLOUR        RGB, BACKGROUND_RGB;
ALLEGRO_DISPLAY     *DISPLAY;
int                 GLOBAL_X, GLOBAL_Y;
ALLEGRO_FONT        *GLOBAL_FONT; /*global font pointer */
ALLEGRO_PATH        *path;
ALLEGRO_EVENT_QUEUE *event_queue;
ALLEGRO_EVENT       event;
int                 XMOUSE, YMOUSE;

#define BLACK           0
#define BLUE            1
#define GREEN           2
#define CYAN            3
#define RED             4
#define MAGENTA         5
#define BROWN           6
#define LIGHTGRAY       7
#define DARKGRAY        8
#define LIGHTBLUE       9
#define LIGHTGREEN      10
#define LIGHTCYAN       11
#define LIGHTRED        12
#define LIGHTMAGENTA    13
#define YELLOW          14
#define WHITE           15


/* prototypes */
int rand_number(int lower_range, int upper_range);

void pausefor(int time_in_milliseconds);

void initwindow(int x, int y);

void initfont(void);

void closegraph(void);

COLOUR make_RGB(unsigned colour);

void setcolor(unsigned x);

void filled_circle(int x, int y, int radius, unsigned fillcolour);

void circle(int x, int y, int radius, int thickness);


void filled_rectangle(int upper_left_x, int upper_left_y,
               int lower_right_x, int lower_right_y,
               unsigned fillcolour);

void rectangle(int upper_left_x, int upper_left_y,
               int lower_right_x, int lower_right_y,
               int thickness);

void filled_ellipse(int centre_x, int centre_y,
               int radius_x, int radius_y,
               unsigned fillcolour);

void ellipse(int centre_x, int centre_y,
               int radius_x, int radius_y,
               int thickness);

void arc(int centre_x, int centre_y, int radius,
         double angle_start, double angle_end, int thickness);

void line(int xstart, int ystart, int xend, int yend, int thickness);

void lineto(int x, int y, int thickness);


void setbkcolor(unsigned colour);

void cleardevice(void);

void moveto(int x, int y);

void outtextxy(int x, int y, char text[]);

void create_event_queue(void);

void reg_display_events(void);

void reg_mouse_events(void);

void hide_mouse_cursor(void);

int check_if_event(void);

void wait_for_event(void);

int event_close_display(void);

int event_mouse_position_changed(void);

void get_mouse_coordinates(void);

int event_mouse_button_down(void);

int event_mouse_left_button_down(void);

int event_mouse_right_button_down(void);

void initmouse(void);

void closemouse(void);

int rand_number(int lower_range, int upper_range)
{
    int x;

    if (upper_range < lower_range)
    {
        x = upper_range;
        upper_range = lower_range;
        lower_range = x;
    }
    return lower_range + rand() %(upper_range -lower_range +1);

}


void pausefor(int time_in_milliseconds)
{

    al_rest(time_in_milliseconds/1000.0);

}


/* wrapper function defining initwindow in terms of
   allegro routines */
void initwindow(int x, int y)
{
    /* These variables are globals defined
       in graphics_lib.h  */
    GLOBAL_X = 0;
    GLOBAL_Y = 0;
    event_queue= NULL;
    DISPLAY = NULL;

    if (!al_init())
    {
        printf("\nIn initwindow: al_init failed\n");
        exit(1);
    }


    al_init_image_addon();

    /* allow the use of allegro drawing primitives */
    al_init_primitives_addon();

    DISPLAY = al_create_display(x, y);

    if (!DISPLAY)
       exit(1);

    setcolor(WHITE);  /* make initial pen colour white */

    setbkcolor(BLACK); /* make initial background colour black */

}

void initfont(void)
{
    /* fonts will only work from dos prompt */
    char pathtofont[300]="./data/fixed_font.tga";

    /* allow the use fo allegro fonts */
    al_init_font_addon();

   /* load a font. This uses windows paths, so would have to
    be changed in Unix and possibly MacOS */

    GLOBAL_FONT = al_load_font(pathtofont, 0, 0);

    if (!GLOBAL_FONT)
    {
        printf("\nFailed to load fixed_font.tga\n");
        printf("\nYou cannot run programs with outtextxy from the Code::Blocks IDE");
        printf("\nRun them from the Command Prompt\n");
        exit(0);
    }
}

/* wraper for allegro close of display window */
void closegraph(void)
{
    al_destroy_display(DISPLAY);
}

/* given an integer colour value returns a RGB structure colour */
COLOUR make_RGB(unsigned colour)
{
    COLOUR result;

    switch(colour)
    {
        case BLACK: result.r = 0; result.g = 0; result.b = 0;
        break;
        case BLUE: result.r = 0; result.g = 0; result.b = 255;
        break;
        case GREEN: result.r = 0; result.g = 255; result.b = 0;
        break;
        case CYAN: result.r = 0; result.g = 255; result.b = 255;
        break;
        case RED: result.r = 255; result.g = 0; result.b = 0;
        break;
        case MAGENTA: result.r = 255; result.g = 0; result.b = 255;
        break;
        case BROWN: result.r = 139; result.g = 69; result.b = 19;
        break;
        case LIGHTGRAY: result.r = 200; result.g = 200; result.b = 200;
        break;
        case DARKGRAY: result.r = 100; result.g = 100; result.b = 100;
        break;
        case LIGHTBLUE: result.r = 200; result.g = 200; result.b = 255;
        break;
        case LIGHTGREEN: result.r = 200; result.g = 255; result.b = 200;
        break;
        case LIGHTCYAN: result.r = 200; result.g = 255; result.b = 255;
        break;
        case LIGHTRED: result.r = 255; result.g = 200; result.b = 200;
        break;
        case LIGHTMAGENTA: result.r = 255; result.g = 200; result.b = 200;
        break;
        case YELLOW: result.r = 255; result.g = 255; result.b = 0;
        break;
        case WHITE: result.r = 255; result.g = 255; result.b = 255;
    }
    return result;
}

/* It writes the RGB value to a global that can be used by
   graphics drawing wrapper functions */
void setcolor(unsigned x)
{
    RGB = make_RGB(x);
}

/* this moves the screen buffer to the display */
void update_display(void)
{
    /* allegro routines write to a screen buffer
    and this has to be moved to the screen buffer
    to actual display so we can see it */
    al_flip_display();
}

/* wrapper for a filled circle function */
void filled_circle(int x, int y, int radius, unsigned fillcolour)
{
    COLOUR fill;

    GLOBAL_X = x;
    GLOBAL_Y = y;


    fill = make_RGB(fillcolour);
    al_draw_filled_circle(x, y , radius, al_map_rgb(fill.r,fill.g,fill.b));
}

/* wrapper for the circle function */
void circle(int x, int y, int radius, int thickness)
{
    GLOBAL_X = x;
    GLOBAL_Y = y;

    al_draw_circle(x, y, radius, al_map_rgb(RGB.r,RGB.g,RGB.b),thickness);

}

/* wrapper for the rectangle function */
void filled_rectangle(int upper_left_x, int upper_left_y,
               int lower_right_x, int lower_right_y,
               unsigned fillcolour)
{
    COLOUR fill;

    GLOBAL_X = lower_right_x;
    GLOBAL_Y = lower_right_y;


    fill = make_RGB(fillcolour);

    al_draw_filled_rectangle(upper_left_x, upper_left_y,
                          lower_right_x, lower_right_y,
                          al_map_rgb(fill.r,fill.g,fill.b));
}

/* wrapper for the rectangle function */
void rectangle(int upper_left_x, int upper_left_y,
               int lower_right_x, int lower_right_y,
               int thickness)
{

    GLOBAL_X = lower_right_x;
    GLOBAL_Y = lower_right_y;


    al_draw_rectangle(upper_left_x, upper_left_y,
                    lower_right_x, lower_right_y,
                    al_map_rgb(RGB.r,RGB.g,RGB.b),thickness);
}

/* wrapper for the ellipse  function */
void filled_ellipse(int centre_x, int centre_y,
               int radius_x, int radius_y,
               unsigned fillcolour)
{
    COLOUR fill;

    GLOBAL_X = centre_x;
    GLOBAL_Y = centre_y;

    fill = make_RGB(fillcolour);

    al_draw_filled_ellipse(centre_x, centre_y,
                          radius_x, radius_y,
                          al_map_rgb(fill.r,fill.g,fill.b));
}

/* wrapper for the ellipse  function */
void ellipse(int centre_x, int centre_y,
               int radius_x, int radius_y,
               int thickness)
{

    GLOBAL_X = centre_x;
    GLOBAL_Y = centre_y;

    al_draw_ellipse(centre_x, centre_y,
                    radius_x, radius_y,
                    al_map_rgb(RGB.r,RGB.g,RGB.b),thickness);
}

/* wrapper for the ellipse  function
 angles in degrees*/
void arc(int centre_x, int centre_y, int radius,
         double angle_start, double angle_end, int thickness)
{
    GLOBAL_X = centre_x;
    GLOBAL_Y = centre_y;

    /* convert to degrees */
    angle_start = 2.0*PI*angle_start/360.0;
    angle_end   = 2.0*PI*angle_end/360.0;

    al_draw_arc(centre_x, centre_y, radius, angle_start, angle_end,
                al_map_rgb(RGB.r,RGB.g,RGB.b),thickness);
}



/* wrapper for setbkcolor */
/* sets the global background colour to the chosen colour */
void setbkcolor(unsigned colour)
{
    BACKGROUND_RGB = make_RGB(colour);
}

/* wrapper for cleardevice */
/* fills current graphics window with the current
background colour and sets the global drawing coordinates
to 0 , 0 */
void cleardevice(void)
{
    al_clear_to_color(al_map_rgb(BACKGROUND_RGB.r,BACKGROUND_RGB.g,BACKGROUND_RGB.b));
    GLOBAL_X = 0;
    GLOBAL_Y = 0;
}

/* wrapper for the line function: draw line in current
  global colour (RGB) from one point to another  */
void line(int xstart, int ystart, int xend, int yend, int thickness)
{

    GLOBAL_X = xend;
    GLOBAL_Y = yend;

    al_draw_line(xstart, ystart, xend, yend, al_map_rgb(RGB.r,RGB.g,RGB.b),thickness);
}

/* wrapper for lineto function
   same as line except it works from global start position
*/
void lineto(int x, int y, int thickness)
{

    al_draw_line(GLOBAL_X, GLOBAL_Y, x, y, al_map_rgb(RGB.r,RGB.g,RGB.b),thickness);

    GLOBAL_X = x;
    GLOBAL_Y = y;
}

/* wrapper for moveto.
   Moves the global drawing position to the user
   choice */
void moveto(int x, int y)
{
     GLOBAL_X = x;
     GLOBAL_Y = y;
}

/* wrapper for outtextxy that writes a string
  in current pen colour to position x, y */
  /* To use this initfont() has to be called first */
void outtextxy(int x, int y, char text[])
{
    al_draw_textf(GLOBAL_FONT, al_map_rgb(RGB.r,RGB.g,RGB.b), x, y, ALLEGRO_ALIGN_LEFT, text);
}


/* allows the detection of events */
void create_event_queue(void)
{
    event_queue = al_create_event_queue();
}

/* register a display event source */
void reg_display_events(void)
{
    if (event_queue != NULL)
        al_register_event_source(event_queue, al_get_display_event_source(DISPLAY));
    else
    {
        printf("\nYou need to create an event queue with function: create_event_queue\n");
        exit(0);
    }
}

/* register a mouse event source */
void reg_mouse_events(void)
{
    if (event_queue != NULL)
        al_register_event_source(event_queue, al_get_mouse_event_source());
    else
    {
        printf("\nYou need to create an event queue with function: create_event_queue\n");
        exit(0);
    }
}

/* hide the mouse cursor */
void hide_mouse_cursor(void)
{
    al_hide_mouse_cursor(DISPLAY);
}

/* if an event return 1 else return 0 */
int check_if_event(void)
{
    if (al_is_event_queue_empty(event_queue))
        return 0;
    else
        return 1;
}

/* wait until the event queue is non-empty. In other words wait
   until the user makes an event (moves mouse, clicks a mouse button
or close the display */
void wait_for_event(void)
{
    al_wait_for_event(event_queue, &event);
}

/* close the display if the user initiates it */
int event_close_display(void)
{
    if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
	   return 1;
    else
        return 0;
}

/* detect if the mosue position has changed */
int event_mouse_position_changed(void)
{
    if (event.type == ALLEGRO_EVENT_MOUSE_AXES)
        return 1;
    else
        return 0;
}

/* get the mouse coordinates */
void get_mouse_coordinates(void)
{
    if (event_mouse_position_changed())
    {
        XMOUSE = event.mouse.x;
        YMOUSE = event.mouse.y;
    }
}

/* detect whether a mouse button is down */
int event_mouse_button_down(void)
{
    if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        return 1;
    else
        return 0;
}

/* detect if left mouse button is down */
int event_mouse_left_button_down(void)
{
    if (event.mouse.button & 1)
        return 1;
    else
        return 0;
}

/* detect if right mouse button down */
int event_mouse_right_button_down(void)
{
    if (event.mouse.button & 2)
        return 1;
    else
        return 0;
}

/* install the mouse */
void initmouse(void)
{

    if (!al_install_mouse())
    {
        printf("\nIn initmouse: al_install_mouse failed\n");
        exit(1);
    }

}

/* uninstall the mouse */
void closemouse(void)
{
    al_uninstall_mouse();
}

