using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.Web;
using System.Web.Script.Serialization;

namespace compilembed
{
    public class CompileMessage
    {
        public string action { get; set; }
        public string severity { get; set; }
        public string type { get; set; }
        public double percent { get; set; }
        public string file { get; set; }
        public string line { get; set; }
        public string text { get; set; }
        public string message { get; set; }
    }

    public class CompileResultData
    {
        public bool task_complete { get; set; }
        public string task_status { get; set; }
        public string task_id { get; set; }
        public CompileMessage[] new_messages { get; set; }
        public bool compilation_success { get; set; }
        public string binary { get; set; }
        public string program { get; set; }
    }

    public class CompileResult
    {
        public CompileResultData data { get; set; }
    }

    public class CompileStartResponse
    {

        public int code { get; set; }
        public string[] errors { get; set; }
        public CompileResult result { get; set; }
    }
    public class MBEDOnlineCompile
    {
        private string authInfo = string.Empty;
        private string taskId = string.Empty;
        private CompileStartResponse lastResponse = null;

        public MBEDOnlineCompile(string userName, string password)
        {
            authInfo = userName + ":" + password;
            authInfo = Convert.ToBase64String(Encoding.Default.GetBytes(authInfo));
        }

        public void StartCompile(string platform, string program = null, string repo = null, bool clean = false)
        {
            int retryCount = 5;
            while (true)
            {
                try
                {
                    WebRequest request = WebRequest.Create("https://developer.mbed.org/api/v2/tasks/compiler/start/");
                    request.Headers["Authorization"] = "Basic " + authInfo;
                    request.Method = "POST";
                    request.ContentType = "application/x-www-form-urlencoded";
                    string requestPayload = string.Format("platform={0}&clean={1}", platform, clean ? "True" : "False");
                    if (!string.IsNullOrEmpty(program))
                    {
                        requestPayload += string.Format("&program={0}", program);
                    }
                    if (!string.IsNullOrEmpty(repo))
                    {
                        requestPayload += string.Format("&repo={0}", repo);
                    }
                    byte[] encodedPayload = Encoding.UTF8.GetBytes(requestPayload);
                    request.GetRequestStream().Write(encodedPayload, 0, Encoding.UTF8.GetBytes(requestPayload).Length);
                    WebResponse response = request.GetResponse();
                    CompileStartResponse compileResponse = new JavaScriptSerializer().Deserialize<CompileStartResponse>(new StreamReader(response.GetResponseStream()).ReadToEnd());
                    taskId = compileResponse.result.data.task_id;
                    lastResponse = compileResponse;

                    break;
                }
                catch (WebException webException)
                {
                    retryCount--;
                    System.Console.WriteLine("Error sending request :{0}, retries left: {1}", webException.Message, retryCount);

                    if (retryCount == 0)
                    {
                        throw webException;
                    }
                }
            }
        }

        public bool PollStatus(ICollection<string> messages, out bool failed)
        {
            string requestURL = string.Format("https://developer.mbed.org/api/v2/tasks/compiler/output/{0}", HttpUtility.UrlEncode(taskId));
            bool complete = false;
            failed = true;
            CompileStartResponse compileResponse;

            int retryCount = 5;
            while (true)
            {
                try
                {
                    WebRequest request = WebRequest.Create(requestURL);
                    request.Headers["Authorization"] = "Basic " + authInfo;
                    request.Method = "GET";

                    // 30s should be enough
                    request.Timeout = 30000;
                    WebResponse response = request.GetResponse();
                    string jsonResponse = new StreamReader(response.GetResponseStream()).ReadToEnd();

                    compileResponse = new JavaScriptSerializer().Deserialize<CompileStartResponse>(jsonResponse);
                    lastResponse = compileResponse;

                    foreach (CompileMessage msg in compileResponse.result.data.new_messages)
                    {
                        if (msg.type == "cc")
                        {
                            messages.Add(string.Format("{0}: {1}: {2}: {3}", msg.severity, msg.file, msg.line, msg.message));
                            messages.Add(string.Format("{0}", msg.text));
                        }
                        else if (msg.type == "progress")
                        {
                            messages.Add(string.Format("{0}: {1} ({2:F} %)", msg.action, msg.file, msg.percent));
                        }
                        else if (msg.type == "tool_error")
                        {
                            messages.Add(string.Format("{0}: {1}", msg.type, msg.message));
                        }
                    }

                    failed = !compileResponse.result.data.compilation_success;
                    if (failed && compileResponse.result.data.task_complete)
                    {
                        messages.Add(string.Format("Raw response: {0}", jsonResponse));
                    }

                    complete = compileResponse.result.data.task_complete;

                    break;
                }
                catch (WebException webException)
                {
                    retryCount--;
                    System.Console.WriteLine("Error sending request :{0}, retries left: {1}", webException.Message, retryCount);

                    if (retryCount == 0)
                    {
                        throw webException;
                    }
                }
            }

            if (retryCount < 0)
            {
                complete = true;
            }

            return complete;
        }

        public void DownloadBin(string downloadPath)
        {
            if (lastResponse == null ||
                lastResponse.result == null ||
                lastResponse.result.data == null ||
                String.IsNullOrWhiteSpace(lastResponse.result.data.program) ||
                String.IsNullOrWhiteSpace(lastResponse.result.data.binary))
            {
                return;
            }

            if (File.Exists(downloadPath))
            {
                File.Delete(downloadPath);
            }

            string requestQuery = string.Format("binary={0}&program={1}&repomode={2}&task_id={3}", lastResponse.result.data.binary, lastResponse.result.data.program, "True", taskId);
            string requestUrl = String.Format("{0}?{1}", "https://developer.mbed.org/api/v2/tasks/compiler/bin/", requestQuery);

            int retryCount = 5;
            while (true)
            {
                try
                {
                    WebRequest request = WebRequest.Create(requestUrl);
                    request.Headers["Authorization"] = "Basic " + authInfo;
                    request.Method = "GET";

                    WebResponse response = request.GetResponse();

                    BinaryReader br = new BinaryReader(response.GetResponseStream());
                    FileStream fs = new FileStream(downloadPath, FileMode.CreateNew);

                    int bytesRead = 0;
                    byte[] buffer = new byte[256];

                    while ((bytesRead = br.Read(buffer, 0, buffer.Length)) > 0)
                    {
                        fs.Write(buffer, 0, bytesRead);
                    }

                    fs.Close();
                    br.Close();

                    break;
                }
                catch (WebException webException)
                {
                    retryCount--;
                    System.Console.WriteLine("Error sending request :{0}, retries left: {1}", webException.Message, retryCount);

                    if (retryCount == 0)
                    {
                        throw webException;
                    }
                }
            }
        }
    }
}
