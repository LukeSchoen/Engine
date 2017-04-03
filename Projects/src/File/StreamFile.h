#ifndef StreamFile_h__
#define StreamFile_h__
#include <stdint.h>
#include <stdio.h>
#include <string>

const int64_t defaultBufferSize = 16/*MB*/ * 1024 * 1024;

struct StreamFileReader
{
  StreamFileReader(const char *a_filePath = nullptr, int64_t *fileSize = nullptr, int64_t a_bufferSize = defaultBufferSize); // Ensure size > 0 before using ReadBytes

  ~StreamFileReader();

  int64_t StartStream(const char *a_filePath, int64_t a_bufferSize = defaultBufferSize); // Ensure return > 0 before using ReadBytes
  void StopStream();

  void FlushStream();

  void *ReadBytes(int64_t a_requestSize, int64_t *a_bytesRead = nullptr);
  void *ReadToEnd(int64_t *a_bytesRead = nullptr);
  int64_t ReadBytes(void *a_pData, int64_t a_requestSize);

  int64_t GetLocation();

  void SetLocation(int64_t a_location);

  int64_t m_bufferSize;
  int64_t m_bufferHead;
  int64_t m_fileHead;
  int64_t m_fileSize;
  void *m_buffer;
  FILE *m_pFile;
};

struct StreamFileWriter
{
  StreamFileWriter(const char *a_filePath = nullptr, int64_t *fileSize = nullptr, int64_t a_bufferSize = defaultBufferSize); // Ensure size > 0 before using ReadBytes

  ~StreamFileWriter();

  bool StartStream(const char *a_filePath, int64_t a_bufferSize = defaultBufferSize);  // Ensure return >0 before using WriteBytes
  void StopStream();

  void FlushStream();

  void WriteBytes(const void *a_data, int64_t a_dataSize);

  int64_t GetLocation();

  void SetLocation(int64_t a_location);

  int64_t m_bufferSize;
  int64_t m_bufferHead;
  int64_t m_fileHead;
  int64_t m_fileSize;
  void *m_buffer;
  FILE *m_pFile;
  std::string m_filePath;
  bool used;
};

#endif // StreamFile_h__
