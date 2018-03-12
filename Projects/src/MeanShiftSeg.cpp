#include "MeanShiftSeg.h"
#include "ImageFile.h"

const int KP = 2;
const double EPSILON = 0.01;
const double TC_DIST_FACTOR = 0.5;
const int LIMIT = 100;
const float LUV_THRESHOLD = 0.1f;
const int NODE_MULTIPLE = 10;
const double Xn = 0.95050;
const double Yn = 1.00000;
const double Zn = 1.08870;
const double Un_prime = 0.19784977571475;
const double Vn_prime = 0.46834507665248;
const double Lt = 0.008856;

const double LUV[3][3] = { { 0.4125,  0.3576,  0.1804 },
{ 0.2125,  0.7154,  0.0721 },
{ 0.0193,  0.1192,  0.9502 } };

vec3 RGBtoLUV(uint32_t rgb)
{
  uint8_t *pRGB = (uint8_t*)&rgb;
  double	x, y, z, L0, u_prime, v_prime, constant;
  x = LUV[0][0] * pRGB[0] + LUV[0][1] * pRGB[1] + LUV[0][2] * pRGB[2];
  y = LUV[1][0] * pRGB[0] + LUV[1][1] * pRGB[1] + LUV[1][2] * pRGB[2];
  z = LUV[2][0] * pRGB[0] + LUV[2][1] * pRGB[1] + LUV[2][2] * pRGB[2];
  L0 = y / (255.0 * Yn);
  vec3 ret;
  ret.x = L0 > Lt ? (float)(116.0 * (pow(L0, 1.0 / 3.0)) - 16.0) : (float)(903.3 * L0);
  constant = x + 15 * y + 3 * z;
  u_prime = constant ? (4 * x) / constant : 4.0;
  v_prime = constant ? (9 * y) / constant : 9.0 / 15.0;
  ret.y = (float)(13 * ret.x * (u_prime - Un_prime));
  ret.z = (float)(13 * ret.x * (v_prime - Vn_prime));
  return ret;
}

inline int my_round(double in_x) { return in_x < 0 ? (int)(in_x - 0.5) : (int)(in_x + 0.5); }

const double RGB[3][3] = { { 3.2405, -1.5371, -0.4985 },
{ -0.9693,  1.8760,  0.0416 },
{ 0.0556, -0.2040,  1.0573 } };

uint32_t LUVtoRGB(vec3 luv)
{
  int r, g, b;
  double	x, y, z, u_prime, v_prime;
  if (luv.x < 0.1)
    r = g = b = 0;
  else
  {
    if (luv.x < 8.0)
      y = Yn * luv.x / 903.3;
    else
    {
      y = (luv.x + 16.0) / 116.0;
      y *= Yn * y * y;
    }
    u_prime = luv.y / (13 * luv.x) + Un_prime;
    v_prime = luv.z / (13 * luv.x) + Vn_prime;
    x = 9 * u_prime * y / (4 * v_prime);
    z = (12 - 3 * u_prime - 20 * v_prime) * y / (4 * v_prime);
    r = my_round((RGB[0][0] * x + RGB[0][1] * y + RGB[0][2] * z)*255.0);
    g = my_round((RGB[1][0] * x + RGB[1][1] * y + RGB[1][2] * z)*255.0);
    b = my_round((RGB[2][0] * x + RGB[2][1] * y + RGB[2][2] * z)*255.0);
    if (r < 0)	r = 0; if (r > 255)	r = 255;
    if (g < 0)	g = 0; if (g > 255)	g = 255;
    if (b < 0)	b = 0; if (b > 255)	b = 255;
  }
  return r | (g << 8) | (b << 16) | (255 << 24);
}

float MeanShiftSeg::sigma_r_;
int MeanShiftSeg::sigma_s_;
int MeanShiftSeg::min_size_;
float MeanShiftSeg::speed_threshold_;

RgnAdjList::RgnAdjList()
{
  label = -1;
  next = nullptr;
  edge_strength = 0;
  edge_pixel_count = 0;
}

int RgnAdjList::Insert(RgnAdjList* entry)
{
  if (!next)
  {
    next = entry;
    entry->next = NULL;
    return 0;
  }
  if (next->label > entry->label)
  {
    entry->next = next;
    next = entry;
    return 0;
  }
  exists = 0;
  cur = next;
  while (cur)
  {
    if (entry->label == cur->label)
    {
      exists = 1;
      break;
    }
    else if ((!(cur->next)) || (cur->next->label > entry->label))
    {
      entry->next = cur->next;
      cur->next = entry;
      break;
    }
    cur = cur->next;
  }
  return (int)(exists);
}

MeanShiftSegmentationState::MeanShiftSegmentationState(int _width, int _height)
{
  h[0] = 1;
  h[1] = 1;
  width = _width;
  height = _height;
  int L = _width * _height;
  int N = 3;
  image = new vec3[width * height];
  regions = new int[width * height * N];
  modes.resize(L * (N + 2));
  mode_point_counts.resize(L);
  index_table.resize(L);
  mode_table.resize(L);
  point_list.resize(L);
  point_count = 0;
  rgn_count = 0;
}

