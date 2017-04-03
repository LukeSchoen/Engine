#ifndef Convertor_h__
#define Convertor_h__
#include "File.h"
#include "StreamFile.h"
#include "Maths.h"
#include <time.h>
#include "Vector.h"
#include <vector>
#include <string>
#include "Creator.h"
#include "FilePath.h"

struct SplitTask
{
  SplitTask(const char *_filePath, int _size, vec3i _position = vec3i())
  {
    filePath = strdup(_filePath);
    size = _size;
    position = _position;
    for (int cItr = 0; cItr < 8; cItr++)
      children[cItr] = 0;
    discLocation = 0;
  }

  SplitTask()
  {
    for (int cItr = 0; cItr < 8; cItr++)
      children[cItr] = 0;
    discLocation = 0;
  }

  SplitTask(const SplitTask &o)
  {
    filePath = strdup(o.filePath);
    size = o.size;
    position = o.position;
    discLocation = o.discLocation;
    memcpy(children, o.children, 8 * sizeof(int64_t));
  }

  SplitTask(SplitTask &&o)
  {
    filePath = o.filePath;
    size = o.size;
    position = o.position;
    o.filePath = nullptr;
    discLocation = o.discLocation;
    memcpy(children, o.children, 8 * sizeof(int64_t));
  }

  SplitTask & operator =(const SplitTask &o)
  {
    free(filePath);
    filePath = strdup(o.filePath);
    size = o.size;
    position = o.position;
    discLocation = o.discLocation;
    memcpy(children, o.children, 8 * sizeof(int64_t));
    return *this;
  }

  ~SplitTask()
  {
    free(filePath);
    filePath = nullptr;
  }

  char *filePath;
  int size;
  vec3i position;
  int64_t discLocation;
  int64_t children[8] = { 0 };
};

struct point
{
  int32_t x;
  int32_t y;
  int32_t z;
  uint32_t color;
};

static int64_t BlockID = 0;

struct Convertor
{
  static void NormalizedPCFtoXYZGrid(const char *inputPath, const char *outputPath, uint32_t gridSize, vec3i gridPos)
  {
    // Read PCF
    int64_t pointCount;
    point * points = (point *)File::ReadFile(inputPath, &pointCount);
    pointCount /= sizeof(point);

    // Create Grid
    uint32_t *grid = new uint32_t[gridSize * gridSize * gridSize];
    uint32_t *Rgrid = new uint32_t[gridSize * gridSize * gridSize];
    uint32_t *Ggrid = new uint32_t[gridSize * gridSize * gridSize];
    uint32_t *Bgrid = new uint32_t[gridSize * gridSize * gridSize];
    uint32_t *Sgrid = new uint32_t[gridSize * gridSize * gridSize];
    memset(grid, 0, gridSize * gridSize* gridSize * sizeof(uint32_t));
    memset(Rgrid, 0, gridSize * gridSize* gridSize * sizeof(uint32_t));
    memset(Ggrid, 0, gridSize * gridSize* gridSize * sizeof(uint32_t));
    memset(Bgrid, 0, gridSize * gridSize* gridSize * sizeof(uint32_t));
    memset(Sgrid, 0, gridSize * gridSize* gridSize * sizeof(uint32_t));

    // Populate Grid
    for (int64_t i = 0; i < pointCount; i++)
    {
      point &p = points[i];
      // Normalize into grid space
      int32_t x = (p.x - gridPos.x);
      int32_t y = (p.y - gridPos.y);
      int32_t z = (p.z - gridPos.z);

      uint32_t pR = (p.color >> 16) & 255;
      uint32_t pG = (p.color >> 8) & 255;
      uint32_t pB = (p.color >> 0) & 255;

      Rgrid[x + y * gridSize + z * gridSize * gridSize] += pR;
      Ggrid[x + y * gridSize + z * gridSize * gridSize] += pG;
      Bgrid[x + y * gridSize + z * gridSize * gridSize] += pB;
      Sgrid[x + y * gridSize + z * gridSize * gridSize]++;
    }
    printf("combining!\n");
    for (int z = 0; z < gridSize; z++)
      for (int y = 0; y < gridSize; y++)
        for (int x = 0; x < gridSize; x++)
        {
          uint32_t gS = Sgrid[x + y * gridSize + z * gridSize * gridSize];
          if (gS)
          {
            uint32_t gR = Rgrid[x + y * gridSize + z * gridSize * gridSize] / gS;
            uint32_t gG = Ggrid[x + y * gridSize + z * gridSize * gridSize] / gS;
            uint32_t gB = Bgrid[x + y * gridSize + z * gridSize * gridSize] / gS;
            grid[x + y * gridSize + z * gridSize * gridSize] = gR + gG * 256 + gB * 256 * 256;
          }
        }

    // Write Grid
    File::WriteFile(outputPath, grid, gridSize * gridSize * gridSize * sizeof(uint32_t));
    delete[] points;
    delete[] grid;
    delete[] Rgrid;
    delete[] Ggrid;
    delete[] Bgrid;
    delete[] Sgrid;
  }

