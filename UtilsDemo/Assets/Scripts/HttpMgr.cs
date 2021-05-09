using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Text;
using System.Threading;
using UnityEngine;

public class HttpMgr : Singletion<HttpMgr>
{
    public class JobInfo
    {
        public int id;
        public Thread thread = null;
        public float progress = 0;
        public bool finished = false;

        public int errorCode = 0;
        public string errorStr = "";
        public string text = "";
        public byte[] bytes = null;
        public Texture2D texture = null;

        public Action<float> progressCallback = null;
        public Action<JobInfo> finishedCallback = null;

        public JobInfo()
        {
        }

        public JobInfo(int id)
        {
            this.id = id;
        }

        public void Success()
        {
            //Debug.Log("Success");
            errorCode = 0;
            errorStr = "success";
            finished = true;
            progress = 1;
        }

        public void Failure(int errorCode, string errorStr)
        {
            this.text = "";
            this.errorCode = errorCode;
            this.errorStr = errorStr;
            finished = true;
            progress = 1;
        }

        public void Progress()
        {
            if (progressCallback != null)
                progressCallback(progress);
        }

        public void Finished()
        {
            if (finishedCallback != null)
                finishedCallback(this);
        }
    }

    private Dictionary<int, JobInfo> jobs = new Dictionary<int, JobInfo>();
    private Queue<int> waitingJobs = new Queue<int>();
    private List<int> workingJobs = new List<int>();
    private int lastId = 0;
    private int MAX_WORKING = 5;
    private int TIMEOUT = 30000;

    private int AddJob(Action<JobInfo> work, Action<JobInfo> finish, Action<float> progress)
    {
        JobInfo job = new JobInfo(lastId++);
        job.progressCallback = progress;
        job.finishedCallback = finish;
        job.thread = new Thread((obj) =>
        {
            work((JobInfo)obj);
        });
        job.thread.IsBackground = true;

        jobs.Add(job.id, job);
        waitingJobs.Enqueue(job.id);

        Tick();
        return job.id;
    }

    public void Tick()
    {
        while (workingJobs.Count < MAX_WORKING && waitingJobs.Count > 0)
        {
            int id = waitingJobs.Dequeue();
            if (jobs.ContainsKey(id))
            {
                workingJobs.Add(id);
                jobs[id].thread.Start(jobs[id]);
            }
        }

        for (int i = 0; i < workingJobs.Count; i++)
        {
            if (jobs.ContainsKey(workingJobs[i]))
            {
                JobInfo job = jobs[workingJobs[i]];
                job.Progress();
                if (job.finished)
                {
                    workingJobs[i] = -1;
                    job.Finished();
                    job.thread.Abort();
                    jobs.Remove(job.id);
                }
            }
            else
            {
                workingJobs[i] = -1;
            }
        }

        int count = workingJobs.Count;
        for (int i = 0; i < count; i++)
        {
            if (!workingJobs.Remove(-1)) break;
        }
    }

    public void Stop(int id)
    {
        if (jobs.ContainsKey(id))
        {
            JobInfo job = jobs[id];
            job.thread.Abort();
            jobs.Remove(id);
            job = null;
        }
    }

    public void StopAll()
    {
        foreach (KeyValuePair<int, JobInfo> kv in jobs)
        {
            kv.Value.thread.Abort();
        }
        jobs.Clear();
        waitingJobs.Clear();
        workingJobs.Clear();
    }

    public int Get(string url, Action<int, string> finish)
    {
        //Debug.Log ("ThreadID: " + Thread.CurrentThread.ManagedThreadId);
        return Request(url, "GET", null, finish);
    }
    public int GetBytes(string url, Action<int, byte[]> finish)
    {
        return Request(url, "GET", null, finish);
    }
    public int GetTexture2D(string url, Action<int, Texture2D> finish)
    {
        return Request(url, "GET", null, finish);
    }


    public int Post(string url, Dictionary<string, string> param, Action<int, string> finish)
    {
        string paramStr = "";
        if (param != null && param.Count > 0)
        {
            foreach (KeyValuePair<string, string> kv in param)
            {
                paramStr += string.Format("{0}={1}&", kv.Key, kv.Value);
            }
            paramStr = paramStr.Substring(0, paramStr.Length - 1);
        }
        return Request(url, "POST", paramStr, finish);
    }

    public int PostWithString(string url, string param, Action<int, string> finish)
    {
        return Request(url, "POST", param, finish);
    }