RegionAdjacencyTable::RegionAdjacencyTable(const MeanShiftSegmentationState& state)
{
  int width = state.width;
  int height = state.height;
  const int* labels = state.regions;
  rgn_adj_list.resize(state.rgn_count);
  rgn_adj_pool.resize(NODE_MULTIPLE * state.rgn_count);
  int i;
  for (i = 0; i < state.rgn_count; i++)
  {
    rgn_adj_list[i].edge_strength = 0;
    rgn_adj_list[i].edge_pixel_count = 0;
    rgn_adj_list[i].label = i;
    rgn_adj_list[i].next = NULL;
  }
  free_rgn_adj_lists = &rgn_adj_pool[0];
  for (i = 0; i < NODE_MULTIPLE * state.rgn_count - 1; i++)
  {
    rgn_adj_pool[i].edge_strength = 0;
    rgn_adj_pool[i].edge_pixel_count = 0;
    rgn_adj_pool[i].next = &rgn_adj_pool[i + 1];
  }
  rgn_adj_pool[NODE_MULTIPLE*state.rgn_count - 1].next = NULL;
  int		j, curLabel, rightLabel, bottomLabel, exists;
  RgnAdjList	*raNode1, *raNode2, *oldRAFreeList;
  for (i = 0; i < height - 1; i++)
  {
    for (j = 0; j < width - 1; j++)
    {
      curLabel = labels[i*width + j];
      rightLabel = labels[i*width + j + 1];
      bottomLabel = labels[(i + 1)*width + j];
      if (curLabel != rightLabel)
      {
        raNode1 = free_rgn_adj_lists;
        raNode2 = free_rgn_adj_lists->next;
        oldRAFreeList = free_rgn_adj_lists;
        free_rgn_adj_lists = free_rgn_adj_lists->next->next;
        raNode1->label = curLabel;
        raNode2->label = rightLabel;
        exists = 0;
        rgn_adj_list[curLabel].Insert(raNode2);
        exists = rgn_adj_list[rightLabel].Insert(raNode1);
        if (exists)
          free_rgn_adj_lists = oldRAFreeList;
      }
      if (curLabel != bottomLabel)
      {
        raNode1 = free_rgn_adj_lists;
        raNode2 = free_rgn_adj_lists->next;
        oldRAFreeList = free_rgn_adj_lists;
        free_rgn_adj_lists = free_rgn_adj_lists->next->next;
        raNode1->label = curLabel;
        raNode2->label = bottomLabel;
        exists = 0;
        rgn_adj_list[curLabel].Insert(raNode2);
        exists = rgn_adj_list[bottomLabel].Insert(raNode1);
        if (exists)
          free_rgn_adj_lists = oldRAFreeList;
      }
    }
    curLabel = labels[i*width + j];
    bottomLabel = labels[(i + 1)*width + j];
    if (curLabel != bottomLabel)
    {
      raNode1 = free_rgn_adj_lists;
      raNode2 = free_rgn_adj_lists->next;
      oldRAFreeList = free_rgn_adj_lists;
      free_rgn_adj_lists = free_rgn_adj_lists->next->next;
      raNode1->label = curLabel;
      raNode2->label = bottomLabel;
      exists = 0;
      rgn_adj_list[curLabel].Insert(raNode2);
      exists = rgn_adj_list[bottomLabel].Insert(raNode1);
      if (exists)
        free_rgn_adj_lists = oldRAFreeList;
    }
  }
  for (j = 0; j < width - 1; j++)
  {
    curLabel = labels[i*width + j];
    rightLabel = labels[i*width + j + 1];
    if (curLabel != rightLabel)
    {
      raNode1 = free_rgn_adj_lists;
      raNode2 = free_rgn_adj_lists->next;
      oldRAFreeList = free_rgn_adj_lists;
      free_rgn_adj_lists = free_rgn_adj_lists->next->next;
      raNode1->label = curLabel;
      raNode2->label = rightLabel;
      exists = 0;
      rgn_adj_list[curLabel].Insert(raNode2);
      exists = rgn_adj_list[rightLabel].Insert(raNode1);
      if (exists)
        free_rgn_adj_lists = oldRAFreeList;
    }
  }
}

