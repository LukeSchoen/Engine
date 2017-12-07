#include "Biped.h"
#include "CustomRenderObjects.h"
#include "AlphaRenderObject.h"
#include "RenderObject.h"
#include "Camera.h"

static RenderObject *Head;
static AlphaRenderObject *Hat;
static RenderObject *Body;
static AlphaRenderObject *Jacket;
static RenderObject *ArmR;
static AlphaRenderObject *ArmR2;
static RenderObject *ArmL;
static AlphaRenderObject *ArmL2;
static RenderObject *LegR;
static AlphaRenderObject *LegR2;
static RenderObject *LegL;
static AlphaRenderObject *LegL2;

void _AddQuad(TexturedRenderObjectMaker *maker, vec3 p1, vec3 p2, vec3 p3, vec3 p4, vec2 uv1, vec2 uv2, vec2 uv3, vec2 uv4)
{
  maker->AddVertex(p1, uv1);
  maker->AddVertex(p2, uv2);
  maker->AddVertex(p4, uv4);
  maker->AddVertex(p2, uv2);
  maker->AddVertex(p3, uv3);
  maker->AddVertex(p4, uv4);
}

RenderObject* _CreateBox(vec3i dims, vec2i toff, bool SwapLeftWithBack = false, float boxWidth = 64, float worldScale = 32)
{
  TexturedRenderObjectMaker maker;

  // Top
  vec2i ltoff(dims.z, 0);
  _AddQuad
  (
    &maker,
    vec3(-dims.x / worldScale, dims.y / worldScale, -dims.z / worldScale),
    vec3(dims.x / worldScale, dims.y / worldScale, -dims.z / worldScale),
    vec3(dims.x / worldScale, dims.y / worldScale, dims.z / worldScale),
    vec3(-dims.x / worldScale, dims.y / worldScale, dims.z / worldScale),
    vec2((toff + ltoff).x / boxWidth, (toff + ltoff).y / boxWidth),
    vec2(((toff + ltoff).x + dims.x) / boxWidth, (toff + ltoff).y / boxWidth),
    vec2(((toff + ltoff).x + dims.x) / boxWidth, ((toff + ltoff).y + dims.z) / boxWidth),
    vec2((toff + ltoff).x / boxWidth, ((toff + ltoff).y + dims.z) / boxWidth)
  );

  // Bot
  ltoff = vec2i(dims.z + dims.x, 0);
  _AddQuad
  (
    &maker,
    vec3(-dims.x / worldScale, -dims.y / worldScale, -dims.z / worldScale),
    vec3(dims.x / worldScale, -dims.y / worldScale, -dims.z / worldScale),
    vec3(dims.x / worldScale, -dims.y / worldScale, dims.z / worldScale),
    vec3(-dims.x / worldScale, -dims.y / worldScale, dims.z / worldScale),
    vec2((toff + ltoff).x / boxWidth, (toff + ltoff).y / boxWidth),
    vec2(((toff + ltoff).x + dims.x) / boxWidth, (toff + ltoff).y / boxWidth),
    vec2(((toff + ltoff).x + dims.x) / boxWidth, ((toff + ltoff).y + dims.z) / boxWidth),
    vec2((toff + ltoff).x / boxWidth, ((toff + ltoff).y + dims.z) / boxWidth)
  );

  // Left
  ltoff = vec2i(0, dims.z);
  _AddQuad
  (
    &maker,
    vec3(-dims.x / worldScale, -dims.y / worldScale, -dims.z / worldScale),
    vec3(-dims.x / worldScale, dims.y / worldScale, -dims.z / worldScale),
    vec3(-dims.x / worldScale, dims.y / worldScale, dims.z / worldScale),
    vec3(-dims.x / worldScale, -dims.y / worldScale, dims.z / worldScale),
    vec2((toff + ltoff).x / boxWidth, ((toff + ltoff).y + dims.y) / boxWidth),
    vec2((toff + ltoff).x / boxWidth, (toff + ltoff).y / boxWidth),
    vec2(((toff + ltoff).x + dims.z) / boxWidth, (toff + ltoff).y / boxWidth),
    vec2(((toff + ltoff).x + dims.z) / boxWidth, ((toff + ltoff).y + dims.y) / boxWidth)
  );

  // Right
  ltoff = vec2i(dims.z + dims.x + (SwapLeftWithBack ? dims.x : 0), dims.z);
  _AddQuad
  (
    &maker,
    vec3(dims.x / worldScale, -dims.y / worldScale, dims.z / worldScale),
    vec3(dims.x / worldScale, dims.y / worldScale, dims.z / worldScale),
    vec3(dims.x / worldScale, dims.y / worldScale, -dims.z / worldScale),
    vec3(dims.x / worldScale, -dims.y / worldScale, -dims.z / worldScale),
    vec2((toff + ltoff).x / boxWidth, ((toff + ltoff).y + dims.y) / boxWidth),
    vec2((toff + ltoff).x / boxWidth, (toff + ltoff).y / boxWidth),
    vec2(((toff + ltoff).x + dims.z) / boxWidth, (toff + ltoff).y / boxWidth),
    vec2(((toff + ltoff).x + dims.z) / boxWidth, ((toff + ltoff).y + dims.y) / boxWidth)
  );

  // Front
  ltoff = vec2i(dims.z, dims.z);
  _AddQuad
  (
    &maker,
    vec3(-dims.x / worldScale, dims.y / worldScale, dims.z / worldScale),
    vec3(dims.x / worldScale, dims.y / worldScale, dims.z / worldScale),
    vec3(dims.x / worldScale, -dims.y / worldScale, dims.z / worldScale),
    vec3(-dims.x / worldScale, -dims.y / worldScale, dims.z / worldScale),
    vec2((toff + ltoff).x / boxWidth, (toff + ltoff).y / boxWidth),
    vec2(((toff + ltoff).x + dims.x) / boxWidth, (toff + ltoff).y / boxWidth),
    vec2(((toff + ltoff).x + dims.x) / boxWidth, ((toff + ltoff).y + dims.y) / boxWidth),
    vec2((toff + ltoff).x / boxWidth, ((toff + ltoff).y + dims.y) / boxWidth)
  );

  // Back
  //ltoff = vec2i(dims.z + dims.x + (SwapLeftWithBack ? 0 : dims.z), dims.z);
  ltoff = vec2i(dims.z + dims.x + dims.z, dims.z);
  _AddQuad
  (
    &maker,
    vec3(-dims.x / worldScale, dims.y / worldScale, -dims.z / worldScale),
    vec3(dims.x / worldScale, dims.y / worldScale, -dims.z / worldScale),
    vec3(dims.x / worldScale, -dims.y / worldScale, -dims.z / worldScale),
    vec3(-dims.x / worldScale, -dims.y / worldScale, -dims.z / worldScale),
    vec2((toff + ltoff).x / boxWidth, (toff + ltoff).y / boxWidth),
    vec2(((toff + ltoff).x + dims.x) / boxWidth, (toff + ltoff).y / boxWidth),
    vec2(((toff + ltoff).x + dims.x) / boxWidth, ((toff + ltoff).y + dims.y) / boxWidth),
    vec2((toff + ltoff).x / boxWidth, ((toff + ltoff).y + dims.y) / boxWidth)
  );


  return maker.AssembleRenderObject();
}