  static void PCFtoXYZGrid(const char *inputPath, const char *outputPath, uint32_t gridSize)
  {
    // Read PCF
    int64_t pointCount;
    point * points = (point *)File::ReadFile(inputPath, &pointCount);
    pointCount /= sizeof(point);

    // Calculate PCF extents
    int32_t maxX, maxY, maxZ, minX, minY, minZ;
    maxX = maxY = maxZ = -1000000000;
    minX = minY = minZ = 1000000000;
    for (int64_t i = 0; i < pointCount; i++)
    {
      point &p = points[i];
      maxX = Max(p.x, maxX);
      maxY = Max(p.y, maxY);
      maxZ = Max(p.z, maxZ);
      minX = Min(p.x, minX);
      minY = Min(p.y, minY);
      minZ = Min(p.z, minZ);
    }

    // Create Grid
    uint32_t *grid = new uint32_t[gridSize * gridSize * gridSize];
    uint32_t *Rgrid = new uint32_t[gridSize * gridSize * gridSize];
    uint32_t *Ggrid = new uint32_t[gridSize * gridSize * gridSize];
    uint32_t *Bgrid = new uint32_t[gridSize * gridSize * gridSize];
    uint32_t *Sgrid = new uint32_t[gridSize * gridSize * gridSize];
    memset(grid, 0, gridSize * gridSize* gridSize * sizeof(uint32_t));
    memset(Rgrid, 0, gridSize * gridSize* gridSize * sizeof(uint32_t));
    memset(Ggrid, 0, gridSize * gridSize* gridSize * sizeof(uint32_t));
    memset(Bgrid, 0, gridSize * gridSize* gridSize * sizeof(uint32_t));
    memset(Sgrid, 0, gridSize * gridSize* gridSize * sizeof(uint32_t));

    // Populate Grid
    for (int64_t i = 0; i < pointCount; i++)
    {
      point &p = points[i];
      // Normalize into grid space
      int32_t biggestEdge = Max(Max(maxX - minX, maxY - minY), maxZ - minZ);
      int32_t x = (p.x - minX) * (gridSize - 1) / biggestEdge;
      int32_t y = (p.y - minY) * (gridSize - 1) / biggestEdge;
      int32_t z = (p.z - minZ) * (gridSize - 1) / biggestEdge;

      uint32_t pR = (p.color >> 16) & 255;
      uint32_t pG = (p.color >> 8) & 255;
      uint32_t pB = (p.color >> 0) & 255;

      Rgrid[x + y * gridSize + z * gridSize * gridSize] += pR;
      Ggrid[x + y * gridSize + z * gridSize * gridSize] += pG;
      Bgrid[x + y * gridSize + z * gridSize * gridSize] += pB;
      Sgrid[x + y * gridSize + z * gridSize * gridSize]++;
    }
    printf("combining!\n");
    for (int z = 0; z < gridSize; z++)
      for (int y = 0; y < gridSize; y++)
        for (int x = 0; x < gridSize; x++)
        {
          uint32_t gS = Sgrid[x + y * gridSize + z * gridSize * gridSize];
          if (gS)
          {
            uint32_t gR = Rgrid[x + y * gridSize + z * gridSize * gridSize] / gS;
            uint32_t gG = Ggrid[x + y * gridSize + z * gridSize * gridSize] / gS;
            uint32_t gB = Bgrid[x + y * gridSize + z * gridSize * gridSize] / gS;
            grid[x + y * gridSize + z * gridSize * gridSize] = gR + gG * 256 + gB * 256 * 256;
          }
        }

    // Write Grid
    File::WriteFile(outputPath, grid, gridSize * gridSize * gridSize * sizeof(uint32_t));
    delete[] points;
    delete[] grid;
    delete[] Rgrid;
    delete[] Ggrid;
    delete[] Bgrid;
    delete[] Sgrid;
  }