void MeanShiftSeg::FilterFast(vec3 *inp, MeanShiftSegmentationState& state)
{
  int width = state.width;
  int height = state.height;
  int N = 3;
  float sigma_s = static_cast<float>(sigma_s_);
  const float* data = inp->Data();
  float* msRawData = state.image[0].Data();
  int		iterationCount, i, j, k, modeCandidateX, modeCandidateY, modeCandidate_i;
  double	mvAbs, diff, el;
  int L = width * height;
  if (((state.h[0] = sigma_s) <= 0) || ((state.h[1] = sigma_r_) <= 0))
    throw std::exception("sigmaS and/or sigmaR is zero or negative.");
  int lN = N + 2;
  std::vector<double> yk(lN);
  std::vector<double> Mh(lN);
  std::vector<float> sdata(lN*L);
  int idxs, idxd;
  idxs = idxd = 0;
  if (N == 3)
  {
    for (i = 0; i < L; i++)
    {
      sdata[idxs++] = (i%width) / sigma_s;
      sdata[idxs++] = (i / width) / sigma_s;
      sdata[idxs++] = data[idxd++] / sigma_r_;
      sdata[idxs++] = data[idxd++] / sigma_r_;
      sdata[idxs++] = data[idxd++] / sigma_r_;
    }
  }
  else if (N == 1)
  {
    for (i = 0; i < L; i++)
    {
      sdata[idxs++] = (i%width) / sigma_s;
      sdata[idxs++] = (i / width) / sigma_s;
      sdata[idxs++] = data[idxd++] / sigma_r_;
    }
  }
  else
  {
    for (i = 0; i < L; i++)
    {
      sdata[idxs++] = (i%width) / sigma_s;
      sdata[idxs++] = (i / width) / sigma_s;
      for (j = 0; j < N; j++)
        sdata[idxs++] = data[idxd++] / sigma_r_;
    }
  }
  std::vector<int> slist(L);
  int bucNeigh[27];
  float sMins;
  float sMaxs[3];
  sMaxs[0] = width / sigma_s;
  sMaxs[1] = height / sigma_s;
  sMins = sMaxs[2] = sdata[2];
  idxs = 2;
  float cval;
  for (i = 0; i < L; i++)
  {
    cval = sdata[idxs];
    if (cval < sMins)
      sMins = cval;
    else if (cval > sMaxs[2])
      sMaxs[2] = cval;
    idxs += lN;
  }
  int nBuck1, nBuck2, nBuck3;
  int cBuck1, cBuck2, cBuck3, cBuck;
  nBuck1 = (int)(sMaxs[0] + 3);
  nBuck2 = (int)(sMaxs[1] + 3);
  nBuck3 = (int)(sMaxs[2] - sMins + 3);
  std::vector<int> buckets(nBuck1*nBuck2*nBuck3);
  for (i = 0; i < (nBuck1*nBuck2*nBuck3); i++)
    buckets[i] = -1;
  idxs = 0;
  for (i = 0; i < L; i++)
  {
    cBuck1 = (int)sdata[idxs] + 1;
    cBuck2 = (int)sdata[idxs + 1] + 1;
    cBuck3 = (int)(sdata[idxs + 2] - sMins) + 1;
    cBuck = cBuck1 + nBuck1*(cBuck2 + nBuck2*cBuck3);
    slist[i] = buckets[cBuck];
    buckets[cBuck] = i;
    idxs += lN;
  }
  idxd = 0;
  for (cBuck1 = -1; cBuck1 <= 1; cBuck1++)
  {
    for (cBuck2 = -1; cBuck2 <= 1; cBuck2++)
    {
      for (cBuck3 = -1; cBuck3 <= 1; cBuck3++)
      {
        bucNeigh[idxd++] = cBuck1 + nBuck1*(cBuck2 + nBuck2*cBuck3);
      }
    }
  }
  double wsuml, weight;
  double hiLTr = 80.0 / sigma_r_;
  memset(&(state.mode_table[0]), 0, width*height);
  for (i = 0; i < L; i++)
  {
    if (state.mode_table[i] == 1)
      continue;
    state.point_count = 0;
    idxs = i*lN;
    for (j = 0; j < lN; j++)
      yk[j] = sdata[idxs + j];
    for (j = 0; j < lN; j++)
      Mh[j] = 0;
    wsuml = 0;
    cBuck1 = (int)yk[0] + 1;
    cBuck2 = (int)yk[1] + 1;
    cBuck3 = (int)(yk[2] - sMins) + 1;
    cBuck = cBuck1 + nBuck1*(cBuck2 + nBuck2*cBuck3);
    for (j = 0; j < 27; j++)
    {
      idxd = buckets[cBuck + bucNeigh[j]];
      while (idxd >= 0)
      {
        idxs = lN*idxd;
        el = sdata[idxs + 0] - yk[0];
        diff = el*el;
        el = sdata[idxs + 1] - yk[1];
        diff += el*el;
        if (diff < 1.0)
        {
          el = sdata[idxs + 2] - yk[2];
          if (yk[2] > hiLTr)
            diff = 4 * el*el;
          else
            diff = el*el;
          if (N > 1)
          {
            el = sdata[idxs + 3] - yk[3];
            diff += el*el;
            el = sdata[idxs + 4] - yk[4];
            diff += el*el;
          }
          if (diff < 1.0)
          {
            weight = 1.0;
            for (k = 0; k < lN; k++)
              Mh[k] += weight*sdata[idxs + k];
            wsuml += weight;
            if (diff < speed_threshold_)
            {
              if (state.mode_table[idxd] == 0)
              {
                state.point_list[state.point_count++] = idxd;
                state.mode_table[idxd] = 2;
              }
            }
          }
        }
        idxd = slist[idxd];
      }
    }
    if (wsuml > 0)
    {
      for (j = 0; j < lN; j++)
        Mh[j] = Mh[j] / wsuml - yk[j];
    }
    else
    {
      for (j = 0; j < lN; j++)
        Mh[j] = 0;
    }
    mvAbs = (Mh[0] * Mh[0] + Mh[1] * Mh[1])*sigma_s_*sigma_s_;
    if (N == 3)
      mvAbs += (Mh[2] * Mh[2] + Mh[3] * Mh[3] + Mh[4] * Mh[4])*sigma_r_*sigma_r_;
    else
      mvAbs += Mh[2] * Mh[2] * sigma_r_*sigma_r_;
    iterationCount = 1;
    while ((mvAbs >= EPSILON) && (iterationCount < LIMIT))
    {
      for (j = 0; j < lN; j++)
        yk[j] += Mh[j];
      modeCandidateX = (int)(sigma_s_*yk[0] + 0.5);
      modeCandidateY = (int)(sigma_s_*yk[1] + 0.5);
      modeCandidate_i = modeCandidateY*width + modeCandidateX;
      if ((state.mode_table[modeCandidate_i] != 2) && (modeCandidate_i != i))
      {
        diff = 0;
        idxs = lN*modeCandidate_i;
        for (k = 2; k < lN; k++)
        {
          el = sdata[idxs + k] - yk[k];
          diff += el*el;
        }
        if (diff < speed_threshold_)
        {
          if (state.mode_table[modeCandidate_i] == 0)
          {
            state.point_list[state.point_count++] = modeCandidate_i;
            state.mode_table[modeCandidate_i] = 2;
          }
          else
          {
            for (j = 0; j < N; j++)
              yk[j + 2] = msRawData[modeCandidate_i*N + j] / sigma_r_;
            state.mode_table[i] = 1;
            mvAbs = -1;
            break;
          }
        }
      }
      for (j = 0; j < lN; j++)
        Mh[j] = 0;
      wsuml = 0;
      cBuck1 = (int)yk[0] + 1;
      cBuck2 = (int)yk[1] + 1;
      cBuck3 = (int)(yk[2] - sMins) + 1;
      cBuck = cBuck1 + nBuck1*(cBuck2 + nBuck2*cBuck3);
      for (j = 0; j < 27; j++)
      {
        idxd = buckets[cBuck + bucNeigh[j]];
        while (idxd >= 0)
        {
          idxs = lN*idxd;
          el = sdata[idxs + 0] - yk[0];
          diff = el*el;
          el = sdata[idxs + 1] - yk[1];
          diff += el*el;
          if (diff < 1.0)
          {
            el = sdata[idxs + 2] - yk[2];
            if (yk[2] > hiLTr)
              diff = 4 * el*el;
            else
              diff = el*el;
            if (N > 1)
            {
              el = sdata[idxs + 3] - yk[3];
              diff += el*el;
              el = sdata[idxs + 4] - yk[4];
              diff += el*el;
            }

            if (diff < 1.0)
            {
              weight = 1;
              for (k = 0; k < lN; k++)
                Mh[k] += weight*sdata[idxs + k];
              wsuml += weight;
              if (diff < speed_threshold_)
              {
                if (state.mode_table[idxd] == 0)
                {
                  state.point_list[state.point_count++] = idxd;
                  state.mode_table[idxd] = 2;
                }
              }
            }
          }
          idxd = slist[idxd];
        }
      }
      if (wsuml > 0)
      {
        for (j = 0; j < lN; j++)
          Mh[j] = Mh[j] / wsuml - yk[j];
      }
      else
      {
        for (j = 0; j < lN; j++)
          Mh[j] = 0;
      }
      mvAbs = (Mh[0] * Mh[0] + Mh[1] * Mh[1])*sigma_s_*sigma_s_;
      if (N == 3)
        mvAbs += (Mh[2] * Mh[2] + Mh[3] * Mh[3] + Mh[4] * Mh[4])*sigma_r_*sigma_r_;
      else
        mvAbs += Mh[2] * Mh[2] * sigma_r_*sigma_r_;
      iterationCount++;
    }
    if (mvAbs >= 0)
    {
      for (j = 0; j < lN; j++)
        yk[j] += Mh[j];
      state.mode_table[i] = 1;
    }

    for (k = 0; k < N; k++)
      yk[k + 2] *= sigma_r_;
    for (j = 0; j < state.point_count; j++)
    {
      modeCandidate_i = state.point_list[j];
      state.mode_table[modeCandidate_i] = 1;
      for (k = 0; k < N; k++)
        msRawData[N*modeCandidate_i + k] = (float)(yk[k + 2]);
    }
    for (j = 0; j < N; j++)
      msRawData[N*i + j] = (float)(yk[j + 2]);
  }
}