void _CreateModels()
{
  Head = _CreateBox(vec3i(8, 8, 8), vec2i());
  Hat = (AlphaRenderObject*)_CreateBox(vec3i(8, 8, 8), vec2i(32, 0));

  Body = _CreateBox(vec3i(8, 12, 4), vec2i(16, 16), true);
  Jacket = (AlphaRenderObject*)_CreateBox(vec3i(8, 12, 4), vec2i(16, 32), true);

  ArmR = _CreateBox(vec3i(4, 12, 4), vec2i(40, 16));
  ArmR2 = (AlphaRenderObject*)_CreateBox(vec3i(4, 12, 4), vec2i(40, 32));
  ArmL = _CreateBox(vec3i(4, 12, 4), vec2i(32, 48));
  ArmL2 = (AlphaRenderObject*)_CreateBox(vec3i(4, 12, 4), vec2i(48, 48));

  LegR = _CreateBox(vec3i(4, 12, 4), vec2i(0, 16));
  LegR2 = (AlphaRenderObject*)_CreateBox(vec3i(4, 12, 4), vec2i(0, 32));

  LegL = _CreateBox(vec3i(4, 12, 4), vec2i(16, 48));
  LegL2 = (AlphaRenderObject*)_CreateBox(vec3i(4, 12, 4), vec2i(0, 48));
}


