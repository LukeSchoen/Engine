#include "biomorphs.h"
#include "Window.h"
#include "Controls.h"
#include <math.h>
#include "Maths.h"

int Rand(int low, int high) { return (rand() % (high - low + 1)) + low; }
float NormalDis() { return (Rand(200, 5000) * 0.01f) / (Rand(200, 5000) * 0.01f); }
void DrawLine(Window *window, uint32_t color, int startX, int startY, int endX, int endY)
{
  // 2D-DDA algorithm
  int posx = startX;
  int posy = startY;
  float dirx = endX - startX + FLT_EPSILON;
  float diry = endY - startY + FLT_EPSILON;
  int attx = dirx > 0;
  int atty = diry > 0;
  float dirLen = sqrt(dirx * dirx + diry * diry);
  dirx /= dirLen;
  diry /= dirLen;
  float idirx = 1.0 / dirx;
  float idiry = 1.0 / diry;
  float fposx = posx;
  float fposy = posy;
  float currLen = 0;
  while (currLen < dirLen)
  {
    if(posx >= 0 && posx < window->width && posy >= 0 && posy < window->height) window->pixels[posx + posy * window->width] = color;
    float crossx = ((posx + attx) - fposx) * idirx;
    float crossy = ((posy + atty) - fposy) * idiry;
    bool cross = crossx < crossy;
    if (cross)
    {
      posx += attx * 2 - 1;
      fposx += dirx * crossx;
      fposy += diry * crossx;
      currLen += crossx;
    }
    else
    {
      posy += atty * 2 - 1;
      fposx += dirx * crossy;
      fposy += diry * crossy;
      currLen += crossy;
    }
  }
}

struct Gene
{
  float lengthMult = 1.0f;
  float angleMult = 1.0f;
  float limbMult = 1.0f;
  float redMult = 1.0f;
  float greenMult = 1.0f;
  float blueMult = 1.0f;
};

struct BioMorph
{
  std::vector<Gene> genes;

  void Express(int geneID, Window *window, int xpos, int ypos, float len, float ang, float red, float green, float blue)
  {
    if (geneID >= genes.size()) return;
    len = (genes[geneID].lengthMult-1) * 10;
    red = genes[geneID].redMult * 100;
    green = genes[geneID].greenMult * 100;
    blue = genes[geneID].blueMult * 100;
    int limbCount = 2;
    for (int64_t l = 0; l < limbCount; l++)
    {
      float myAng = ang + genes[geneID].angleMult*2 * (l * 2 - 1);
      int myXpos = (int)(xpos + sin(myAng) * len);
      int myYpos = (int)(ypos - cos(myAng) * len);
      int myRed = (int)red;
      int myGreen = (int)green;
      int myBlue = (int)blue;
      uint32_t myCol = (myRed << 16) + (myGreen << 8) + myBlue;
      DrawLine(window, myCol, xpos, ypos, myXpos, myYpos);
      Express(geneID + 1, window, myXpos, myYpos, len, myAng, red, green, blue);
    }
  }

  void Randomize()
  {
    //genes.resize(Rand(1, 3));
    genes.resize(7);
    for (auto & gene : genes)
    {
      gene.lengthMult = NormalDis();
      gene.angleMult = NormalDis();
      gene.limbMult = NormalDis();
      gene.redMult = NormalDis();
      gene.greenMult = NormalDis();
      gene.blueMult = NormalDis();
    }
  }
  void Mutate()
  {
    int geneID = Rand(0, genes.size() - 1);
    int subGeneID = Rand(0, 5);
    float &gene = (&genes[geneID].angleMult)[subGeneID];
    //gene = ((gene * 99) + NormalDis()) * 0.01;
    //gene = NormalDis();
    if (Rand(0, 1))
      gene *= 1.1;
    else
      gene /= 1.1;
    if (gene < 0.2) gene = 0.2;
    if (gene > 5) gene = 5;
  }
};

void SpawnChildren(std::vector<BioMorph> *bioMorphs, int parent, BioMorph *grandma)
{
  const int mutationRate = 200;
  for (int64_t c = 0; c < bioMorphs->size(); c++)
    if (c != parent)
    {
      (*bioMorphs)[c] = (*bioMorphs)[parent];
      for (int i = 0; i < (*bioMorphs)[c].genes.size(); i++) // mutation momentum
      {
        float scrub = 1.0;
        (*bioMorphs)[c].genes[i].angleMult += ((*bioMorphs)[parent].genes[i].angleMult - grandma->genes[i].angleMult) * scrub;
        (*bioMorphs)[c].genes[i].blueMult += ((*bioMorphs)[parent].genes[i].blueMult - grandma->genes[i].blueMult) * scrub;
        (*bioMorphs)[c].genes[i].greenMult += ((*bioMorphs)[parent].genes[i].greenMult - grandma->genes[i].greenMult) * scrub;
        (*bioMorphs)[c].genes[i].lengthMult += ((*bioMorphs)[parent].genes[i].lengthMult - grandma->genes[i].lengthMult) * scrub;
        (*bioMorphs)[c].genes[i].limbMult += ((*bioMorphs)[parent].genes[i].limbMult - grandma->genes[i].limbMult) * scrub;
        (*bioMorphs)[c].genes[i].redMult += ((*bioMorphs)[parent].genes[i].redMult - grandma->genes[i].redMult) * scrub;
      }
      for (int i = 0; i < mutationRate; i++)
        (*bioMorphs)[c].Mutate();
    }
}

