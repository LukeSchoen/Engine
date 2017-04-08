#include "StreamFile.h"
#include "File.h"
#include "Math.h"
#include <Memory.h>

StreamFileReader::StreamFileReader(const char *a_filePath, int64_t *fileSize, int64_t a_bufferSize)
{
  int64_t file = StartStream(a_filePath, a_bufferSize);
  if (fileSize)
    *fileSize = file;
}

int64_t StreamFileReader::StartStream(const char *a_filePath, int64_t a_bufferSize)
{
  m_bufferSize = 0;
  m_bufferHead = 0;
  m_fileSize = 0;
  m_fileHead = 0;
  m_buffer = nullptr;
  m_pFile = nullptr;

  if (!a_filePath)
    return 0;
  StopStream();
  fopen_s(&m_pFile, a_filePath, "rb");
  if (!m_pFile)
    return 0;
  m_bufferSize = a_bufferSize;
  m_buffer = malloc(m_bufferSize);
  _fseeki64(m_pFile, 0L, SEEK_END);
  m_fileSize = _ftelli64(m_pFile);
  _fseeki64(m_pFile, 0L, SEEK_SET);
  return m_fileSize;
}


StreamFileReader::~StreamFileReader()
{
  StopStream();
}


void StreamFileReader::StopStream()
{
  m_bufferSize = 0;
  m_fileHead = 0;
  m_bufferHead = 0;
  m_fileSize = 0;
  if (m_buffer)
    free(m_buffer);
  if (m_pFile)
    fclose(m_pFile);
  m_buffer = nullptr;
  m_pFile = nullptr;
}


void StreamFileReader::FlushStream()
{
  m_bufferHead = 0;
}


void *StreamFileReader::ReadBytes(int64_t a_requestSize, int64_t *a_bytesRead)
{
  if (a_requestSize <= 0)
  {
    if (a_bytesRead) *a_bytesRead = 0;
    return nullptr; // bad request
  }
  if (m_fileHead + a_requestSize > m_fileSize) a_requestSize = m_fileSize - m_fileHead; // eof 
  if (a_requestSize > m_bufferSize) // expand buffer
  {
    m_bufferSize = a_requestSize;
    free(m_buffer);
    m_buffer = malloc(m_bufferSize);
    m_bufferHead = 0;
  }
  if (m_bufferHead + a_requestSize > m_bufferSize) m_bufferHead = 0; // request more data
  if (m_bufferHead == 0) fread(m_buffer, 1, m_bufferSize, m_pFile); // Read Data
  m_fileHead += a_requestSize;
  m_bufferHead += a_requestSize;
  if (a_bytesRead) *a_bytesRead = a_requestSize;
  return (char*)m_buffer + m_bufferHead - a_requestSize;
}


int64_t StreamFileReader::ReadBytes(void *pData, int64_t a_requestSize)
{
  int64_t readSize;
  void *readData = ReadBytes(a_requestSize, &readSize);
  memcpy(pData, readData, readSize);
  return readSize;
}

void *StreamFileReader::ReadToEnd(int64_t *a_numOfBytesRead)
{
  int64_t start = GetLocation();
  SetLocation(-1);
  int64_t end = GetLocation();
  SetLocation(start);
  return ReadBytes(end - start);
}

int64_t StreamFileReader::GetLocation()
{
  return m_fileHead;
}

void StreamFileReader::SetLocation(int64_t a_location)
{
  FlushStream();
  if(a_location < 0)
    _fseeki64(m_pFile, 0, SEEK_END); 
  else
    _fseeki64(m_pFile, a_location, SEEK_SET);
  m_fileHead = _ftelli64(m_pFile);
}


StreamFileWriter::StreamFileWriter(const char *a_filePath, int64_t *fileSize, int64_t a_bufferSize)
{
  int64_t file = StartStream(a_filePath, a_bufferSize);
  if (fileSize)
    *fileSize = file;
  used = false;
}

bool StreamFileWriter::StartStream(const char *a_filePath, int64_t a_bufferSize)
{
  m_filePath = a_filePath;
  m_bufferSize = 0;
  m_fileHead = 0;
  m_bufferHead = 0;
  m_fileSize = 0;
  m_buffer = nullptr;
  m_pFile = nullptr;

  StopStream();
  fopen_s(&m_pFile, a_filePath, "wb");
  if (!m_pFile)
    return false;
  m_bufferSize = a_bufferSize;
  m_buffer = malloc(m_bufferSize);
  return true;
}


StreamFileWriter::~StreamFileWriter()
{
  StopStream();
}


void StreamFileWriter::StopStream()
{
  FlushStream();
  m_fileHead = 0;
  if (m_pFile)
    fclose(m_pFile);
  if (!used) File::DelFile(m_filePath.c_str());
  if (m_buffer)
    free(m_buffer);
  m_buffer = nullptr;
  m_pFile = nullptr;
}


void StreamFileWriter::FlushStream()
{
  if (m_bufferHead)
    fwrite(m_buffer, m_bufferHead, 1, m_pFile);
  m_bufferHead = 0;
}


void StreamFileWriter::WriteBytes(const void *a_data, int64_t a_dataSize)
{
  if (a_dataSize > 0) used = true;
  m_fileHead += a_dataSize;
  if (a_dataSize > (m_bufferSize - m_bufferHead))
  {
    if (m_bufferHead)
      fwrite(m_buffer, m_bufferHead, 1, m_pFile);
    m_bufferHead = 0;
  }
  if (a_dataSize <= m_bufferSize)
  {
    memcpy((char*)m_buffer + m_bufferHead, a_data, a_dataSize);
    m_bufferHead += a_dataSize;
  }
  else
    fwrite(a_data, a_dataSize, 1, m_pFile);
}

int64_t StreamFileWriter::GetLocation()
{
  return m_fileHead;
}

void StreamFileWriter::SetLocation(int64_t a_location)
{
  FlushStream();
  if (a_location < 0)
    _fseeki64(m_pFile, 0, SEEK_END);
  else
    _fseeki64(m_pFile, a_location, SEEK_SET);
  m_fileHead = _ftelli64(m_pFile);
}