  static void SplitPCF(int TaskID, const char *outputDirectory, std::vector<SplitTask> &splitTaskList)
  {
    SplitTask lTask = splitTaskList[TaskID];
    if (!File::FileExists(lTask.filePath)) return;
    // Setup Output Streams
    StreamFileWriter *outputPCFs[8];
    for (int i = 0; i < 8; i++)
    {
      FilePath outFileName = lTask.filePath;
      
      char nodeIDtxt[2];
      sprintf(nodeIDtxt, "%d", i + 1);
      std::string fullName = outFileName.GetNameNoExt() + nodeIDtxt + outFileName.GetExtension();
      std::string outputDir = outputDirectory;
      outputDir += fullName;
      outputPCFs[i] = new StreamFileWriter(outputDir.c_str());

      // Add New Sub Tasks To Main Task List
      int x = (i & 1) > 0;
      int y = (i & 2) > 0;
      int z = (i & 4) > 0;
      uint32_t subTaskSize = lTask.size >> 1;
      vec3i subTaskPos = lTask.position;
      subTaskPos = subTaskPos + vec3i(subTaskSize * x, subTaskSize * y, subTaskSize * z);
      SplitTask subTask(outputDir.c_str(), subTaskSize, subTaskPos);
      splitTaskList[TaskID].children[i] = ++BlockID;
      splitTaskList.push_back(subTask);
    }

    //Calculate Split Points
    uint32_t HCS = lTask.size >> 1;
    int32_t HCX = lTask.position.x + HCS;
    int32_t HCY = lTask.position.y + HCS;
    int32_t HCZ = lTask.position.z + HCS;

    int64_t handSize = sizeof(point) * 65536;
    StreamFileReader inputPCF(lTask.filePath, nullptr, handSize);
    // Calculate Models Extents
    int64_t maxX, maxY, maxZ, minX, minY, minZ;
    maxX = maxY = maxZ = -1000000000;
    minX = minY = minZ = 1000000000;
    while (true)
    {
      // Read PCF
      int64_t pointCount;
      point *points = (point*)inputPCF.ReadBytes(handSize, &pointCount);;
      if (pointCount == 0) break;
      pointCount /= sizeof(point);
      for (int64_t pItr = 0; pItr < pointCount; pItr++)
      {
        point &p = points[pItr];
        int childID = (p.x >= HCX) | ((p.y >= HCY) << 1) | ((p.z >= HCZ) << 2);
        outputPCFs[childID]->WriteBytes(&p, sizeof(p));
      }
    }

    // Complete Output Stream
    for (int i = 0; i < 8; i++)
    {
      outputPCFs[i]->StopStream();
      delete outputPCFs[i];
    }

  }