void MeanShiftSeg::FilterSlow(vec3 *inp, MeanShiftSegmentationState& state)
{
  int N = 3;
  int		iterationCount, i, j, k, modeCandidateX, modeCandidateY, modeCandidate_i;
  double	mvAbs, diff, el;
  int width = state.width;
  int height = state.height;
  int L = height * width;
  if (((state.h[0] = static_cast<float>(sigma_s_)) <= 0) || ((state.h[1] = sigma_r_) <= 0))
    throw std::exception("sigmaS and/or sigmaR is zero or negative.");
  int lN = N + 2;
  std::vector<double> yk(lN);
  std::vector<double> Mh(lN);
  const float* data = inp->Data();
  float* msRawData = state.image[0].Data();
  std::vector<float> sdata(lN*L);
  int idxs, idxd;
  idxs = idxd = 0;
  if (N == 3)
  {
    for (i = 0; i < L; i++)
    {
      sdata[idxs++] = (i%width) / static_cast<float>(sigma_s_);
      sdata[idxs++] = (i / width) / static_cast<float>(sigma_s_);
      sdata[idxs++] = data[idxd++] / sigma_r_;
      sdata[idxs++] = data[idxd++] / sigma_r_;
      sdata[idxs++] = data[idxd++] / sigma_r_;
    }
  }
  else if (N == 1)
  {
    for (i = 0; i < L; i++)
    {
      sdata[idxs++] = (i%width) / static_cast<float>(sigma_s_);
      sdata[idxs++] = (i / width) / static_cast<float>(sigma_s_);
      sdata[idxs++] = data[idxd++] / sigma_r_;
    }
  }
  else
  {
    for (i = 0; i < L; i++)
    {
      sdata[idxs++] = (i%width) / static_cast<float>(sigma_s_);
      sdata[idxs++] = (i / width) / static_cast<float>(sigma_s_);
      for (j = 0; j < N; j++)
        sdata[idxs++] = data[idxd++] / sigma_r_;
    }
  }
  std::vector<int> slist(L);
  int bucNeigh[27];
  float sMins;
  float sMaxs[3];
  sMaxs[0] = width / static_cast<float>(sigma_s_);
  sMaxs[1] = height / static_cast<float>(sigma_s_);
  sMins = sMaxs[2] = sdata[2];
  idxs = 2;
  float cval;
  for (i = 0; i < L; i++)
  {
    cval = sdata[idxs];
    if (cval < sMins)
      sMins = cval;
    else if (cval > sMaxs[2])
      sMaxs[2] = cval;

    idxs += lN;
  }
  int nBuck1, nBuck2, nBuck3;
  int cBuck1, cBuck2, cBuck3, cBuck;
  nBuck1 = (int)(sMaxs[0] + 3);
  nBuck2 = (int)(sMaxs[1] + 3);
  nBuck3 = (int)(sMaxs[2] - sMins + 3);
  std::vector<int> buckets(nBuck1*nBuck2*nBuck3);
  for (i = 0; i < (nBuck1*nBuck2*nBuck3); i++)
    buckets[i] = -1;
  idxs = 0;
  for (i = 0; i < L; i++)
  {
    cBuck1 = (int)sdata[idxs] + 1;
    cBuck2 = (int)sdata[idxs + 1] + 1;
    cBuck3 = (int)(sdata[idxs + 2] - sMins) + 1;
    cBuck = cBuck1 + nBuck1*(cBuck2 + nBuck2*cBuck3);

    slist[i] = buckets[cBuck];
    buckets[cBuck] = i;

    idxs += lN;
  }
  idxd = 0;
  for (cBuck1 = -1; cBuck1 <= 1; cBuck1++)
  {
    for (cBuck2 = -1; cBuck2 <= 1; cBuck2++)
    {
      for (cBuck3 = -1; cBuck3 <= 1; cBuck3++)
      {
        bucNeigh[idxd++] = cBuck1 + nBuck1*(cBuck2 + nBuck2*cBuck3);
      }
    }
  }
  double wsuml;
  double hiLTr = 80.0 / sigma_r_;
  memset(&(state.mode_table[0]), 0, width*height);
  for (i = 0; i < L; i++)
  {
    if (state.mode_table[i] == 1)
      continue;
    state.point_count = 0;
    idxs = i*lN;
    for (j = 0; j < lN; j++)
      yk[j] = sdata[idxs + j];
    for (j = 0; j < lN; j++)
      Mh[j] = 0;
    wsuml = 0;
    cBuck1 = (int)yk[0] + 1;
    cBuck2 = (int)yk[1] + 1;
    cBuck3 = (int)(yk[2] - sMins) + 1;
    cBuck = cBuck1 + nBuck1*(cBuck2 + nBuck2*cBuck3);
    for (j = 0; j < 27; j++)
    {
      idxd = buckets[cBuck + bucNeigh[j]];
      while (idxd >= 0)
      {
        idxs = lN*idxd;
        el = sdata[idxs + 0] - yk[0];
        diff = el*el;
        el = sdata[idxs + 1] - yk[1];
        diff += el*el;
        if (diff < 1.0)
        {
          el = sdata[idxs + 2] - yk[2];
          if (yk[2] > hiLTr)
            diff = 4 * el*el;
          else
            diff = el*el;
          if (N > 1)
          {
            el = sdata[idxs + 3] - yk[3];
            diff += el*el;
            el = sdata[idxs + 4] - yk[4];
            diff += el*el;
          }
          if (diff < 1.0)
          {
            double weight = 1.0;
            for (k = 0; k < lN; k++)
              Mh[k] += weight*sdata[idxs + k];
            wsuml += weight;
          }
        }
        idxd = slist[idxd];
      }
    }
    if (wsuml > 0)
    {
      for (j = 0; j < lN; j++)
        Mh[j] = Mh[j] / wsuml - yk[j];
    }
    else
    {
      for (j = 0; j < lN; j++)
        Mh[j] = 0;
    }
    mvAbs = (Mh[0] * Mh[0] + Mh[1] * Mh[1])*sigma_s_*sigma_s_;
    if (N == 3)
      mvAbs += (Mh[2] * Mh[2] + Mh[3] * Mh[3] + Mh[4] * Mh[4])*sigma_r_*sigma_r_;
    else
      mvAbs += Mh[2] * Mh[2] * sigma_r_*sigma_r_;
    iterationCount = 1;
    while ((mvAbs >= EPSILON) && (iterationCount < LIMIT))
    {
      for (j = 0; j < lN; j++)
        yk[j] += Mh[j];
      modeCandidateX = (int)(sigma_s_*yk[0] + 0.5);
      modeCandidateY = (int)(sigma_s_*yk[1] + 0.5);
      modeCandidate_i = modeCandidateY*width + modeCandidateX;
      if ((state.mode_table[modeCandidate_i] != 2) && (modeCandidate_i != i))
      {
        diff = 0;
        idxs = lN*modeCandidate_i;
        for (k = 2; k < lN; k++)
        {
          el = sdata[idxs + k] - yk[k];
          diff += el*el;
        }
        if (diff < TC_DIST_FACTOR)
        {
          if (state.mode_table[modeCandidate_i] == 0)
          {
            state.point_list[state.point_count++] = modeCandidate_i;
            state.mode_table[modeCandidate_i] = 2;
          }
          else
          {
            for (j = 0; j < N; j++)
              yk[j + 2] = msRawData[modeCandidate_i*N + j] / sigma_r_;
            state.mode_table[i] = 1;
            mvAbs = -1;
            break;
          }
        }
      }
      for (j = 0; j < lN; j++)
        Mh[j] = 0;
      wsuml = 0;
      cBuck1 = (int)yk[0] + 1;
      cBuck2 = (int)yk[1] + 1;
      cBuck3 = (int)(yk[2] - sMins) + 1;
      cBuck = cBuck1 + nBuck1*(cBuck2 + nBuck2*cBuck3);
      for (j = 0; j < 27; j++)
      {
        idxd = buckets[cBuck + bucNeigh[j]];
        while (idxd >= 0)
        {
          idxs = lN*idxd;
          el = sdata[idxs + 0] - yk[0];
          diff = el*el;
          el = sdata[idxs + 1] - yk[1];
          diff += el*el;
          if (diff < 1.0)
          {
            el = sdata[idxs + 2] - yk[2];
            if (yk[2] > hiLTr)
              diff = 4 * el*el;
            else
              diff = el*el;
            if (N > 1)
            {
              el = sdata[idxs + 3] - yk[3];
              diff += el*el;
              el = sdata[idxs + 4] - yk[4];
              diff += el*el;
            }
            if (diff < 1.0)
            {
              double weight = 1.0;
              for (k = 0; k < lN; k++)
                Mh[k] += weight*sdata[idxs + k];
              wsuml += weight;
            }
          }
          idxd = slist[idxd];
        }
      }
      if (wsuml > 0)
      {
        for (j = 0; j < lN; j++)
          Mh[j] = Mh[j] / wsuml - yk[j];
      }
      else
      {
        for (j = 0; j < lN; j++)
          Mh[j] = 0;
      }
      mvAbs = (Mh[0] * Mh[0] + Mh[1] * Mh[1])*sigma_s_*sigma_s_;
      if (N == 3)
        mvAbs += (Mh[2] * Mh[2] + Mh[3] * Mh[3] + Mh[4] * Mh[4])*sigma_r_*sigma_r_;
      else
        mvAbs += Mh[2] * Mh[2] * sigma_r_*sigma_r_;
      iterationCount++;
    }
    if (mvAbs >= 0)
    {
      for (j = 0; j < lN; j++)
        yk[j] += Mh[j];
      state.mode_table[i] = 1;
    }
    for (k = 0; k < N; k++)
      yk[k + 2] *= sigma_r_;
    for (j = 0; j < state.point_count; j++)
    {
      modeCandidate_i = state.point_list[j];
      state.mode_table[modeCandidate_i] = 1;
      for (k = 0; k < N; k++)
        msRawData[N * modeCandidate_i + k] = (float)(yk[k + 2]);
    }
    for (j = 0; j < N; j++)
      msRawData[N * i + j] = (float)(yk[j + 2]);
  }
}

