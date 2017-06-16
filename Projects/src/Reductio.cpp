#include "Reductio.h"
#include "Window.h"
#include "Controls.h"
#include <time.h>
#include "Threads.h"
#include "Camera.h"
#include "ImageFile.h"
#include "PointCloud.h"
#include "Convertor.h"
#include <thread>

struct ScreenJob
{
  int m_x;
  int m_y;
  int m_w;
  int m_h;
};

struct WorldJob
{
  int m_x;
  int m_y;
  int m_z;
  int m_w;
  int m_h;
  int m_l;
};

bool SphereInFrustum(float x, float y, float z, float radius)
{
  // Go through all the sides of the frustum
  for (int i = 0; i < 6; i++)
  {
    // If the center of the sphere is farther away from the plane than the radius
    //if (m_Frustum[i][A] * x + m_Frustum[i][B] * y + m_Frustum[i][C] * z + m_Frustum[i][D] <= -radius)
    {
      // The distance was greater than the radius so the sphere is outside of the frustum
      return false;
    }
  }

  // The sphere was inside of the frustum!
  return true;
}

static std::vector<float> s_renderDepth;
static std::vector<float> s_streamDepth;
static std::vector<Point> s_ActiveList[2];
static bool s_swap = false;
static int activeList;
bool s_running = true;
mat4 s_MVP;

void Stream(Window *win, PointCloud *cloud)
{
  while (s_running)
  {
    if (s_streamDepth.size() != win->width * win->height) s_streamDepth.resize(win->width * win->height);
    if (s_ActiveList[1 - activeList].size() != win->width * win->height) s_ActiveList[1 - activeList].resize(win->width * win->height);

    for (auto & item : s_streamDepth) item = FLT_MAX;
    for (auto & item : s_ActiveList[1 - activeList]) item.color = 0;

    for (int64_t p = 0; p < cloud->pointCount; p++)
    {
      Point & point = cloud->points[p];

      // Project
      vec4 worldPos(point.x, 0 - point.y, point.z, 1);
      vec4 viewPos = s_MVP * worldPos;
      if (viewPos.z > 0) continue;
      float invz = 1.0 / (viewPos.z);
      int x = floor((win->width >> 1) + (viewPos.x * invz) * (win->width >> 1));
      int y = win->height - floor((win->height >> 1) + (viewPos.y  * invz) * (win->height >> 1));

      if (x >= 0 && y >= 0 && x < win->width && y < win->height)
      {
        if (0 - viewPos.z < s_streamDepth[x + y * win->width])
        {
          s_streamDepth[x + y * win->width] = 0 - viewPos.z;
          s_ActiveList[1 - activeList][x + y * win->width] = point;
        }
      }
    }
    s_swap = true;
    while (s_swap && s_running) Sleep(1);
  }
}

class Renderer
{
public:
  Renderer(Window *pWindow)
  {
    m_pWin = pWindow;
    //screenQue.reserve(m_pWin->width * m_pWin->height);
    m_pTexture = ImageFile::ReadImage("C:/Luke/Programming/Engine/Assets/Skybox/Skybox - Copy/Bottom.jpg", &m_textureWidth, &m_textureHeight);
    //m_pcf.Load("F:/Luke/Programming/Visual Studio/pixelly/Software Tracer/Colledge.pcf");

    //std::string inputFile = "F:/Luke/Programming/Visual Studio/pixelly/Software Tracer/CarrickHill.pcf";
    //std::string outputFile = "F:/Temp/out/carrickLow.pcf";
    //Convertor::ResamplePCF(inputFile.c_str(), outputFile.c_str(), 4096);
    //m_pcf.Load(outputFile.c_str());
    //streamer = new std::thread(Stream, m_pWin, &m_pcf);
  }

  ~Renderer()
  {
    //s_running = false;
    //streamer->join();
    //delete streamer;
  }