void BioMorphs()
{
  // Setup
  int rows = 3;
  int colums = 5;
  std::vector<BioMorph> bioMorphs;
  bioMorphs.resize(rows * colums);
  int parent = ((rows / 2) * colums) + (colums / 2);
  BioMorph grandma;

  // Begin first round
  grandma.Randomize();
  bioMorphs[parent] = grandma;
  SpawnChildren(&bioMorphs, parent, &grandma);

  Window window("BioMorphs", false, 1000, 600);

  while (Controls::Update())
  {
    window.Clear();

    vec2i mouse = Controls::GetMouse();
    int mouseCol = mouse.x / 200;
    int mouseRow = mouse.y / 200;

    static int timer = 0;
    // Draw BioMorphs
    int id = 0;
    for (int y = 0; y < rows; y++)
      for (int x = 0; x < colums; x++)
      {
        if (x == mouseCol && y == mouseRow)
        {
          float scrub = (mouse.x - (mouseCol * 200)) / 200.0;
          BioMorph scrubMorph = bioMorphs[id];
          for (int g = 0; g < scrubMorph.genes.size(); g++)
          {
            scrubMorph.genes[g].angleMult = bioMorphs[parent].genes[g].angleMult * (1.0 - scrub) + bioMorphs[id].genes[g].angleMult * scrub;
            scrubMorph.genes[g].blueMult = bioMorphs[parent].genes[g].blueMult * (1.0 - scrub) + bioMorphs[id].genes[g].blueMult * scrub;
            scrubMorph.genes[g].greenMult = bioMorphs[parent].genes[g].greenMult * (1.0 - scrub) + bioMorphs[id].genes[g].greenMult * scrub;
            scrubMorph.genes[g].lengthMult = bioMorphs[parent].genes[g].lengthMult * (1.0 - scrub) + bioMorphs[id].genes[g].lengthMult * scrub;
            scrubMorph.genes[g].limbMult = bioMorphs[parent].genes[g].limbMult * (1.0 - scrub) + bioMorphs[id].genes[g].limbMult * scrub;
            scrubMorph.genes[g].redMult = bioMorphs[parent].genes[g].redMult * (1.0 - scrub) + bioMorphs[id].genes[g].redMult * scrub;
          }
          scrubMorph.Express(0, &window, x * 200 + 100, y * 200 + 100, 10.0, 0.0, 128, 128, 128);
        }
        else
        {
          timer++;
          float scrub = (sin(timer * 0.002)+1)*0.5;
          BioMorph scrubMorph = bioMorphs[id];
          for (int g = 0; g < scrubMorph.genes.size(); g++)
          {
            scrubMorph.genes[g].angleMult = bioMorphs[parent].genes[g].angleMult * (1.0 - scrub) + bioMorphs[id].genes[g].angleMult * scrub;
            scrubMorph.genes[g].blueMult = bioMorphs[parent].genes[g].blueMult * (1.0 - scrub) + bioMorphs[id].genes[g].blueMult * scrub;
            scrubMorph.genes[g].greenMult = bioMorphs[parent].genes[g].greenMult * (1.0 - scrub) + bioMorphs[id].genes[g].greenMult * scrub;
            scrubMorph.genes[g].lengthMult = bioMorphs[parent].genes[g].lengthMult * (1.0 - scrub) + bioMorphs[id].genes[g].lengthMult * scrub;
            scrubMorph.genes[g].limbMult = bioMorphs[parent].genes[g].limbMult * (1.0 - scrub) + bioMorphs[id].genes[g].limbMult * scrub;
            scrubMorph.genes[g].redMult = bioMorphs[parent].genes[g].redMult * (1.0 - scrub) + bioMorphs[id].genes[g].redMult * scrub;
          }
          scrubMorph.Express(0, &window, x * 200 + 100, y * 200 + 100, 10.0, 0.0, 128, 128, 128);

          //bioMorphs[id].Express(0, &window, x * 200 + 100, y * 200 + 100, 10.0, 0.0, 128, 128, 128);
        }
        id++;
      }

    //if (timer % 2000 == 0)
      //SpawnChildren(&bioMorphs, parent, &grandma);

    // Select child
    int x = mouseCol * 200;
    int y = mouseRow * 200;
    DrawLine(&window, 0xFFFFFFFF, x, y, x + 200, y);
    DrawLine(&window, 0xFFFFFFFF, x, y + 200, x + 200, y + 200);
    DrawLine(&window, 0xFFFFFFFF, x, y, x, y + 200);
    DrawLine(&window, 0xFFFFFFFF, x + 200, y, x + 200, y + 200);
    static bool clicked = false;
    if (Controls::GetLeftClick())
    {
      if (!clicked)
      {
        grandma = bioMorphs[parent];
        bioMorphs[parent] = bioMorphs[mouseCol + mouseRow * colums];
        SpawnChildren(&bioMorphs, parent, &grandma);
      }
      clicked = true;
    }
    else
      clicked = false;

    //DrawLine(&window, 255 * 256 * 256, 0, 0, mouse.x, mouse.y);
    if (Controls::GetRightClick())
    {
      bioMorphs[parent].Randomize();
      grandma = bioMorphs[parent];
      SpawnChildren(&bioMorphs, parent, &grandma);
      window.Swap();
      SDL_Delay(25);
    }

    window.Swap();
  }

}