void MeanShiftSeg::Fill(int regionLoc, int label, int neigh[], MeanShiftSegmentationState& state)
{
  float* LUV_data = state.image[0].Data();
  int* labels = state.regions;
  int width = state.width;
  int height = state.height;
  int N = 3;
  int	i, k, neighLoc, neighborsFound, imageSize = width*height;
  int	index = 0;
  state.index_table[0] = regionLoc;
  state.mode_point_counts[label]++;
  while (true)
  {
    neighborsFound = 0;
    for (i = 0; i < 8; i++)
    {
      neighLoc = regionLoc + neigh[i];
      if ((neighLoc >= 0) && (neighLoc < imageSize) && (labels[neighLoc] < 0))
      {
        for (k = 0; k < N; k++)
        {
          if (fabs(LUV_data[(regionLoc*N) + k] - LUV_data[(neighLoc*N) + k]) >= LUV_THRESHOLD)
            break;
        }
        if (k == N)
        {
          labels[neighLoc] = label;
          state.mode_point_counts[label]++;
          state.index_table[++index] = neighLoc;
          neighborsFound = 1;
        }
      }
    }
    if (neighborsFound)
      regionLoc = state.index_table[index];
    else if (index > 1)
      regionLoc = state.index_table[--index];
    else
      break;
  }
}

