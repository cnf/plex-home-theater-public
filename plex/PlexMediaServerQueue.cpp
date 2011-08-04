#include "log.h"
#include "PlexMediaServerQueue.h"
#include "HTTP.h"

PlexMediaServerQueue PlexMediaServerQueue::g_plexMediaServerQueue;

/////////////////////////////////////////////////////////////////////////////
PlexMediaServerQueue::PlexMediaServerQueue()
  : m_allowScrobble(true)
{
  Create();
}

/////////////////////////////////////////////////////////////////////////////
void PlexMediaServerQueue::Process()
{
  while (m_bStop == false)
  {
    // Wait for be signalled.
    m_mutex.lock();
    m_condition.wait(m_mutex);
    
    // OK, now process our queue.
    while (m_queue.size() > 0)
    {
      // Get a URL.
      pair<string, string> pair = m_queue.front();
      m_queue.pop();
      m_mutex.unlock();
      
      // Hit the Plex Media Server.
      CHTTP http;
      http.Open(pair.second, pair.first.c_str(), 0);
      CLog::Log(LOGNOTICE, "Plex Media Server Queue: %s", pair.second.c_str());

      m_mutex.lock();
    }
    
    m_mutex.unlock();
  }
  
  printf("Exiting Plex Media Server queue...\n");
}

/////////////////////////////////////////////////////////////////////////////
void PlexMediaServerQueue::StopThread()
{
  if (m_bStop)
    return;
  
  // Signal the condition.
  m_mutex.lock();
  m_bStop = true;
  m_condition.notify_one();
  m_mutex.unlock();
  
  CThread::StopThread();
}
