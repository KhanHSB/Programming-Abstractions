/*
 * File: fractals.cpp
 * --------------------------
 * Name:
 * Section leader:
 * This file contains fractal problems for CS106B.
 */


#include "fractals.h"
#include <cmath>

using namespace std;

const int LEAF_COLOR = 0x2e8b57;   /* Color of all leaves of recursive tree (level 1) */
const int BRANCH_COLOR = 0x8b7765; /* Color of all branches of recursive tree (level >=2) */

/**
 * Draws a Sierpinski triangle of the specified size and order, placing its
 * top-left corner at position (x, y).
 *
 * This will be called by fractalgui.cpp.
 *
 * @param gw - The window in which to draw the Sierpinski triangle.
 * @param x - The x coordinate of the top-left corner of the triangle.
 * @param y - The y coordinate of the top-left corner of the triangle.
 * @param size - The length of one side of the triangle.
 * @param order - The order of the fractal.
 */
void drawSierpinskiTriangle(GWindow& gw, double x, double y, double size, int order)
// TODO: write this function
{
    if (x < 0)
        throw string("drawSierpinskiTriangle() error: negative x coordinate.");
    if (y < 0)
        throw string("drawSierpinskiTriangle() error: negative y coordinate.");
    if (size < 0)
        throw string("drawSierpinskiTriangle() error: negative size.");
    if (order < 0)
        throw string("drawSierpinskiTriangle() error: negative order.");

    if (order == 0)
        return;

    double h = size * sqrt(3) / 2;

    if (order == 1)
    {
        gw.drawLine(x, y, x + size, y);
        gw.drawLine(x, y, x + size / 2, y + h);
        gw.drawLine(x + size, y, x + size /2, y + h);
    }
    else
    {
        drawSierpinskiTriangle(gw, x, y, size / 2, order - 1);
        drawSierpinskiTriangle(gw, x + size / 2, y, size / 2, order - 1);
        drawSierpinskiTriangle(gw, x + size / 4, y + h / 2, size / 2, order - 1);
    }
}

/**
 * Recursive helper function to draw subtree
 *
 * @param gw - The window in which to draw the recursive tree.
 * @param p0 - The coordinate of previous line.
 * @param r - The size of line.
 * @param angle - The angle for drawing line
 * @param order - The order of the fractal.
 */
void drawTreeHelper(GWindow& gw, GPoint p0, double r, double angle, int order)
{
    if (order < 2)
        return;

    for (int i = 0; i < 7; ++i)
    {
        if (order == 2)
            gw.setColor(LEAF_COLOR);
        else
            gw.setColor(BRANCH_COLOR);

        double theta = angle - 45 + 15 * i;
        GPoint p = gw.drawPolarLine(p0, r, theta);

        drawTreeHelper(gw, p, r/2, theta, order - 1);
    }
}

/**
 * Draws a recursive tree fractal image of the specified size and order,
 * placing the bounding box's top-left corner at position (x,y).
 *
 * This will be called by fractalgui.cpp.
 *
 * @param gw - The window in which to draw the recursive tree.
 * @param x - The x coordinate of the top-left corner of the bounding box.
 * @param y - The y coordinate of the top-left corner of the bounding box.
 * @param size - The length of one side of the bounding box.
 * @param order - The order of the fractal.
 */
void drawTree(GWindow& gw, double x, double y, double size, int order)
// TODO: write this function
{
    if (x < 0)
        throw string("drawTree() error: negative x coordinate.");
    if (y < 0)
        throw string("drawTree() error: negative y coordinate.");
    if (size < 0)
        throw string("drawTree() error: negative size.");
    if (order < 0)
        throw string("drawTree() error: negative order.");

    if (order == 0)
        return;

    if (order == 1)
        gw.setColor(LEAF_COLOR);
    else
        gw.setColor(BRANCH_COLOR);

    GPoint p0(x + size / 2, y + size);
    GPoint p = gw.drawPolarLine(p0, size / 2, 90);

    drawTreeHelper(gw, p, size / 4, 90, order);

}



