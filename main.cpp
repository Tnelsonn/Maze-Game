#include <math.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <GL/glut.h> // assuming you're using GLUT for OpenGL
#include "libim/im_color.h"

using namespace std;




#define ROTATE 1
#define TRANSLATE 2
#define MOVE 3

int xangle = 10;
int yangle = 10;
int zangle = 10;

int xpos = 0;
int ypos = 0;
int zpos = 0;

//default mode set to MOVE
int mode = MOVE;

int row = 0;
int col = 0;
int srow = 0;
int scol = 0;
char maze[50][50];

int numGold = 3;
int numGems = 3;
int goldCount = 0;
int gemCount = 0;

float gridSize = 3.0 / 10.0; 

int xdim, ydim;

unsigned char* brick;
unsigned char* grass;
unsigned char* rock;
unsigned char* gravel;
unsigned char* wood;
unsigned char* gold;
unsigned char* gems;
unsigned char* yellow;

bool useTexture1 = true;


void readMaze()
{
    ifstream file("maze.txt");
    if(!file.is_open())
    {
        cerr<< "Error opening file"<<endl;
        exit(1);
    }
    
    file >> row >> col;
    file >> srow >> scol;
    file.ignore(256, '\n');
    for(int i = 0; i < row; i++)
    {
        string line;
        getline(file,line);
        for(int j = 0; j < col; j++)
        {
            maze[i][j] = line[j];
        }
    }
    file.close();
}

//allows the ability to switch floor texture
unsigned char* getCurrentTexture()
{
    return useTexture1 ? grass : gravel;
}

// void printMaze() {
//     for (int i = 0; i <= row; ++i) {
//         for (int j = 0; j < col; ++j) {
//             cout << maze[i][j];
//         }
//         cout<<endl;
//     }
// }

unsigned char* getTex(char tex)
{
    //simple if branch to check each texture for drawWalls
    if(tex == 'r')
    {
        return rock;
    }
    else if(tex == 'b')
    {
        return brick;
    }
    else if(tex == 'w')
    {
        return wood;
    }
    else if(tex == 'g')
    {
        return gold;
    }
    else if(tex == 'G')
    {
        return gems;
    }
    else
    {
        return rock;
    }
}