  static int64_t PCFLongestSide(const char *inputPath)
  {
    printf("Measuring input model...\n");
    int64_t handSize = sizeof(point) * 65536;
    StreamFileReader inputPCF(inputPath, nullptr, handSize);
    // Calculate Models Extents
    int64_t maxX, maxY, maxZ, minX, minY, minZ;
    maxX = maxY = maxZ = -1000000000;
    minX = minY = minZ = 1000000000;
    while (true)
    {
      // Read PCF
      int64_t pointCount;
      point *points = (point*)inputPCF.ReadBytes(handSize, &pointCount);;
      if (pointCount == 0) break;
      pointCount /= sizeof(point);

      // Calculate PCF extents
      for (int64_t i = 0; i < pointCount; i++)
      {
        point &p = points[i];
        maxX = Max(p.x, maxX);
        maxY = Max(p.y, maxY);
        maxZ = Max(p.z, maxZ);
        minX = Min(p.x, minX);
        minY = Min(p.y, minY);
        minZ = Min(p.z, minZ);
      }
    }

    // Create Transformed Point Cloud File
    return Max(Max(maxX - minX, maxY - minY), maxZ - minZ);
  }

  static void ResamplePCF(const char *inputPath, const char *outputPath, uint32_t newSize)
  {
    // Transform Points Into Model Units
    int64_t handSize = sizeof(point) * 65536;
    StreamFileReader inputPCF(inputPath, nullptr, handSize);
    StreamFileWriter outputPCF(outputPath, nullptr, handSize);
    // Calculate Original Model Extents
    int32_t maxX, maxY, maxZ, minX, minY, minZ;
    maxX = maxY = maxZ = -1000000000;
    minX = minY = minZ = 1000000000;
    while (true)
    {
      // Read PCF
      int64_t pointCount;
      point *points = (point*)inputPCF.ReadBytes(handSize, &pointCount);
      if (pointCount == 0) break;
      pointCount /= sizeof(point);

      // Calculate PCF extents
      for (int64_t i = 0; i < pointCount; i++)
      {
        point &p = points[i];
        maxX = Max(p.x, maxX);
        maxY = Max(p.y, maxY);
        maxZ = Max(p.z, maxZ);
        minX = Min(p.x, minX);
        minY = Min(p.y, minY);
        minZ = Min(p.z, minZ);
      }
    }

    // Reread the file
    inputPCF.SetLocation(0);

    // Create Transformed Point Cloud File
    int32_t biggestEdge = Max(Max(maxX - minX, maxY - minY), maxZ - minZ);
    while (true)
    {
      // Read PCF
      int64_t pointCount;
      point *points = (point*)inputPCF.ReadBytes(handSize, &pointCount);
      if (pointCount == 0) break;
      pointCount /= sizeof(point);
      for (int64_t i = 0; i < pointCount; i++)
      {
        // Normalize into model space
        point &p = points[i];
        p.x = int64_t(p.x - minX) * int64_t(newSize - 1) / biggestEdge;
        p.y = int64_t(p.y - minY) * int64_t(newSize - 1) / biggestEdge;
        p.z = int64_t(p.z - minZ) * int64_t(newSize - 1) / biggestEdge;
        outputPCF.WriteBytes(&p, sizeof(p));
      }
    }
    outputPCF.StopStream();
  }