    private int Request<T>(string url, string method, string param, Action<int, T> finish)
    {
        return AddJob((JobInfo job) =>
        {
            DateTime startTime = DateTime.Now;
            HttpWebRequest req = (HttpWebRequest)WebRequest.Create(url);
            req.Method = method;
            req.Timeout = TIMEOUT;
            try
            {
                if (method == "POST")
                {
                    byte[] bs = Encoding.UTF8.GetBytes(param);
                    using (Stream s = req.GetRequestStream())
                    {
                        s.Write(bs, 0, bs.Length);
                    }
                }
                using (HttpWebResponse resp = (HttpWebResponse)req.GetResponse())
                {
                    if (resp.StatusCode == HttpStatusCode.OK)
                    {
                        using (Stream reader = resp.GetResponseStream())
                        {
                            using (MemoryStream ms = new MemoryStream())
                            {
                                byte[] buffer = new byte[1024];
                                int size = 0;
                                while ((size = reader.Read(buffer, 0, buffer.Length)) > 0)
                                {
                                    ms.Write(buffer, 0, size);
                                }
                                job.bytes = ms.ToArray();
                                job.Success();
                            }
                        }
                    }
                    else
                    {
                        job.Failure((int)resp.StatusCode, resp.StatusDescription);
                        Debug.Log(string.Format("HTTP_ERROR:code={0}, error={1}", resp.StatusCode, resp.StatusDescription));
                    }
                }
            }
            catch (WebException e)
            {
                job.Failure((int)e.Status, e.Message);
                Debug.Log(string.Format("HTTP_ERROR:code={0}, error={1}", e.Status, e.Message));
            }
            catch (ThreadAbortException e)
            {
                Debug.Log(string.Format("HTTP_ERROR:code={0}, error={1}", -1, e.Message));
            }
            catch (Exception e)
            {
                job.Failure(-1, e.Message);
                Debug.Log(string.Format("HTTP_ERROR:code={0}, error={1}", -1, e.Message));
            }
            finally
            {
                TimeSpan span = DateTime.Now - startTime;
				Debug.Log(string.Format("HttpMgr.Request cost {0} seconds", span.TotalSeconds));
            }
        }, (job) =>
        {
            if (finish != null)
            {
                if (finish is Action<int, string>)
                {
                    if(job.bytes != null)
                    {
                        job.text = Encoding.UTF8.GetString(job.bytes);
                    }
                    finish.DynamicInvoke(job.errorCode, job.errorCode == 0 ? job.text : job.errorStr);
                }
                else if (finish is Action<int, byte[]>)
                {
                    finish.DynamicInvoke(job.errorCode, job.bytes);
                }
                else if (finish is Action<int, Texture2D>)
                {
                    job.texture = new Texture2D(0, 0);
                    job.texture.LoadImage(job.bytes);
                    //Debug.Log(job.texture.width+","+job.texture.height);
                    finish.DynamicInvoke(job.errorCode, job.texture);
                }
            }
        }, null);
    }

    public int Upload(string url, string file, Action<int, string> finish, Action<float> progress)
    {
        if (!File.Exists(file))
        {
            if (finish != null)
                finish(-1, "file not exists");
            Debug.Log(string.Format("HTTP_ERROR:code={0}, error={1}", -1, "file not exists"));
            return -1;
        }
        string fileName = Path.GetFileName(file);
        // 要上传的文件   
        FileStream fileStream = new FileStream(file, FileMode.Open, FileAccess.Read);
        return Upload(url, fileName, fileStream, finish, progress);
    }

    public int Upload(string url, string fileName, byte[] fileBytes, Action<int, string> finish, Action<float> progress)
    {
        return Upload(url, fileName, new MemoryStream(fileBytes), finish, progress);
    }

    public int Upload(string url, string fileName, Stream fileStream, Action<int, string> finish, Action<float> progress)
    {
        return AddJob((JobInfo job) =>
        {
            DateTime startTime = DateTime.Now;
            //时间戳   
            string boundary = "---------------------------" + DateTime.Now.Ticks.ToString("x");
            byte[] boundaryBytes = Encoding.ASCII.GetBytes("\r\n--" + boundary + "\r\n");
            byte[] endBytes = Encoding.ASCII.GetBytes("\r\n--" + boundary + "--\r\n");

            //请求头部信息   
            string header = string.Format("Content-Disposition: form-data; name=\"file\";filename=\"{0}\"\r\nContent-Type: application/octet-stream\r\n\r\n", fileName);
            byte[] headerBytes = Encoding.UTF8.GetBytes(header);

            // 根据uri创建HttpWebRequest对象   
            HttpWebRequest req = (HttpWebRequest)WebRequest.Create(url);
            req.Method = "POST";
            req.KeepAlive = true;
            req.Credentials = CredentialCache.DefaultCredentials;
            req.Timeout = TIMEOUT;
            req.ContentType = "multipart/form-data; boundary=" + boundary;
            req.AllowWriteStreamBuffering = false;

            long fileLength = fileStream.Length;
            long length = boundaryBytes.Length + headerBytes.Length + fileLength + endBytes.Length;
            req.ContentLength = length;

            try
            {
                using (Stream stream = req.GetRequestStream())
                {
                    stream.Write(boundaryBytes, 0, boundaryBytes.Length);
                    stream.Write(headerBytes, 0, headerBytes.Length);

                    byte[] buffer = new byte[4096];
                    int size = 0;
                    long offset = 0;
                    while ((size = fileStream.Read(buffer, 0, buffer.Length)) != 0)
                    {
                        stream.Write(buffer, 0, size);
                        offset += size;
                        job.progress = Mathf.Min((offset + 0f) / fileLength, 0.99f);
                    }
                    stream.Write(endBytes, 0, endBytes.Length);
                }
                using (HttpWebResponse resp = (HttpWebResponse)req.GetResponse())
                {
                    job.progress = 1;
                    if (resp.StatusCode == HttpStatusCode.OK)
                    {
                        using (StreamReader reader = new StreamReader(resp.GetResponseStream(), Encoding.UTF8))
                        {
                            job.text = reader.ReadToEnd();
                            job.Success();
                        }
                    }
                    else
                    {
                        job.Failure((int)resp.StatusCode, resp.StatusDescription);
                        Debug.Log(string.Format("HTTP_ERROR:code={0}, error={1}", resp.StatusCode, resp.StatusDescription));
                    }
                }

            }
            catch (WebException e)
            {
                job.Failure((int)e.Status, e.Message);
                Debug.Log(string.Format("HTTP_ERROR:code={0}, error={1}", e.Status, e.Message));
            }
            catch (ThreadAbortException e)
            {
                Debug.Log(string.Format("HTTP_ERROR:code={0}, error={1}", -1, e.Message));
            }
            catch (Exception e)
            {
                job.Failure(-1, e.Message);
                Debug.Log(string.Format("HTTP_ERROR:code={0}, error={1}", -1, e.Message));
            }
            finally
            {
                fileStream.Close();
                TimeSpan span = DateTime.Now - startTime;
				Debug.Log(string.Format("HttpMgr.Upload cost {0} seconds", span.TotalSeconds));
            }
        }, (job) =>
        {
            if (finish != null)
            {
                finish(job.errorCode, job.errorCode == 0 ? job.text : job.errorStr);
            }
        }, progress);

    }

