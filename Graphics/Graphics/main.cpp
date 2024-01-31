
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "glut.h"
#include <algorithm>

using namespace std;

const int W = 600;
const int H = 600;
const int THICKNESS = 80; // Thickness of the line

typedef struct
{
	int red;
	int green;
	int blue;
} COLOR;

typedef struct {
	int row;
	int col;
} POINT2D;

unsigned char pixels[H][W][3] = { 0 }; // 3 saves RGB values
double offset = 0;
int oldX, oldY;

void init()
{
	int i, j;
	for (i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			pixels[i][j][0] = 0;
			pixels[i][j][1] = 0;
			pixels[i][j][2] = 0;
		}
	}
	glClearColor(0, 0.5, 0.8, 0);// color of window background
	glOrtho(-1, 1, -1, 1, -1, 1);
}

// x is columns, y is lines
void DrawLine(int startX, int startY, int endX, int endY)
{
	int row, col;
	double a, b;
	int tmp;

	if (endX != startX) {
		a = (endY - startY) / ((double)endX - startX);
		if (fabs(a) < 1)
		{
			b = startY - a * startX;

			if (startX > endX)
			{
				tmp = startX;
				startX = endX;
				endX = tmp;
			}

			for (col = startX; col <= endX; col++)
			{
				row = a * col + b;

				pixels[row][col][0] = 80;
				pixels[row][col][1] = 80;
				pixels[row][col][2] = 155;
			}
		}
		else // fabs(a) >1
		{
			a = 1 / a; // now slope is dx/dy
			b = startX - a * startY;

			if (startY > endY)
			{
				tmp = startY;
				startY = endY;
				endY = tmp;
			}

			for (row = startY; row <= endY; row++)
			{
				col = a * row + b;
				pixels[row][col][0] = 80;
				pixels[row][col][1] = 80;
				pixels[row][col][2] = 155;
			}
		}
	}
}

void Paint(int row, int col, COLOR c)
{
	pixels[row][col][0] = c.red;
	pixels[row][col][1] = c.green;
	pixels[row][col][2] = c.blue;
}

bool SameColor(int row, int col, COLOR c)
{
	return pixels[row][col][0] == c.red && pixels[row][col][1] == c.green &&
		pixels[row][col][2] == c.blue;
}


void DrawTriangle(int centerX, int centerY, int size) {
	// Calculate the height of the triangle
	double height = size * sqrt(3) / 2;

	// Calculate the vertices of the triangle
	int x1 = centerX - size / 2;
	int y1 = centerY - height / 3;
	int x2 = centerX + size / 2;
	int y2 = y1;
	int x3 = centerX;
	int y3 = centerY + 2 * height / 3;

	// Draw lines between the vertices
	DrawLine(x1, y1, x2, y2);
	DrawLine(x2, y2, x3, y3);
	DrawLine(x3, y3, x1, y1);
}


void DrawUpsideDownTriangle(int centerX, int centerY, int size) {
	// Calculate the height of the triangle
	double height = size * sqrt(3) / 2;

	// Calculate the vertices of the upside-down triangle
	int x1 = centerX - size / 2;
	int y1 = centerY + height / 3; // Invert the Y-coordinate
	int x2 = centerX + size / 2;
	int y2 = y1;
	int x3 = centerX;
	int y3 = centerY - 2 * height / 3; // Invert the Y-coordinate

	// Draw lines between the vertices
	DrawLine(x1, y1, x2, y2);
	DrawLine(x2, y2, x3, y3);
	DrawLine(x3, y3, x1, y1);
}

void DrawThickLine(int startX, int startY, int endX, int endY, int thickness) {
	for (int i = -thickness / 2; i <= thickness / 2; i++) {
		DrawLine(startX, startY + i, endX, endY + i);
	}
}




void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	// Draw the triangle in the middle of the screen
	int centerX = W / 2;
	int centerY = H / 2;
	int size = 100;
	DrawTriangle(centerX, centerY, size);

	// Draw the upside-down triangle in the middle of the screen
	DrawUpsideDownTriangle(centerX, centerY, size);
	glDrawPixels(W, H, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	glutSwapBuffers(); // show all
}

void DrawWavyLine(int startX, int startY, int endX, int endY, int thickness, double waveAmplitude, double waveFrequency) {
	for (int x = startX; x <= endX; x++) {
		// Calculate the sine wave offset for the current x position
		double waveOffset = sin(x * waveFrequency) * waveAmplitude;

		// Draw vertical lines for each x position with wave offset
		for (int yDelta = -thickness / 2; yDelta <= thickness / 2; yDelta++) {
			int y = startY + waveOffset + yDelta;
			if (y >= 0 && y < H) { // Ensure y is within the screen bounds
				pixels[y][x][0] = 0;   // red
				pixels[y][x][1] = 0;   // green
				pixels[y][x][2] = 255; // blue - Set the color to blue
			}
		}
	}
}



void idle()
{
	int i, j;
	double distance;
	offset -= 0.08;

	// 1. create basic background 
	unsigned char lightGrayValue = 155;
	for (i = 0; i < H; i++)
		for (j = 0; j < W; j++)
		{
			pixels[i][j][0] = lightGrayValue;   // red
			pixels[i][j][1] = lightGrayValue;   // green
			pixels[i][j][2] = lightGrayValue; // blue
		}

	// Draw a thick blue line at the top of the screen
	DrawThickLine(0, H - THICKNESS, W, H - THICKNESS, THICKNESS);
	// The y-coordinate is set to 'thickness' to start the line at the bottom
	DrawThickLine(0, THICKNESS, W, THICKNESS, THICKNESS);


	// 2. add color wave
	for (i = 0; i < H; i++)
		for (j = 0; j < W; j++)
		{
			distance = sqrt(pow(i - H / 2, 2) + pow(j + W / 2, 2)); // distance from (H/2,W/2)
			pixels[i][j][0] += sin(distance / 30.0 + offset) * 60;   // red
			pixels[i][j][1] += sin(distance / 30.0 + offset) * 60;   // green
			pixels[i][j][2] += sin(distance / 30.0 + offset) * 60;   // blue
		}
	glutPostRedisplay();
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(400, 100);
	glutCreateWindow("Israel flag with pixels");

	glutDisplayFunc(display);
	glutIdleFunc(idle);

	init();

	glutMainLoop();
}