  void a_Render(mat4 MVP)
  {
    if (s_renderDepth.size() != m_pWin->width * m_pWin->height) s_renderDepth.resize(m_pWin->width * m_pWin->height);
    for (auto & item : s_renderDepth) item = FLT_MAX;

    if (s_swap)
    {
      activeList = 1 - activeList;
      s_MVP = MVP;
      s_swap = false;
    }
    for (auto & point : s_ActiveList[activeList])
    {
      if (point.color == 0) continue;
      // Project
      vec4 worldPos(point.x, 0 - point.y, point.z, 1);
      vec4 viewPos = MVP * worldPos;
      if (viewPos.z > 0) continue;
      float invz = 1.0 / (viewPos.z);
      int x = floor((m_pWin->width >> 1) + (viewPos.x * invz) * (m_pWin->width >> 1));
      int y = m_pWin->height - floor((m_pWin->height >> 1) + (viewPos.y  * invz) * (m_pWin->height >> 1));

      if (x >= 0 && y >= 0 && x < m_pWin->width && y < m_pWin->height)
      {
        int pointSize = 0 - invz * (m_pWin->width >> 1);
        if (pointSize < 1) pointSize = 1;
        //for (int iy = 0 - pointSize; iy < pointSize; iy++)
        {
          //for (int ix = 0 - pointSize; ix < pointSize; ix++)
          {
            int fx = x; // + ix;
            int fy = y; // + iy;
            if (fx >= 0 && fy >= 0 && fx < m_pWin->width && fy < m_pWin->height)
              if (0 - viewPos.z < s_renderDepth[fx + fy * m_pWin->width])
              {
                s_renderDepth[fx + fy * m_pWin->width] = 0 - viewPos.z;
                m_pWin->pixels[fx + fy * m_pWin->width] = point.color;
              }
          }
        }
      }
    }




    // fill
    for (int y = 0; y < m_pWin->height; y++)
      for (int x = m_pWin->width - 1; x > 0; x--)
        if (m_pWin->pixels[x + y * m_pWin->width] == 0 && m_pWin->pixels[x - 1 + y * m_pWin->width] != 0)
          m_pWin->pixels[x + y * m_pWin->width] = m_pWin->pixels[x - 1 + y * m_pWin->width];

    for (int y = m_pWin->height - 1; y > 0; y--)
      for (int x = 0; x < m_pWin->width; x++)
        if (m_pWin->pixels[x + y * m_pWin->width] == 0 && m_pWin->pixels[x + (y - 1) * m_pWin->width] != 0)
          m_pWin->pixels[x + y * m_pWin->width] = m_pWin->pixels[x + (y - 1) * m_pWin->width];


    Sleep(0);
  }


  vec3 UnProject(mat4 projection, vec3 camera_pos, mat4 view, Window * window, float x, float y)
  {
    float normalised_x = 2 * x / window->width - 1;
    float normalised_y = 1 - 2 * y / window->height;
    mat4 unviewMat = view.inverse() * projection.inverse();
    unviewMat.Transpose();
    vec4 near_point = unviewMat * vec4(normalised_x, normalised_y, 0, 1);
    vec3 ray_dir = vec3(near_point) - camera_pos;
    return ray_dir;
  }

  void Raytrace(mat4 view, mat4 projection)
  {
    // Ray tracer
    float invX = m_pWin->width;
    float invY = m_pWin->height;
    for (int y = 0; y < m_pWin->height; y++)
      for (int x = 0; x < m_pWin->width; x++)
      {
        vec3 ray = UnProject(projection, Camera::Position(), view, m_pWin, x, y);
        ray = ray - Camera::Position();
        ray = ray.Normalized();
        //vec4 ray(x / invX - 0.5f, y / invY - 0.5f, 1, 0);
        //ray = ray * MVP);

        if (ray.y > 0)
        {
          m_pTexture[(x & 255) + (y & 255) * 256];
          m_pWin->pixels[x + y * m_pWin->width] = 0;
        }
        else
        {
            int fx = ray.x * Camera::Position().y;
            int fz = ray.z * Camera::Position().y;

            m_pWin->pixels[x + y * m_pWin->width] = m_pTexture[abs(fx % 255) + abs(fz % 255) * 256];

          //m_pWin->pixels[x + y * m_pWin->width] = m_pTexture[(x & 255) + (y & 255) * 256];
        }
      }
  }