  static void ExportPCFtoGlorious(const char *inputPath, const char *outputPath, const char *tempDirectory, uint32_t modelSize)
  {
    // Split Resolution
    int gridSize = 256;

    // Calculate Enclosing Octree
    int fullModelRes = gridSize;
    while (fullModelRes < modelSize)
      fullModelRes = fullModelRes << 1;

    // Transform Point Cloud Into Model Units
    std::string tempDir = tempDirectory;
    tempDir += "/model.pcf";
    const char *transformedPointCloud = tempDir.c_str();
    printf("Re-sampling\n");
    ResamplePCF(inputPath, transformedPointCloud, modelSize);

    // Setup Recursive Point Cloud Split Task
    std::vector<SplitTask> splitTask;
    SplitTask splitWork(transformedPointCloud, fullModelRes);
    splitTask.push_back(splitWork);

    // Create Point Cloud Octree
    bool splitting = true;
    while (splitting)
    {
      splitting = false;
      for (int i = 0; i < splitTask.size(); i++)
      {
        SplitTask job = splitTask[i];
        if (job.size > gridSize)
        {
          printf("Splitting...%d\n", rand());
          SplitPCF(i, tempDirectory, splitTask);
          File::DelFile(job.filePath);
          delete[] job.filePath;
          splitTask.erase(splitTask.begin() + i);
          splitting = true;
          break;
        }
      }
    }


    // Create Render
    StreamFileWriter gloriousModel(outputPath);
    int modelCount = 0;
    gloriousModel.WriteBytes(&modelCount, sizeof(modelCount));

    // Combine Coherency
    for (int i = 0; i < splitTask.size(); i++)
    {
      SplitTask job = splitTask[i];
      if (File::FileExists(job.filePath))
      {
        // Convert To Grid
        FilePath gridFilePath = job.filePath;

        std::string gridFile = gridFilePath.GetFolder() + gridFilePath.GetName() + ".grid";

        NormalizedPCFtoXYZGrid(job.filePath, gridFile.c_str(), job.size, job.position);
        // Delete Point Cloud
        File::DelFile(job.filePath);

        // Slice Grid
        RenderObject * cliffNote = Creator::SliceGrid(gridFile.c_str(), job.size, job.position);
        // Delete Grid
        File::DelFile(gridFile.c_str());

        char *cliffNoteData;
        int cliffNoteDataLength;
        cliffNote->Serialize(&cliffNoteData, &cliffNoteDataLength);
        if (cliffNoteData)
        {
          gloriousModel.WriteBytes(cliffNoteData, cliffNoteDataLength);
          modelCount++;
        }
      }
    }
    gloriousModel.SetLocation(0);
    gloriousModel.WriteBytes(&modelCount, sizeof(modelCount));


  }

