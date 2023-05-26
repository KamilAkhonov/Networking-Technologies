using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Server
{
    class Server
    {
        bool active_voiting = true;
        bool flagok = true;

        TcpListener listener;

        TextBox textBox1;

        SortedSet<TcpClient> connections;

        public Server(TextBox _log, int port)
        {
            textBox1 = _log;
            connections = new SortedSet<TcpClient>();
            IPEndPoint localEndPoint = new IPEndPoint(IPAddress.Any, port);
            listener = new TcpListener(localEndPoint);
        }

        public void Start()
        {
            try
            {
                listener.Start();
                while (true)
                {
                    // Set the event to nonsignaled state.  
                    TcpClient client = listener.AcceptTcpClient();

                    connections.Add(client);
                    flagok = true;

                    LogClient(client.Client);

                    Task t = new Task(() => ClientHandle(client));
                    t.Start();

                }

            }
            catch (SocketException e)
            {
                MessageBox.Show("Server stopped listening");
            }
        }

        private void LogClient(Socket client)
        {
            var ip = (IPEndPoint)client.RemoteEndPoint;
            textBox1.Invoke((MethodInvoker)delegate { textBox1.AppendText(ip.Address.ToString() + ":" + ip.Port.ToString() + " connected to server" + Environment.NewLine); });
        }

        private void LogDisconnectClient(Socket client)
        {
            var ip = (IPEndPoint)client.RemoteEndPoint;
            textBox1.Invoke((MethodInvoker)delegate { textBox1.AppendText(ip.Address.ToString() + ":" + ip.Port.ToString() + " disconnected from server" + Environment.NewLine); });
        }

        public void Stop()
        {
            listener.Stop();
            connections.Clear();
        }

        public void ClientHandle(TcpClient client)
        {
            byte[] buffer = new byte[sizeof(int)];
            var handler = client.GetStream();
            while (handler.Read(buffer,0,buffer.Length) > 0)
            {
                switch ((Command)BitConverter.ToInt32(buffer, 0))
                {
                    case Command.CloseTheVoid:
                        CloseTheVoid(handler);
                        break;
                    case Command.Voit:
                        Voit(handler);
                        break;
                }
            }
            LogDisconnectClient(client.Client);
            client.Close();
            connections.Remove(client);
        }

        public enum Command
        {
            Voit, CloseTheVoid
        }

        private void Voit(NetworkStream stream)
        {
            if (flagok && active_voiting) {
                byte[] buffer = new byte[1024];
                int bytes = stream.Read(buffer, 0, buffer.Length);
                string candidate = Encoding.UTF8.GetString(buffer, 0, bytes).Trim();

                // Прочитать файл CSV и увеличить число голосов для выбранного кандидата
                string[] lines = File.ReadAllLines("candidates.csv");
                for (int i = 0; i < lines.Length; i++)
                {
                    string[] parts = lines[i].Split(',');
                    if (parts[0].Equals(candidate, StringComparison.OrdinalIgnoreCase))
                    {
                        int count = int.Parse(parts[1]);
                        count++;
                        lines[i] = parts[0] + "," + count.ToString();
                        break;
                    }
                }
                File.WriteAllLines("candidates.csv", lines);
                flagok = false;
                // Отправить клиенту сообщение об успехе
                byte[] successMsg = Encoding.UTF8.GetBytes("успешно");
                stream.Write(successMsg, 0, successMsg.Length);

            }
            else
                {
                    byte[] successMsg = Encoding.UTF8.GetBytes("Вы уже голосовали и/или голосование было завершено");
                    stream.Write(successMsg, 0, successMsg.Length);
                }
        }

        private void CloseTheVoid(NetworkStream stream)
        {
            if (active_voiting)
            {
                // Прочитать файл CSV и сформировать список кандидатов с количеством голосов
                string[] lines = File.ReadAllLines("candidates.csv");
                StringBuilder sb = new StringBuilder();
                sb.AppendLine("Голосование успешно завершено");
                for (int i = 0; i < lines.Length; i++)
                {
                    string[] parts = lines[i].Split(',');
                    sb.AppendLine(parts[0] + ": " + parts[1]);
                }

                active_voiting = false;
                // Отправить список клиенту
                byte[] buffer = Encoding.UTF8.GetBytes(sb.ToString());
                stream.Write(buffer, 0, buffer.Length);
            }
            else 
            {
                // Прочитать файл CSV и сформировать список кандидатов с количеством голосов
                string[] lines = File.ReadAllLines("candidates.csv");
                StringBuilder sb = new StringBuilder();
                sb.AppendLine("Голосование уже завершено, вот результаты:");
                for (int i = 0; i < lines.Length; i++)
                {
                    string[] parts = lines[i].Split(',');
                    sb.AppendLine(parts[0] + ": " + parts[1]);
                }

                // Отправить список клиенту
                byte[] buffer = Encoding.UTF8.GetBytes(sb.ToString());
                stream.Write(buffer, 0, buffer.Length);
            }

        }


    }


}