void bg()
{
    //define the floor and map the texture to the size defined
    //making the map 3.0 units across to get a size thats 75% of the screen
    glEnable(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xdim, ydim, 0, GL_RGB, GL_UNSIGNED_BYTE, getCurrentTexture());
    glBegin(GL_QUADS);  
    
        glTexCoord2f(0.0f,0.0f);
        glVertex2f(-1.5, -1.5);

        glTexCoord2f(1.0f,0.0f);
        glVertex2f(1.5f,-1.5f);

        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(1.5f,1.5f);

        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(-1.5f,1.5f);

    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void makeBlock(float x, float y, float size) 
{
    glBegin(GL_QUADS);
        // Front
        glTexCoord2f(0.0f, 0.0f); 
        glVertex3f(x, y, size);
        glTexCoord2f(1.0f, 0.0f); 
        glVertex3f(x + size, y, size);
        glTexCoord2f(1.0f, 1.0f); 
        glVertex3f(x + size, y + size, size);
        glTexCoord2f(0.0f, 1.0f); 
        glVertex3f(x, y + size, size);
        // Back
        glTexCoord2f(0.0f, 0.0f); 
        glVertex3f(x, y, 0.0);
        glTexCoord2f(1.0f, 0.0f); 
        glVertex3f(x + size, y, 0.0);
        glTexCoord2f(1.0f, 1.0f); 
        glVertex3f(x + size, y + size, 0.0);
        glTexCoord2f(0.0f, 1.0f); 
        glVertex3f(x, y + size, 0.0);
        // Top
        glTexCoord2f(0.0f, 0.0f); 
        glVertex3f(x, y + size, size);
        glTexCoord2f(1.0f, 0.0f); 
        glVertex3f(x + size, y + size, size);
        glTexCoord2f(1.0f, 1.0f); 
        glVertex3f(x + size, y + size, 0.0);
        glTexCoord2f(0.0f, 1.0f); 
        glVertex3f(x, y + size, 0.0);
        // Bottom
        glTexCoord2f(0.0f, 0.0f); 
        glVertex3f(x, y, size);
        glTexCoord2f(1.0f, 0.0f); 
        glVertex3f(x + size, y, size);
        glTexCoord2f(1.0f, 1.0f); 
        glVertex3f(x + size, y, 0.0);
        glTexCoord2f(0.0f, 1.0f); 
        glVertex3f(x, y, 0.0);
        // Right
        glTexCoord2f(0.0f, 0.0f); 
        glVertex3f(x + size, y, size);
        glTexCoord2f(1.0f, 0.0f); 
        glVertex3f(x + size, y + size, size);
        glTexCoord2f(1.0f, 1.0f); 
        glVertex3f(x + size, y + size, 0.0);
        glTexCoord2f(0.0f, 1.0f); 
        glVertex3f(x + size, y, 0.0);
        // Left
        glTexCoord2f(0.0f, 0.0f); 
        glVertex3f(x, y, size);
        glTexCoord2f(1.0f, 0.0f); 
        glVertex3f(x, y + size, size);
        glTexCoord2f(1.0f, 1.0f); 
        glVertex3f(x, y + size, 0.0);
        glTexCoord2f(0.0f, 1.0f); 
        glVertex3f(x, y, 0.0);
    glEnd();
}

void makeWalls()
{
    glEnable(GL_TEXTURE_2D);
    //row column size is 10x10 so that means that each block must be sized to .3 or 1/10 of the floor.
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            float x = -1.5 + j * gridSize;
            float y = -1.5 + i * gridSize;
            if (maze[i][j] != ' ')
            {
                unsigned char* texture = getTex(maze[i][j]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xdim, ydim, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
                //checking if the current index is g or G and adjusting the size accordingly
                if(maze[i][j] == 'g' || maze[i][j] == 'G')
                {
                    //adjust where to draw based on the current cell to position it in the center
                    float cellCenterX = -1.5 + j * gridSize + gridSize / 2.0;
                    float cellCenterY = -1.5 + i * gridSize + gridSize / 2.0;
                    float tS = gridSize / 2.0;
                    float tX = cellCenterX - tS / 2.0;
                    float tY = cellCenterY - tS / 2.0;
                    makeBlock(tX, tY, tS);
                } 
                else
                {
                    //draws the regular maze Block when maze index is not g or G
                    makeBlock(x, y, gridSize);
                } 
            }
        }
    }
}

void cubePlayer()
{
    // Draw the player block at the center of the grid cell
    float gridCenterX = -1.5 + scol * gridSize + gridSize / 2.0;
    float gridCenterY = -1.5 + srow * gridSize + gridSize / 2.0;
    glEnable(GL_TEXTURE_2D);
    float playerSize = gridSize / 2.0; 
    float playerPosX = gridCenterX - playerSize / 2.0;
    float playerPosY = gridCenterY - playerSize / 2.0;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xdim, ydim, 0, GL_RGB, GL_UNSIGNED_BYTE, yellow);
    makeBlock(playerPosX, playerPosY, playerSize);
}

void addTreasures()
{
    int i = 0;
    int j = 0;
    //while loop to ensure numGold treasures are placed
    while (i < numGold)
    {
        //generate random r and c index
        int r = rand() % row; 
        int c = rand() % col;
        if (maze[r][c] == ' ')
        {
            maze[r][c] = 'g';
            i++; 
        }
    }

    // while loop to ensure numGems treasures are placed
    while(j < numGems)
    {
        int r = rand() % row; // Generate a random row index
        int c = rand() % col; // Generate a random column index
        if (maze[r][c] == ' ')
        {
            maze[r][c] = 'G';
            j++;
        }
    }
}