  void old_Render(mat4 MVP)
  {
    // Rhasterizer
    //for (int64_t p = 0; p < m_pcf.pointCount; p++)
    ////for (int64_t p = 0; p < 1000000; p++)
    //{
    //  Point & point = m_pcf.points[p];
    //  point.color;
    //
    //  mat4 aMVP = MVP;
    //  aMVP = aMVP.Transpose();
    //
    //  vec4 worldPos(point.x, point.y, point.z, 1);
    //  vec4 viewPos = aMVP * worldPos;
    //  if(viewPos.z < 0) continue;
    //  int x = floor((m_pWin->width >> 1) + (viewPos.x / viewPos.z) * (m_pWin->width >> 1));
    //  int y = floor((m_pWin->height >> 1) + (viewPos.y / viewPos.z) * (m_pWin->height >> 1));
    //  if (x >= 0 && y >= 0 && x < m_pWin->width && y < m_pWin->height)
    //    m_pWin->pixels[x + y * m_pWin->width] = point.color;
    //}

    // Ray tracer
    //for (int y = 0; y < m_pWin->height; y++)
    //  for (int x = 0; x < m_pWin->width; x++)
    //  {
    //    vec4 ray(x, y, 1, 0);
    //    ray = ray.Normalized();
    //    if (ray.y > 0)
    //    {
    //      m_pTexture[(x & 255) + (y & 255) * 256];
    //      m_pWin->pixels[x + y * m_pWin->width] = 0;
    //    }
    //    else
    //      m_pWin->pixels[x + y * m_pWin->width] = m_pTexture[(x & 255) + (y & 255) * 256];
    //  }

    //ScreenJob screenRoot;
    //screenRoot.m_w = 0;
    //screenRoot.m_h = 0;
    //screenRoot.m_w = m_pWin->width;
    //screenRoot.m_h = m_pWin->height;
    //screenQue.push_back(screenRoot);
    //
    //WorldJob worldRoot;
    //worldRoot.m_x = 0;
    //worldRoot.m_y = 0;
    //worldRoot.m_z = 0;
    //worldRoot.m_w = 256;
    //worldRoot.m_h = 256;
    //worldRoot.m_l = 256;
    //worldQue.push_back(worldRoot);
    //
    //IterativeRender();
  }

  mat4 m_MVP;

  std::vector<ScreenJob> screenQue;
  std::vector<WorldJob> worldQue;

  uint32_t *m_pTexture;
  uint32_t m_textureWidth;
  uint32_t m_textureHeight;

  PointCloud m_pcf;

  std::thread *streamer;

  Window *m_pWin = nullptr;
};


void Reductio()
{
  Threads::SetSlowMode();

  Window window("Render", false, 800, 600, false);

  Renderer reductio(&window);

  printf("Loading Done.\n");
  printf("%d points\n", (int)reductio.m_pcf.pointCount);

  int fps = 0;
  time_t last = clock();


  Point point;
  if(reductio.m_pcf.pointCount> 0)
    point = reductio.m_pcf.points[0];
  Camera::SetPosition(vec3(point.x, point.y, point.z));

  Controls::SetMouseLock(true);

  mat4 modelMat;
  mat4 projectionMat;
  projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, 1.0 / 4.0, 10000);

  while (Controls::Update())
  {
    window.Clear(0, 0, 0);

    Camera::Update(-150);

    mat4 viewMat = Camera::Matrix();
    mat4 MVP = projectionMat * viewMat * modelMat;

    //MVP.Transpose();

    //reductio.Render(MVP);
    reductio.Raytrace(viewMat, projectionMat);

    window.Swap();

    if (clock() - last > 1000)
    {
      last = clock();
      printf("%d fps\n", fps);

      fps = 0;
    }

    fps++;
  }


}