/**
 * Draws a Mandelbrot Set in the graphical window give, with maxIterations
 * (size in GUI) and in a given color (zero for palette)
 *
 * This will be called by fractalgui.cpp.
 *
 * @param gw - The window in which to draw the Mandelbrot set.
 * @param minX - left-most column of grid
 * @param incX - increment value of columns of grid
 * @param minY - top-most row of grid
 * @param incY - increment value of rows of grid
 * @param maxIterations - The maximum number of iterations to run recursive step
 * @param color - The color of the fractal; zero if palette is to be used
 */
void mandelbrotSet(GWindow& gw, double minX, double incX,
                   double minY, double incY, int maxIterations, int color)
{
    // Creates palette of colors
    // To use palette:
    // pixels[r][c] = palette[numIterations % palette.size()];
    Vector<int> palette = setPalette();

    int width = gw.getCanvasWidth();
    int height = gw.getCanvasHeight();
    GBufferedImage image(width,height,0xffffff);
    gw.add(&image);
    Grid<int> pixels = image.toGrid(); // Convert image to grid


    for (int row = 0; row < pixels.numRows(); ++row)
    {
        for (int col = 0; col < pixels.numCols(); ++col)
        {
            Complex c(minX + col * incX, minY + row * incY);
            int numIterations = mandelbrotSetIterations(c, maxIterations);
            if (color)
            {
                if (numIterations == maxIterations)
                    pixels[row][col] = color;
            }
            else
                pixels[row][col] = palette[numIterations % palette.size()];
        }
    }

    image.fromGrid(pixels); // Converts and puts the grid back into the image
}




/**
 * Runs the Mandelbrot Set recursive formula on complex number c a maximum
 * of maxIterations times.
 *
 * This will be called by you. Think about how this fits with the other two functions.
 *
 * @param c - Complex number to use for recursive formula.
 * @param maxIterations - The maximum number of iterations to run recursive step
 * @return number of iterations needed to determine if c is unbounded
 */
int mandelbrotSetIterations(Complex c, int maxIterations)
// TODO: Write this function
{
    int remainingIterations = mandelbrotSetIterations(Complex(), c, maxIterations);

    return maxIterations - remainingIterations;
}
/**
 * An iteration of the Mandelbrot Set recursive formula with given values z and c, to
 * run for a maximum of maxIterations.
 *
 * This will be called by you. Think about how this fits with the other two functions.
 *
 * @param z - Complex number for a given number of iterations
 * @param c - Complex number to use for recursive formula.
 * @param remainingIterations - The remaining number of iterations to run recursive step
 * @return number of iterations needed to determine if c is unbounded
 */
int mandelbrotSetIterations(Complex z, Complex c, int remainingIterations)
    // TODO: write this function
{
    if (remainingIterations == 0)
        return 0;

    if (z.abs() > 4.0 )
        return remainingIterations;

    return mandelbrotSetIterations(z * z + c, c, remainingIterations - 1);
}

// Helper function to set the palette
Vector<int> setPalette() {
    Vector<int> colors;

    // Feel free to replace with any palette.
    // You can find palettes at:
    // http://www.colourlovers.com/palettes

    // Example palettes:
    // http://www.colourlovers.com/palette/4480793/in_the_middle
    // string colorSt = "#A0B965,#908F84,#BF3C43,#9D8E70,#C9BE91,#A0B965,#908F84,#BF3C43";

    // http://www.colourlovers.com/palette/4480786/Classy_Glass
    // string colorSt = "#9AB0E9,#C47624,#25269A,#B72202,#00002E,#9AB0E9,#C47624,#25269A";

    // The following is the "Hope" palette:
    // http://www.colourlovers.com/palette/524048/Hope
    string colorSt =  "#04182B,#5A8C8C,#F2D99D,#738585,#AB1111,#04182B,#5A8C8C,#F2D99D";
    Vector<string>colorsStrVec = stringSplit(colorSt,",");
    for (string color : colorsStrVec) {
        colors.add(convertColorToRGB(trim(color)));
    }
    return colors;
}