void init_texture(char *name, unsigned char *&texture, int &xdim, int &ydim)
{
    // Read jpg image
    im_color image;
    image.ReadJpg(name);
    printf("Reading image %s\n", name);
    xdim = 1; while (xdim < image.R.Xdim) xdim*=2; xdim /=2;
    ydim = 1; while (ydim < image.R.Ydim) ydim*=2; ydim /=2;
    image.Interpolate(xdim, ydim);
    printf("Interpolating to %d by %d\n", xdim, ydim);

    // Copy image into texture array
    texture = (unsigned char *)malloc((unsigned int)(xdim*ydim*3));
    int index = 0;
    for (int y = 0; y < ydim; y++)
    {
        for (int x = 0; x < xdim; x++)
        {
            texture[index++] = (unsigned char)(image.R.Data2D[y][x]);
            texture[index++] = (unsigned char)(image.G.Data2D[y][x]);
            texture[index++] = (unsigned char)(image.B.Data2D[y][x]);
        }
    }
}

// Function to initialize OpenGL
void init()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //4 units clipping plane so to get 75% I need to make the map 3 units
    glOrtho(-2.0, 2.0, -2.0, 2.0, -2.0, 2.0);
    glEnable(GL_DEPTH_TEST);

    // Init texture
    init_texture((char *)"textures/brick.jpg", brick, xdim, ydim);
    init_texture((char *)"textures/wood.jpg", wood, xdim, ydim);
    init_texture((char *)"textures/rock.jpg", rock, xdim, ydim);
    init_texture((char *)"textures/gravel.jpg", gravel, xdim, ydim);
    init_texture((char *)"textures/grass.jpg", grass, xdim, ydim);
    init_texture((char *)"textures/yellow.jpg", yellow, xdim, ydim);
    init_texture((char *)"textures/gold.jpg", gold, xdim, ydim);
    init_texture((char *)"textures/gems.jpg", gems, xdim, ydim);
    glEnable(GL_TEXTURE_2D);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void checkWinCondition()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-2.0, 2.0, -2.0, 2.0); // Set up 2D orthographic projection

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    
    glColor3f(1.0, 1.0, 1.0); 
    glRasterPos2f(-1.9, -1.9); 
    string text = "Gold collected: " + to_string(goldCount) + " Gems collected: " + to_string(gemCount);
    for (char c : text)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c); 
    }

    if(goldCount == numGold && gemCount == numGems)
    {
        glRasterPos2f(-0.8,1.8);
        string wintext = "Congrats! You Win";
        for (char c : wintext)
        {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c); 
        } 
    }

    // Restore the matrix modes
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// Function to display the maze
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glTranslatef(xpos / 500.0, ypos / 500.0, zpos / 500.0);
    
    glRotatef(xangle, 1.0, 0.0, 0.0);
    glRotatef(yangle, 0.0, 1.0, 0.0);
    glRotatef(zangle, 0.0, 0.0, 1.0);

    //draw here
    bg();
    makeWalls();
    cubePlayer();
    
    checkWinCondition();
    
    glFlush();
}

void movePlayerUp()
{
    if (srow < row - 1 && ((maze[srow + 1][scol] == ' ') || (maze[srow + 1][scol] == 'g') || (maze[srow + 1][scol] == 'G')))
    {
        srow++; // Move the player up if the cell above is empty
        if(maze[srow][scol] == 'g')
        {
            goldCount++;
            maze[srow][scol] = ' ';
        }
        else if(maze[srow][scol] == 'G')
        {
            gemCount++;
            maze[srow][scol] = ' ';
        }
        glutPostRedisplay(); 
    }
}

// Function to move the player down
void movePlayerDown()
{
    if (srow > 0 && ((maze[srow - 1][scol] == ' ') || (maze[srow - 1][scol] == 'g') || (maze[srow - 1][scol] == 'G')))
    {
        srow--; // Move the player down if the cell below is empty
        if(maze[srow][scol] == 'g')
        {
            goldCount++;
            maze[srow][scol] = ' ';
        }
        else if(maze[srow][scol] == 'G')
        {
            gemCount++;
            maze[srow][scol] = ' ';
        }
        glutPostRedisplay(); 
    } 
}