void MeanShiftSeg::Connect(MeanShiftSegmentationState& state)
{
  int width = state.width;
  int height = state.height;
  int* labels = state.regions;
  float* LUV_data = state.image[0].Data();
  int N = 3;
  int neigh[8];
  neigh[0] = 1;
  neigh[1] = 1 - width;
  neigh[2] = -width;
  neigh[3] = -(1 + width);
  neigh[4] = -1;
  neigh[5] = width - 1;
  neigh[6] = width;
  neigh[7] = width + 1;
  int i;
  for (i = 0; i < width*height; i++)
  {
    labels[i] = -1;
    state.mode_point_counts[i] = 0;
  }
  int k, label = -1;
  for (i = 0; i < height*width; i++)
  {
    if (labels[i] < 0)
    {
      labels[i] = ++label;
      for (k = 0; k < N; k++)
        state.modes[(N*label) + k] = LUV_data[(N*i) + k];
      Fill(i, label, neigh, state);
    }
  }
  state.rgn_count = label + 1;
}

bool MeanShiftSeg::InWindow(const MeanShiftSegmentationState& state, int mode1, int mode2)
{
  int		k = 1, s = 0, p;
  int N = 3;
  int P[2] = { 2, N };
  double	diff = 0, el;
  while ((diff < 0.25) && (k != KP))
  {
    diff = 0;
    for (p = 0; p < P[k]; p++)
    {
      el = (state.modes[mode1*N + p + s] - state.modes[mode2*N + p + s]) / (state.h[k]);
      if ((!p) && (k == 1) && (state.modes[mode1*N] > 80))
        diff += 4 * el*el;
      else
        diff += el*el;
    }
    s += P[k];
    k++;
  }
  return (bool)(diff < 0.25);
}

