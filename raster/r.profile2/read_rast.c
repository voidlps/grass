/*
 * Copyright (C) 2000 by the GRASS Development Team
 * Author: Bob Covill <bcovill@tekmap.ns.ca>
 * 
 * This Program is free software under the GPL (>=v2)
 * Read the file COPYING coming with GRASS for details
 *
 */

#include <grass/gis.h>
#include <grass/raster.h>
#include <grass/glocale.h>
#include "local_proto.h"

int read_rast2(double east, double north, double dist, int num_inputs, struct input_info *inputs, int coords,
               FILE *fp, char *null_string)
{
    char buffer[200];
    int fd = inputs[0].fd;
    int data_type = inputs[0].data_type;
    // Only when more than one input we need to test
    if (num_inputs > 1)
    {
        for (int i = 0; i < num_inputs; i++)
        {
            if (east >= inputs[i].cellhead.west && east <= inputs[i].cellhead.east && north >= inputs[i].cellhead.south && north <= inputs[i].cellhead.north)
            {
                G_debug(4, _("Try layer [%s]; E,N=%.2f,%.2f; extent N,S,E,W: %.2f,%.2f,%.2f,%.2f"), inputs[i].name, east, north, inputs[i].cellhead.north, inputs[i].cellhead.south, inputs[i].cellhead.east, inputs[i].cellhead.west);
                fd = inputs[i].fd;
                data_type = inputs[i].data_type;

                int result = read_rast(east, north, dist, fd, coords, data_type, buffer, NULL);
                if (result)
                {
                    G_debug(4, _("Use layer [%s]; E,N=%.2f,%.2f; extent N,S,E,W: %.2f,%.2f,%.2f,%.2f"), inputs[i].name, east, north, inputs[i].cellhead.north, inputs[i].cellhead.south, inputs[i].cellhead.east, inputs[i].cellhead.west);
                    fprintf(fp, "%s", buffer);
                    return 1;
                }
            }
        }
    }
    int result = read_rast(east, north, dist, fd, coords, data_type, buffer, null_string);
    fprintf(fp, "%s", buffer);

    return result ? 0 : 1;
}

int read_rast(double east, double north, double dist, int fd, int coords,
              RASTER_MAP_TYPE data_type, char *buffer, char *null_string)
{
    static DCELL *dcell;
    static int cur_row = -1;
    static CELL nullcell;
    static int nrows, ncols;
    static struct Cell_head window;
    int row, col;
    int outofbounds = FALSE;

    char *buffer_pointer = buffer;

    if (!dcell)
    {
        Rast_set_c_null_value(&nullcell, 1);
        dcell = Rast_allocate_d_buf();
        G_get_window(&window);
        nrows = window.rows;
        ncols = window.cols;
    }

    row = (window.north - north) / window.ns_res;
    col = (east - window.west) / window.ew_res;
    G_debug(4, "row=%d:%d  col=%d:%d", row, nrows, col, ncols);

    if ((row < 0) || (row >= nrows) || (col < 0) || (col >= ncols))
        outofbounds = TRUE;

    if (!outofbounds)
    {
        if (row != cur_row)
            Rast_get_d_row(fd, dcell, row);
        cur_row = row;
    }

    if (coords)
        buffer_pointer += sprintf(buffer_pointer, "%f %f", east, north);

    buffer_pointer += sprintf(buffer_pointer, " %f", dist);

    if (outofbounds || Rast_is_d_null_value(&dcell[col]))
    {
        if (null_string)
            buffer_pointer += sprintf(buffer_pointer, " %s", null_string);
        else
            return 0;
    }
    else
    {
        if (data_type == CELL_TYPE)
            buffer_pointer += sprintf(buffer_pointer, " %d", (int)dcell[col]);
        else
            buffer_pointer += sprintf(buffer_pointer, " %f", dcell[col]);
    }

    if (clr)
    {
        int red, green, blue;

        if (outofbounds)
            Rast_get_c_color(&nullcell, &red, &green, &blue, &colors);
        else
            Rast_get_d_color(&dcell[col], &red, &green, &blue,
                             &colors);

        buffer_pointer += sprintf(buffer_pointer, " %03d:%03d:%03d", red, green, blue);
    }

    buffer_pointer += sprintf(buffer_pointer, "\n");

    return buffer_pointer - buffer;
}