void Biped::Draw(mat4 VP, vec3 pos, vec3 dir, uint32_t texture, bool use3D)
{
  static bool loaded = false; if (!loaded) _CreateModels(); loaded = true;

  Head->AssignTexture("texture0", texture);
  Hat->AssignTexture("texture0", texture);
  Body->AssignTexture("texture0", texture);
  Jacket->AssignTexture("texture0", texture);
  ArmL->AssignTexture("texture0", texture);
  ArmL2->AssignTexture("texture0", texture);
  ArmR->AssignTexture("texture0", texture);
  ArmR2->AssignTexture("texture0", texture);
  LegL->AssignTexture("texture0", texture);
  LegL2->AssignTexture("texture0", texture);
  LegR->AssignTexture("texture0", texture);
  LegR2->AssignTexture("texture0", texture);

  float worldScale = 32;
  float bodyDown = 20 / worldScale;
  float armOut = 8 / worldScale;
  float limbWidth = 4 / worldScale;
  float legDown = 24 / worldScale;

  mat4 model;
  model.Translate(pos);

  vec3 capPos = vec3() - Camera::Position();
  float headYaw = -atan2(pos.x - capPos.x, pos.z - capPos.z);
  float headPitch = DegsToRads * 270 + atan2(vec2(pos.x - capPos.x, pos.z - capPos.z).Length(), pos.y - capPos.y);
  //model.RotateY(headYaw);
  //model.RotateX(headPitch);

  model.RotateY(headYaw);

  mat4 headModel = model;

  headModel.RotateX(DegsToRads * 180 + headPitch);
  headModel.RotateZ(DegsToRads * 180);

  mat4 headMVP = VP * headModel;
  headMVP.Transpose();

  Head->Render(headMVP);

  float hatScaler = 1 + ((1.0 / 16.0) * worldScale) / 8;
  model.Scale(hatScaler);
  mat4 hatMVP = VP * model;
  hatMVP.Transpose();
  if (use3D) Hat->Render(hatMVP);
  model.Scale(1.0 / hatScaler);

  model.Translate(vec3(0, -bodyDown, 0));
  mat4 torsoMVP = VP * model;
  torsoMVP.Transpose();
  Body->Render(torsoMVP);

  vec3 jacketScaler = vec3(1.1, 1, 1.2);
  model.Scale(jacketScaler);
  mat4 jacketMVP = VP * model;
  jacketMVP.Transpose();
  if (use3D) Jacket->Render(jacketMVP);
  model.Scale(vec3(1, 1, 1) / jacketScaler);

  model.Translate(vec3(-armOut, 0, 0));
  model.Translate(vec3(-limbWidth, 0, 0));
  mat4 armRMVP = VP * model;
  armRMVP.Transpose();
  ArmR->Render(armRMVP);

  vec3 sleveScaler = vec3(1.2, 1 + 0.2 / 3.0, 1.2);

  model.Scale(sleveScaler);
  mat4 sleveMVP = VP * model;
  sleveMVP.Transpose();
  if (use3D) ArmR2->Render(sleveMVP);
  model.Scale(vec3(1, 1, 1) / sleveScaler);

  model.Translate(vec3(armOut * 2, 0, 0));
  model.Translate(vec3(limbWidth * 2, 0, 0));
  mat4 armLMVP = VP * model;
  armLMVP.Transpose();
  ArmL->Render(armLMVP);

  model.Scale(sleveScaler);
  sleveMVP = VP * model;
  sleveMVP.Transpose();
  if (use3D) ArmL2->Render(sleveMVP);
  model.Scale(vec3(1, 1, 1) / sleveScaler);

  model.Translate(vec3(-armOut, 0, 0));
  model.Translate(vec3(-limbWidth, 0, 0));

  model.Translate(vec3(0, -legDown, 0));
  model.Translate(vec3(-limbWidth, 0, 0));
  mat4 legRMVP = VP * model;
  legRMVP.Transpose();
  LegR->Render(legRMVP);

  model.Scale(sleveScaler);
  sleveMVP = VP * model;
  sleveMVP.Transpose();
  if (use3D) LegR2->Render(sleveMVP);
  model.Scale(vec3(1, 1, 1) / sleveScaler);

  model.Translate(vec3(limbWidth * 2, 0, 0));
  mat4 legLMVP = VP * model;
  legLMVP.Transpose();
  LegL->Render(legLMVP);

  model.Scale(sleveScaler);
  sleveMVP = VP * model;
  sleveMVP.Transpose();
  if (use3D) LegL2->Render(sleveMVP);
  model.Scale(vec3(1, 1, 1) / sleveScaler);

  RenderAlphaPolygons();
}