void MeanShiftSeg::TransitiveClosure(MeanShiftSegmentationState& state)
{
  int width = state.width;
  int height = state.height;
  int N = 3;
  const float epsilon = 1.0f;
  RegionAdjacencyTable ram(state);
  int		i, iCanEl, neighCanEl;
  float	threshold;
  RgnAdjList	*neighbor;
  for (i = 0; i < state.rgn_count; i++)
  {
    neighbor = ram.rgn_adj_list[i].next;
    if (epsilon > ram.rgn_adj_list[i].edge_strength)
      threshold = epsilon;
    else
      threshold = ram.rgn_adj_list[i].edge_strength;
    while (neighbor)
    {
      if ((InWindow(state, i, neighbor->label)) && (neighbor->edge_strength < epsilon))
      {
        iCanEl = i;
        while (ram.rgn_adj_list[iCanEl].label != iCanEl)
          iCanEl = ram.rgn_adj_list[iCanEl].label;
        neighCanEl = neighbor->label;
        while (ram.rgn_adj_list[neighCanEl].label != neighCanEl)
          neighCanEl = ram.rgn_adj_list[neighCanEl].label;
        if (iCanEl < neighCanEl)
          ram.rgn_adj_list[neighCanEl].label = iCanEl;
        else
        {
          ram.rgn_adj_list[ram.rgn_adj_list[iCanEl].label].label = neighCanEl;
          ram.rgn_adj_list[iCanEl].label = neighCanEl;
        }
      }
      neighbor = neighbor->next;
    }
  }
  for (i = 0; i < state.rgn_count; i++)
  {
    iCanEl = i;
    while (ram.rgn_adj_list[iCanEl].label != iCanEl)
      iCanEl = ram.rgn_adj_list[iCanEl].label;
    ram.rgn_adj_list[i].label = iCanEl;
  }
  std::vector<float> modes_buffer(N*state.rgn_count);
  std::vector<int> MPC_buffer(state.rgn_count);
  for (i = 0; i < state.rgn_count; i++)
    MPC_buffer[i] = 0;
  for (i = 0; i < N * state.rgn_count; i++)
    modes_buffer[i] = 0;
  int k, iMPC;
  for (i = 0; i < state.rgn_count; i++)
  {
    iCanEl = ram.rgn_adj_list[i].label;
    iMPC = state.mode_point_counts[i];
    for (k = 0; k < N; k++)
      modes_buffer[(N*iCanEl) + k] += iMPC * state.modes[(N*i) + k];
    MPC_buffer[iCanEl] += iMPC;
  }
  std::vector<int> label_buffer(state.rgn_count);
  for (i = 0; i < state.rgn_count; i++)
    label_buffer[i] = -1;
  int	label = -1;
  for (i = 0; i < state.rgn_count; i++)
  {
    iCanEl = ram.rgn_adj_list[i].label;
    if (label_buffer[iCanEl] < 0)
    {
      label_buffer[iCanEl] = ++label;
      iMPC = MPC_buffer[iCanEl];
      for (k = 0; k < N; k++)
        state.modes[(N*label) + k] = (modes_buffer[(N*iCanEl) + k]) / (iMPC);
      state.mode_point_counts[label] = MPC_buffer[iCanEl];
    }
  }
  int	oldRegionCount = state.rgn_count;
  state.rgn_count = label + 1;
  int* labels = state.regions;
  for (i = 0; i < height*width; i++)
    labels[i] = label_buffer[ram.rgn_adj_list[labels[i]].label];
}

float MeanShiftSeg::SqDistance(const MeanShiftSegmentationState& state, int mode1, int mode2)
{
  int N = 3;
  int		k = 1, s = 0, p;
  float	dist = 0, el;
  int P[2] = { 2, N };
  for (k = 1; k < KP; k++)
  {
    for (p = 0; p < P[k]; p++)
    {
      el = (state.modes[mode1*N + p + s] - state.modes[mode2*N + p + s]) / (state.h[k]);
      dist += el*el;
    }
    s += P[k];
    k++;
  }
  return dist;
}