  static int64_t CloudToCosmBlock(const char *inputPath, StreamFileWriter &NovaCosmModel, uint32_t cloudSize, uint32_t gridSize, vec3i gridPos)
  {
    if (!File::FileExists(inputPath)) return 0;
    // Create Grid
    uint32_t *grid = (uint32_t*)calloc(gridSize * gridSize * gridSize, sizeof(uint32_t));
    uint32_t *Rgrid = (uint32_t*)calloc(gridSize * gridSize * gridSize, sizeof(uint32_t));
    uint32_t *Ggrid = (uint32_t*)calloc(gridSize * gridSize * gridSize, sizeof(uint32_t));
    uint32_t *Bgrid = (uint32_t*)calloc(gridSize * gridSize * gridSize, sizeof(uint32_t));
    uint32_t *Sgrid = (uint32_t*)calloc(gridSize * gridSize * gridSize, sizeof(uint32_t));

    const int holFillSize = 2;

    printf("blitting!\n");

    // Read PCF
    int64_t handSize = sizeof(point) * 65536;
    StreamFileReader inputPCF(inputPath, nullptr, handSize);
    // Calculate Models Extents
    int64_t maxX, maxY, maxZ, minX, minY, minZ;
    maxX = maxY = maxZ = -1000000000;
    minX = minY = minZ = 1000000000;
    while (true)
    {
      // Read PCF
      int64_t pointCount;
      point *points = (point*)inputPCF.ReadBytes(handSize, &pointCount);;
      if (pointCount == 0) break;
      pointCount /= sizeof(point);

      // Populate Grid
      for (int64_t i = 0; i < pointCount; i++)
      {
        point &p = points[i];
        // Normalize into grid space
        int32_t x = (p.x - gridPos.x) * gridSize / cloudSize;
        int32_t y = (p.y - gridPos.y) * gridSize / cloudSize;
        int32_t z = (p.z - gridPos.z) * gridSize / cloudSize;

        uint32_t pR = (p.color >> 16) & 255;
        uint32_t pG = (p.color >> 8) & 255;
        uint32_t pB = (p.color >> 0) & 255;

        // Single sample
        //Rgrid[x + y * gridSize + z * gridSize * gridSize] += pR;
        //Ggrid[x + y * gridSize + z * gridSize * gridSize] += pG;
        //Bgrid[x + y * gridSize + z * gridSize * gridSize] += pB;
        //Sgrid[x + y * gridSize + z * gridSize * gridSize]++;

        // Hole filler
        for (int iz = -holFillSize; iz <= holFillSize; iz++)
          for (int iy = -holFillSize; iy <= holFillSize; iy++)
            for (int ix = -holFillSize; ix <= holFillSize; ix++)
              if (x + ix >= 0 && y + iy >= 0 && z + iz >= 0 && x + ix < gridSize && y + iy < gridSize  && z + iz < gridSize)
              {
                float l = vec3(ix * 2, iy * 2, iz * 2).LengthSquared() + 1;
                int dm = Min(Max(256.0f / (l * l * l * l), 1), 256);
                int i = (x + ix) + (y + iy) * gridSize + (z + iz) * gridSize * gridSize;
                Rgrid[i] += pR * dm;
                Ggrid[i] += pG * dm;
                Bgrid[i] += pB * dm;
                Sgrid[i] += dm;
              }
      }
    }

    printf("combining!\n");
    for (int z = 0; z < gridSize; z++)
      for (int y = 0; y < gridSize; y++)
        for (int x = 0; x < gridSize; x++)
        {
          uint32_t gS = Sgrid[x + y * gridSize + z * gridSize * gridSize];
          if (gS)
          {
            uint32_t gR = Rgrid[x + y * gridSize + z * gridSize * gridSize] / gS;
            uint32_t gG = Ggrid[x + y * gridSize + z * gridSize * gridSize] / gS;
            uint32_t gB = Bgrid[x + y * gridSize + z * gridSize * gridSize] / gS;
            grid[x + y * gridSize + z * gridSize * gridSize] = gR + gG * 256 + gB * 256 * 256;
          }
        }

    // Corrosion pass
    for (int z = 0; z < gridSize; z++)
      for (int y = 0; y < gridSize; y++)
        for (int x = 0; x < gridSize; x++)
          if (grid[x + y * gridSize + z * gridSize * gridSize])
          {
            bool c = false;
            for (int iz = -holFillSize; iz <= holFillSize; iz++)
              for (int iy = -holFillSize; iy <= holFillSize; iy++)
                for (int ix = -holFillSize; ix <= holFillSize; ix++)
                  if (x + ix >= 0 && y + iy >= 0 && z + iz >= 0 && x + ix < gridSize && y + iy < gridSize  && z + iz < gridSize)
                    if (Sgrid[(x + ix) + (y + iy) * gridSize + (z + iz) * gridSize * gridSize] == 0)
                      c = true;
            if (c) grid[x + y * gridSize + z * gridSize * gridSize] = 0;
          }

    int64_t voxelCount = 0;
    struct { uint8_t x, y, z, r, g, b; } voxel;
    for (int z = 0; z < gridSize; z++)
      for (int y = 0; y < gridSize; y++)
        for (int x = 0; x < gridSize; x++)
        {
          uint32_t c = grid[x + y * gridSize + z * gridSize * gridSize];
          if (c)
          {
            // Bury Algorithm
            bool visable = false;
            if (x * y * z == 0 || (gridSize - 1 - x) * (gridSize - 1 - y) * (gridSize - 1 - z) == 0) visable = true; // edge 
            if (!visable) visable = grid[(x - 1) + y * gridSize + z * gridSize * gridSize]; // left exposed
            if (!visable) visable = grid[(x + 1) + y * gridSize + z * gridSize * gridSize]; // right exposed
            if (!visable) visable = grid[x + (y - 1) * gridSize + z * gridSize * gridSize]; // up exposed
            if (!visable) visable = grid[x + (y + 1) * gridSize + z * gridSize * gridSize]; // down exposed
            if (!visable) visable = grid[x + y * gridSize + (z + 1) * gridSize * gridSize]; // front exposed
            if (!visable) visable = grid[x + y * gridSize + (z - 1) * gridSize * gridSize]; // back exposed
            if (visable)
            {
              voxel.x = x;
              voxel.y = y;
              voxel.z = z;
              voxel.r = c & 0xFF;
              voxel.g = (c >> 8) & 0xFF;
              voxel.b = (c >> 16) & 0xFF;
              NovaCosmModel.WriteBytes(&voxel, sizeof(voxel));
              voxelCount++;
            }
          }
        }

    free(grid);
    free(Rgrid);
    free(Ggrid);
    free(Bgrid);
    free(Sgrid);

    return voxelCount;
  }