void movePlayerLeft()
{
    if (scol > 0 && ((maze[srow][scol - 1] == ' ') || (maze[srow][scol - 1] == 'g') || (maze[srow][scol - 1] == 'G')))
    {
        scol--; 
        if(maze[srow][scol] == 'g')
        {
            goldCount++;
            maze[srow][scol] = ' ';
        }
        else if(maze[srow][scol] == 'G')
        {
            gemCount++;
            maze[srow][scol] = ' ';
        }
        glutPostRedisplay(); 
    } 
}


void movePlayerRight()
{
    if (scol < col - 1 && ((maze[srow][scol + 1] == ' ') || (maze[srow][scol + 1] == 'g') || (maze[srow][scol + 1] == 'G')))
    {
        scol++; 
        if(maze[srow][scol] == 'g')
        {
            goldCount++;
            maze[srow][scol] = ' ';
        }
        else if(maze[srow][scol] == 'G')
        {
            gemCount++;
            maze[srow][scol] = ' ';
        }
        glutPostRedisplay();
    } 
}

// Function to handle keyboard input for rotating the maze
void keyboard(unsigned char key, int x, int y)
{
    // Determine if we are in ROTATE or TRANSLATE mode
   if ((key == 'r') || (key == 'R'))
   {
        printf("Type x y z to decrease or X Y Z to increase ROTATION angles.\n");
        mode = ROTATE;
   }
   else if ((key == 't') || (key == 'T'))
   {
        printf("Type x y z to decrease or X Y Z to increase TRANSLATION distance.\n");
        mode = TRANSLATE;
   }
   else if((key == 'm') || (key == 'M'))
   {
        printf("Use w a s d to move the player cube.");
        mode = MOVE;
   }

   if ((key == 'f') || (key == 'F'))
   {
        useTexture1 = !useTexture1;
        glutPostRedisplay();
   }

   // Handle ROTATE
   if (mode == ROTATE)
   {
      if (key == 'x')
	 xangle -= 5;
      else if (key == 'y')
	 yangle -= 5;
      else if (key == 'z')
	 zangle -= 5;
      else if (key == 'X')
	 xangle += 5;
      else if (key == 'Y')
	 yangle += 5;
      else if (key == 'Z')
	 zangle += 5;
      glutPostRedisplay();
   }

   // Handle TRANSLATE
   if (mode == TRANSLATE)
   {
      if (key == 'x')
	 xpos -= 5;
      else if (key == 'y')
	 ypos -= 5;
      else if (key == 'z')
	 zpos -= 5;
      else if (key == 'X')
	 xpos += 5;
      else if (key == 'Y')
	 ypos += 5;
      else if (key == 'Z')
	 zpos += 5;
      glutPostRedisplay();
   }

    if(mode == MOVE)
    {
        if(key == 'w')
            movePlayerUp();
        else if (key == 's')
            movePlayerDown();
        else if (key == 'a')
            movePlayerLeft();
        else if (key == 'd')
            movePlayerRight();

        glutPostRedisplay();
    }

    

}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(250, 250);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
    glutCreateWindow("Maze Game");
    
    srand(time(nullptr));
    
    readMaze();
    addTreasures();
    
    

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    
    init();
    printf("Keyboard commands:\n");
    printf("   'f' or 'F' - switch floor texture\n");
    printf("   't' or 'T' - go to translate mode\n");
    printf("   'r' or 'R' - go to rotate mode\n");
    printf("   'm' or 'M' - go to move mode\n");
    printf("   'x' - rotate or translate on x-axis -5\n");
    printf("   'X' - rotate or translate on x-axis +5\n");
    printf("   'y' - rotate or translate on y-axis -5\n");
    printf("   'Y' - rotate or translate on y-axis +5\n");
    glutMainLoop();

    return 0;
}