void MeanShiftSeg::Prune(MeanShiftSegmentationState& state)
{
  int N = 3;
  std::vector<float> modes_buffer(N*state.rgn_count);
  std::vector<int> MPC_buffer(state.rgn_count);
  std::vector<int> label_buffer(state.rgn_count);

  int	i, k, candidate, iCanEl, neighCanEl, iMPC, label, oldRegionCount, minRegionCount;
  double	minSqDistance, neighborDistance;
  RgnAdjList	*neighbor;

  do
  {
    minRegionCount = 0;

    RegionAdjacencyTable ram(state);
    for (i = 0; i < state.rgn_count; i++)
    {
      if (state.mode_point_counts[i] < min_size_)
      {
        minRegionCount++;
        neighbor = ram.rgn_adj_list[i].next;
        candidate = neighbor->label;
        minSqDistance = SqDistance(state, i, candidate);
        neighbor = neighbor->next;
        while (neighbor)
        {
          neighborDistance = SqDistance(state, i, neighbor->label);
          if (neighborDistance < minSqDistance)
          {
            minSqDistance = neighborDistance;
            candidate = neighbor->label;
          }
          neighbor = neighbor->next;
        }
        iCanEl = i;
        while (ram.rgn_adj_list[iCanEl].label != iCanEl)
          iCanEl = ram.rgn_adj_list[iCanEl].label;
        neighCanEl = candidate;
        while (ram.rgn_adj_list[neighCanEl].label != neighCanEl)
          neighCanEl = ram.rgn_adj_list[neighCanEl].label;
        if (iCanEl < neighCanEl)
          ram.rgn_adj_list[neighCanEl].label = iCanEl;
        else
        {
          ram.rgn_adj_list[ram.rgn_adj_list[iCanEl].label].label = neighCanEl;
          ram.rgn_adj_list[iCanEl].label = neighCanEl;
        }
      }
    }
    for (i = 0; i < state.rgn_count; i++)
    {
      iCanEl = i;
      while (ram.rgn_adj_list[iCanEl].label != iCanEl)
        iCanEl = ram.rgn_adj_list[iCanEl].label;
      ram.rgn_adj_list[i].label = iCanEl;
    }
    for (i = 0; i < state.rgn_count; i++)
      MPC_buffer[i] = 0;
    for (i = 0; i < N*state.rgn_count; i++)
      modes_buffer[i] = 0;
    for (i = 0; i < state.rgn_count; i++)
    {
      iCanEl = ram.rgn_adj_list[i].label;
      iMPC = state.mode_point_counts[i];
      for (k = 0; k < N; k++)
        modes_buffer[(N*iCanEl) + k] += iMPC*state.modes[(N*i) + k];
      MPC_buffer[iCanEl] += iMPC;
    }
    for (i = 0; i < state.rgn_count; i++)
      label_buffer[i] = -1;
    label = -1;
    for (i = 0; i < state.rgn_count; i++)
    {
      iCanEl = ram.rgn_adj_list[i].label;
      if (label_buffer[iCanEl] < 0)
      {
        label_buffer[iCanEl] = ++label;
        iMPC = MPC_buffer[iCanEl];
        for (k = 0; k < N; k++)
          state.modes[(N*label) + k] = (modes_buffer[(N*iCanEl) + k]) / (iMPC);
        state.mode_point_counts[label] = MPC_buffer[iCanEl];
      }
    }
    oldRegionCount = state.rgn_count;
    state.rgn_count = label + 1;
    int width = state.width;
    int height = state.height;
    int* labels = state.regions;
    for (i = 0; i < height*width; i++)
      labels[i] = label_buffer[ram.rgn_adj_list[labels[i]].label];

  } while (minRegionCount > 0);
}

void MeanShiftSeg::FuseRegions(MeanShiftSegmentationState& state)
{
  int width = state.width;
  int height = state.height;
  int L = width * height;
  int N = 3;
  float sigmaS = sigma_r_;
  if ((state.h[1] = sigmaS) <= 0)
    throw std::exception("FuseRegions : The feature radius must be greater than or equal to zero.");
  std::vector<uint8_t> visitTable(L);
  TransitiveClosure(state);
  int oldRC = state.rgn_count;
  int deltaRC, counter = 0;
  do
  {
    TransitiveClosure(state);
    deltaRC = oldRC - state.rgn_count;
    oldRC = state.rgn_count;
    counter++;
  } while ((deltaRC <= 0) && (counter < 10));
  Prune(state);
  const int* labels = state.regions;
  float* msRawData = state.image[0].Data();
  int i, j, label;
  for (i = 0; i < L; i++)
  {
    label = labels[i];
    for (j = 0; j < N; j++)
      msRawData[N*i + j] = state.modes[N*label + j];
  }
}

void MeanShiftSeg::ApplySegmentation(std::string input, std::string output, int sigmaS /*= 7*/, float sigmaR /*= 6.5*/, int minRegionSize /*= 20*/, bool fastFilter /*= false*/)
{
  printf("Loading\n");
  uint32_t imageWidth, imageHeight;
  uint32_t *rgbImage = ImageFile::ReadImage(input.c_str(), &imageWidth, &imageHeight);
  ApplySegmentation(rgbImage, imageWidth, imageHeight, rgbImage, sigma_s_, sigmaR, minRegionSize, fastFilter);
  ImageFile::WriteImagePNG(output.c_str(), rgbImage, imageWidth, imageHeight);
  delete rgbImage;
}

void MeanShiftSeg::ApplySegmentation(uint32_t *pInput, int width, int height, uint32_t *pOutput, int sigmaS /*= 7*/, float sigmaR /*= 6.5*/, int minRegionSize, bool fastFilter /*= false*/)
{
  sigma_r_ = sigmaR;
  sigma_s_ = sigmaS;
  min_size_ = minRegionSize;
  speed_threshold_ = 0.5f;
  vec3 *luvInput = new vec3[width * height];
  for (int y = 0; y < height; y++) for (int x = 0; x < width; x++) luvInput[x + y * width] = RGBtoLUV(pInput[x + y * width]);
  MeanShiftSegmentationState state(width, height);
  printf("Filtering\n");
  if (fastFilter)
    FilterFast(luvInput, state);
  else
    FilterSlow(luvInput, state);
  printf("Connecting\n");
  Connect(state);
  printf("Fusing\n");
  FuseRegions(state);
  printf("Saving\n");
  for (int y = 0; y < height; y++) for (int x = 0; x < width; x++) pOutput[x + y * width] = LUVtoRGB(state.image[x + y * width]);
  delete[] luvInput;
}