  static void ExportPCFtoNovaCosm(const char *inputPath, const char *outputPath, const char *tempDirectory, uint32_t modelSize = 0)
  {
    // Split Resolution
    int gridSize = 256;

    // Reset NodeIDS
    BlockID = 0;

    if (modelSize == 0) // Use PCFs actual size
      modelSize = PCFLongestSide(inputPath);

    // Calculate Enclosing Octree
    int fullModelRes = gridSize;
    while (fullModelRes < modelSize)
      fullModelRes = fullModelRes << 1;

    // Transform Point Cloud Into Model Units
    std::string transformedPointCloud = tempDirectory;
    transformedPointCloud += "/model.pcf";
    printf("Re-sampling\n");
    ResamplePCF(inputPath, transformedPointCloud.c_str(), modelSize);

    // Setup Recursive Point Cloud Split Task
    std::vector<SplitTask> splitTask;
    std::vector<SplitTask> completedTasks;
    SplitTask splitWork(transformedPointCloud.c_str(), fullModelRes);
    splitTask.push_back(splitWork);

    // Create Point Cloud Hierarchy
    while (splitTask.size() > 0)
    {
      if (splitTask[0].size > gridSize)
      {
        if (File::FileExists(splitTask[0].filePath))
        {
          printf("Splitting...%d\n", rand());
          SplitPCF(0, tempDirectory, splitTask);
          //File::DelFile(splitTask[0].filePath);
        }
      }
      completedTasks.push_back(splitTask[0]);
      splitTask.erase(splitTask.begin());
    }

    // Create NovaCosm
    StreamFileWriter NovaCosmModel(outputPath);

    // Create Blocks
    for (int i = 0; i < completedTasks.size(); i++)
    {
      SplitTask &job = completedTasks[i];
      if (File::FileExists(job.filePath))
      {
        vec3 blockPos(job.position.x, job.position.y, job.position.z);
        blockPos = blockPos * gridSize / job.size; // Resize positions
        job.discLocation = NovaCosmModel.GetLocation();
        // Write Dummy Children
        NovaCosmModel.WriteBytes(job.children, sizeof(job.children));
        // Write Actual Position
        NovaCosmModel.WriteBytes(blockPos.Data(), sizeof(blockPos));
        // Write Dummy voxel Count
        int64_t voxelCount = 0;
        NovaCosmModel.WriteBytes(&voxelCount, sizeof(voxelCount));

        voxelCount = CloudToCosmBlock(job.filePath, NovaCosmModel, job.size, gridSize, job.position);

        int64_t fileEnd = NovaCosmModel.GetLocation();
        NovaCosmModel.SetLocation(job.discLocation);
        NovaCosmModel.WriteBytes(job.children, sizeof(job.children));
        NovaCosmModel.WriteBytes(blockPos.Data(), sizeof(blockPos));
        NovaCosmModel.WriteBytes(&voxelCount, sizeof(voxelCount));
        NovaCosmModel.SetLocation(fileEnd);
      }
    }

    // Link Blocks Together
    for (int i = 0; i < completedTasks.size(); i++)
    {
      SplitTask &job = completedTasks[i];
      if (File::FileExists(job.filePath))
      {
        NovaCosmModel.SetLocation(job.discLocation);
        for (int cID = 0; cID < 8; cID++) // 8 Children's Actual disc locations
        {
          int64_t child = job.children[cID];
          if (child)
            NovaCosmModel.WriteBytes(&completedTasks[child].discLocation, sizeof(int64_t));
          else
            NovaCosmModel.WriteBytes(&child, sizeof(int64_t));
        }
      }
    }

    for (int i = 0; i < completedTasks.size(); i++)
    {
      SplitTask &job = completedTasks[i];
      if (File::FileExists(job.filePath))
        File::DelFile(job.filePath);
    }
  }

};


#endif // Convertor_h__
