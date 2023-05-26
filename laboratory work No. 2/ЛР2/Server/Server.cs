using System.Net;
using System.Net.Sockets;
using System.Text;

namespace Server
{

    class Server
    {

        Socket listener;

        TextBox log;

        SortedSet<Socket> connections;
        ProductService fileservice;

        public Server(TextBox _log)
        {
            log = _log;
            connections = new SortedSet<Socket>();
            fileservice = new ProductService();
        }
        public void Start(int port)
        {
            IPEndPoint localEndPoint = new IPEndPoint(IPAddress.Any, port);

            listener = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            try
            {
                listener.Bind(localEndPoint);
                listener.Listen(100);

                while (true)
                {
                    // Set the event to nonsignaled state.  
                    Socket client = listener.Accept();

                    connections.Add(client);

                    LogClient(client);

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
            log.Invoke((MethodInvoker)delegate { log.AppendText(ip.Address.ToString() + ":" + ip.Port.ToString() + " connected to server" + Environment.NewLine); });
        }
        private void LogDisconnectClient(Socket client)
        {
            var ip = (IPEndPoint)client.RemoteEndPoint;
            log.Invoke((MethodInvoker)delegate { log.AppendText(ip.Address.ToString() + ":" + ip.Port.ToString() + " disconnected from server" + Environment.NewLine); });
        }

        public void Stop()
        {
            listener.Shutdown(SocketShutdown.Both);
            listener.Close();
            connections.Clear();
        }
        public void ClientHandle(Socket client)
        {
            byte[] buffer = new byte[sizeof(int)];
            while (client.Receive(buffer, buffer.Length, SocketFlags.None) > 0)
            {
                switch ((Command)BitConverter.ToInt32(buffer, 0))
                {
                    case Command.GetProducts:
                        GetList(client);
                        break;
                    case Command.ChangePrice:
                        ChangePrice(client);
                        break;
                }
            }
            LogDisconnectClient(client);
            client.Shutdown(SocketShutdown.Both);
            client.Close();
            connections.Remove(client);
        }
        public enum Command
        {
            GetProducts,
            ChangePrice
        }

        private void GetList(Socket handler)
        {
            string result = fileservice.GetProducts();
            handler.Send(BitConverter.GetBytes(result.Length));
            handler.Send(Encoding.UTF8.GetBytes(result));
        }
        private void ChangePrice(Socket handler)
        {
            string product_name = RecvString(handler);
            int price = int.Parse(RecvString(handler));
            fileservice.ChangePrice(product_name, price);
        }

        private string RecvString(Socket handler)
        {
            byte[] buffer = new byte[sizeof(int)];
            handler.Receive(buffer);
            buffer = new byte[BitConverter.ToInt32(buffer)];
            handler.Receive(buffer);

            return Encoding.UTF8.GetString(buffer);
        }
    }
}
