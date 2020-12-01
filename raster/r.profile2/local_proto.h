#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <grass/gis.h>
#include <grass/raster.h>


struct input_info
{
    const char *name;
    int fd;
    struct Cell_head cellhead;
    RASTER_MAP_TYPE data_type;
};


/* main.c */
int do_profile(double, double, double, double, int, double, int num_inputs, struct input_info *,
	       FILE *, char *, const char *, double);

/* read_rast.c */
int read_rast(double, double, double, int, int, RASTER_MAP_TYPE, FILE *,
	      char *);
int read_rast2(double, double, double, int num_inputs, struct input_info *, int, FILE *,
	      char *);

/* input.c */
int input(char *, char *, char *, char *, char *, FILE *);

extern int clr;
extern struct Colors colors;