    public int Download(string url, string filePath, Action<int, string> finish, Action<float> progress)
    {
        return AddJob((JobInfo job) =>
        {
            DateTime startTime = DateTime.Now;

            string _filePath = filePath.Replace('\\', '/');
            string dir = Directory.GetParent(_filePath).FullName;
            if (!Directory.Exists(dir))
            {
                Directory.CreateDirectory(dir);
            }
            string fileName = Path.GetFileName(_filePath);
            string tempFileName = fileName + ".download";
            string tempFilePath = dir + "/" + tempFileName;

            try
            {
                using (FileStream fileStream = new FileStream(tempFilePath, FileMode.OpenOrCreate, FileAccess.Write))
                {
                    //获取文件现在的长度
                    long offset = fileStream.Length;
                    HttpWebRequest req = (HttpWebRequest)WebRequest.Create(url);
                    //断点续传核心，设置远程访问文件流的起始位置
                    req.AddRange((int)offset);
                    HttpWebResponse resp = (HttpWebResponse)req.GetResponse();
                    long totalLength = resp.ContentLength;
                    if (resp.StatusCode == HttpStatusCode.PartialContent)
                    {
                        //断点续传核心，设置本地文件流的起始位置
                        fileStream.Seek(offset, SeekOrigin.Begin);
                        totalLength += offset;
                    }
                    else
                    {
                        offset = 0;
                    }
                    if (resp.StatusCode == HttpStatusCode.PartialContent || resp.StatusCode == HttpStatusCode.OK)
                    {
                        using (Stream stream = resp.GetResponseStream())
                        {
                            byte[] buffer = new byte[1024];
                            int size = 0;
                            while ((size = stream.Read(buffer, 0, buffer.Length)) > 0)
                            {
                                //将内容再写入本地文件中
                                fileStream.Write(buffer, 0, size);
                                //计算进度
                                offset += size;
                                job.progress = (offset + 0f) / totalLength;
                            }
                        }
                    }
                    else
                    {
                        job.Failure((int)resp.StatusCode, resp.StatusDescription);
                        Debug.Log(string.Format("HTTP_ERROR:code={0}, error={1}", resp.StatusCode, resp.StatusDescription));
                        return;
                    }
                }
                if (File.Exists(_filePath)) File.Delete(_filePath);
                File.Move(tempFilePath, _filePath);
                job.Success();
            }
            catch (WebException e)
            {
                job.Failure((int)e.Status, e.Message);
                Debug.Log(string.Format("HTTP_ERROR:code={0}, error={1}", e.Status, e.Message));
            }
            catch (ThreadAbortException e)
            {
                Debug.Log(string.Format("HTTP_ERROR:code={0}, error={1}", -1, e.Message));
            }
            catch (Exception e)
            {
                job.Failure(-1, e.Message);
                Debug.Log(string.Format("HTTP_ERROR:code={0}, error={1}", -1, e.Message));
            }
            finally
            {
                TimeSpan span = DateTime.Now - startTime;
                //Debug.Log(string.Format("HttpMgr.Download cost {0} seconds", span.TotalSeconds));
            }

        }, (job) =>
        {
            if (finish != null)
            {
                finish(job.errorCode, job.errorCode == 0 ? job.text : job.errorStr);
            }
        }, progress);
    }
}
