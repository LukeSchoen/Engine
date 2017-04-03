#include "GLTypes.h"

int GetUniformSize(GLUniformType dataType)
{
  switch (dataType)
  {
  case UT_1f:
    return sizeof(float);
    break;
  case UT_2f:
    return sizeof(float) * 2;
    break;
  case UT_3f:
    return sizeof(float) * 3;
    break;
  case UT_4f:
    return sizeof(float) * 4;
    break;

  case UT_1i:
    return sizeof(int);
    break;
  case UT_2i:
    return sizeof(int) * 2;
    break;
  case UT_3i:
    return sizeof(int) * 3;
    break;
  case UT_4i:
    return sizeof(int) * 4;
    break;

  case UT_1fv:
    return sizeof(float);
    break;
  case UT_2fv:
    return sizeof(float) * 2;
    break;
  case UT_3fv:
    return sizeof(float) * 3;
    break;
  case UT_4fv:
    return sizeof(float) * 4;
    break;

  case UT_1iv:
    return sizeof(int);
    break;
  case UT_2iv:
    return sizeof(int) * 2;
    break;
  case UT_3iv:
    return sizeof(int) * 3;
    break;
  case UT_4iv:
    return sizeof(int) * 4;
    break;

  case UT_mat2fv:
    return sizeof(float) * 2 * 2;
    break;
  case UT_mat3fv:
    return sizeof(float) * 3 * 3;
    break;
  case UT_mat4fv:
    return sizeof(float) * 4 * 4;
    break;

  default:
    return -1;
    break;
  }
}

int GetAttributeSize(GLAttributeType dataType)
{
  switch (dataType)
  {
  case AT_BYTE:
    return sizeof(char);
    break;
  case AT_BYTE_NORM:
    return sizeof(char);
    break;
  case AT_UNSIGNED_BYTE:
    return sizeof(char);
    break;
  case AT_UNSIGNED_BYTE_NORM:
    return sizeof(char);
    break;

  case AT_SHORT:
    return sizeof(short);
    break;
  case AT_UNSIGNED_SHORT:
    return sizeof(short);
    break;

  case AT_INT:
    return sizeof(int);
    break;
  case AT_UNSIGNED_INT:
    return sizeof(int);
    break;

  case AT_HALF_FLOAT:
    return sizeof(float) / 2;
    break;

  case AT_FLOAT:
    return sizeof(float);
    break;

  case AT_DOUBLE:
    return sizeof(double);
    break;
  default:
    return -1;
    break;
  }
}
